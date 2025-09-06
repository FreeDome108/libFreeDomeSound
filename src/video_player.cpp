#include "video_player.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <random>

// Внешние библиотеки для видео (будут добавлены в CMakeLists.txt)
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
}

using namespace AnantaSound;

VinylVideoPlayer::VinylVideoPlayer() 
    : state_(PlaybackState::STOPPED)
    , should_stop_(false)
    , playback_speed_(1.0)
    , current_position_(0.0)
    , loop_enabled_(false)
    , vinyl_params_() {
}

VinylVideoPlayer::~VinylVideoPlayer() {
    shutdown();
}

bool VinylVideoPlayer::initialize() {
    std::lock_guard<std::mutex> lock(player_mutex_);
    
    // Инициализация FFmpeg (в новых версиях av_register_all() устарел)
    avformat_network_init();
    
    state_ = PlaybackState::STOPPED;
    should_stop_ = false;
    
    std::cout << "VinylVideoPlayer initialized successfully" << std::endl;
    return true;
}

void VinylVideoPlayer::shutdown() {
    stop();
    
    if (playback_thread_.joinable()) {
        should_stop_ = true;
        playback_thread_.join();
    }
    
    std::lock_guard<std::mutex> lock(player_mutex_);
    state_ = PlaybackState::STOPPED;
    
    std::cout << "VinylVideoPlayer shutdown complete" << std::endl;
}

bool VinylVideoPlayer::loadVideo(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(player_mutex_);
    
    if (state_ == PlaybackState::PLAYING) {
        stop();
    }
    
    video_filepath_ = filepath;
    video_info_.filename = filepath;
    
    // Открываем файл для анализа
    AVFormatContext* format_ctx = nullptr;
    if (avformat_open_input(&format_ctx, filepath.c_str(), nullptr, nullptr) < 0) {
        std::cerr << "Could not open video file: " << filepath << std::endl;
        return false;
    }
    
    if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
        std::cerr << "Could not find stream information" << std::endl;
        avformat_close_input(&format_ctx);
        return false;
    }
    
    // Ищем видео и аудио потоки
    int video_stream = -1;
    int audio_stream = -1;
    
    for (unsigned int i = 0; i < format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && video_stream == -1) {
            video_stream = i;
        } else if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audio_stream == -1) {
            audio_stream = i;
        }
    }
    
    if (video_stream == -1) {
        std::cerr << "No video stream found" << std::endl;
        avformat_close_input(&format_ctx);
        return false;
    }
    
    // Получаем информацию о видео
    AVCodecParameters* video_codecpar = format_ctx->streams[video_stream]->codecpar;
    video_info_.width = video_codecpar->width;
    video_info_.height = video_codecpar->height;
    video_info_.duration = static_cast<double>(format_ctx->duration) / AV_TIME_BASE;
    video_info_.frame_rate = static_cast<double>(format_ctx->streams[video_stream]->r_frame_rate.num) / 
                            format_ctx->streams[video_stream]->r_frame_rate.den;
    video_info_.has_video = true;
    
    // Получаем информацию об аудио
    if (audio_stream != -1) {
        AVCodecParameters* audio_codecpar = format_ctx->streams[audio_stream]->codecpar;
        video_info_.audio_channels = audio_codecpar->ch_layout.nb_channels;
        video_info_.audio_sample_rate = audio_codecpar->sample_rate;
        video_info_.has_audio = true;
    } else {
        video_info_.has_audio = false;
    }
    
    avformat_close_input(&format_ctx);
    
    std::cout << "Video loaded: " << video_info_.width << "x" << video_info_.height 
              << ", duration: " << video_info_.duration << "s" << std::endl;
    
    return true;
}

bool VinylVideoPlayer::loadVinylRecording(const std::string& filepath) {
    if (!loadVideo(filepath)) {
        return false;
    }
    
    // Определяем тип виниловой записи и устанавливаем соответствующие параметры
    VinylUtils::VinylType vinyl_type = VinylUtils::detectVinylType(filepath);
    vinyl_params_ = VinylUtils::getRecommendedSettings(vinyl_type);
    
    std::cout << "Vinyl recording loaded with type: " << static_cast<int>(vinyl_type) << std::endl;
    return true;
}

bool VinylVideoPlayer::play() {
    std::lock_guard<std::mutex> lock(player_mutex_);
    
    if (state_ == PlaybackState::PLAYING) {
        return true;
    }
    
    if (video_filepath_.empty()) {
        std::cerr << "No video loaded" << std::endl;
        return false;
    }
    
    state_ = PlaybackState::PLAYING;
    should_stop_ = false;
    
    if (playback_thread_.joinable()) {
        playback_thread_.join();
    }
    
    playback_thread_ = std::thread(&VinylVideoPlayer::playbackLoop, this);
    
    emitEvent("playback_started");
    return true;
}

bool VinylVideoPlayer::pause() {
    std::lock_guard<std::mutex> lock(player_mutex_);
    
    if (state_ == PlaybackState::PLAYING) {
        state_ = PlaybackState::PAUSED;
        emitEvent("playback_paused");
        return true;
    }
    
    return false;
}

bool VinylVideoPlayer::stop() {
    std::lock_guard<std::mutex> lock(player_mutex_);
    
    if (state_ == PlaybackState::STOPPED) {
        return true;
    }
    
    should_stop_ = true;
    state_ = PlaybackState::STOPPED;
    current_position_ = 0.0;
    
    if (playback_thread_.joinable()) {
        playback_thread_.join();
    }
    
    emitEvent("playback_stopped");
    return true;
}

bool VinylVideoPlayer::seek(double position_seconds) {
    std::lock_guard<std::mutex> lock(player_mutex_);
    
    if (position_seconds < 0.0 || position_seconds > video_info_.duration) {
        return false;
    }
    
    current_position_ = position_seconds;
    emitEvent("seek");
    
    return true;
}

void VinylVideoPlayer::playbackLoop() {
    AVFormatContext* format_ctx = nullptr;
    AVCodecContext* video_codec_ctx = nullptr;
    AVCodecContext* audio_codec_ctx = nullptr;
    SwsContext* sws_ctx = nullptr;
    SwrContext* swr_ctx = nullptr;
    
    // Открываем файл
    if (avformat_open_input(&format_ctx, video_filepath_.c_str(), nullptr, nullptr) < 0) {
        std::cerr << "Could not open video file for playback" << std::endl;
        return;
    }
    
    if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
        std::cerr << "Could not find stream information" << std::endl;
        avformat_close_input(&format_ctx);
        return;
    }
    
    // Находим потоки
    int video_stream = -1;
    int audio_stream = -1;
    
    for (unsigned int i = 0; i < format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && video_stream == -1) {
            video_stream = i;
        } else if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audio_stream == -1) {
            audio_stream = i;
        }
    }
    
    if (video_stream == -1) {
        std::cerr << "No video stream found for playback" << std::endl;
        avformat_close_input(&format_ctx);
        return;
    }
    
    // Инициализируем декодеры
    AVCodecParameters* video_codecpar = format_ctx->streams[video_stream]->codecpar;
    const AVCodec* video_codec = avcodec_find_decoder(video_codecpar->codec_id);
    video_codec_ctx = avcodec_alloc_context3(video_codec);
    avcodec_parameters_to_context(video_codec_ctx, video_codecpar);
    avcodec_open2(video_codec_ctx, video_codec, nullptr);
    
    if (audio_stream != -1) {
        AVCodecParameters* audio_codecpar = format_ctx->streams[audio_stream]->codecpar;
        const AVCodec* audio_codec = avcodec_find_decoder(audio_codecpar->codec_id);
        audio_codec_ctx = avcodec_alloc_context3(audio_codec);
        avcodec_parameters_to_context(audio_codec_ctx, audio_codecpar);
        avcodec_open2(audio_codec_ctx, audio_codec, nullptr);
    }
    
    // Настраиваем конвертеры
    sws_ctx = sws_getContext(
        video_codec_ctx->width, video_codec_ctx->height, video_codec_ctx->pix_fmt,
        video_codec_ctx->width, video_codec_ctx->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );
    
    if (audio_codec_ctx) {
        swr_ctx = swr_alloc_set_opts(nullptr,
            AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_FLT, 44100,
            audio_codec_ctx->ch_layout.u.mask, audio_codec_ctx->sample_fmt, audio_codec_ctx->sample_rate,
            0, nullptr);
        swr_init(swr_ctx);
    }
    
    // Буферы
    AVFrame* frame = av_frame_alloc();
    AVPacket* packet = av_packet_alloc();
    
    // Основной цикл воспроизведения
    auto start_time = std::chrono::high_resolution_clock::now();
    double frame_duration = 1.0 / video_info_.frame_rate;
    
    while (!should_stop_ && state_ == PlaybackState::PLAYING) {
        // Читаем пакет
        if (av_read_frame(format_ctx, packet) < 0) {
            if (loop_enabled_) {
                av_seek_frame(format_ctx, -1, 0, AVSEEK_FLAG_BACKWARD);
                current_position_ = 0.0;
                continue;
            } else {
                break; // Конец файла
            }
        }
        
        // Обрабатываем видео пакеты
        if (packet->stream_index == video_stream) {
            int ret = avcodec_send_packet(video_codec_ctx, packet);
            if (ret == 0) {
                ret = avcodec_receive_frame(video_codec_ctx, frame);
                if (ret == 0) {
                    // Конвертируем кадр в RGB
                    AVFrame* rgb_frame = av_frame_alloc();
                    rgb_frame->format = AV_PIX_FMT_RGB24;
                    rgb_frame->width = video_codec_ctx->width;
                    rgb_frame->height = video_codec_ctx->height;
                    av_frame_get_buffer(rgb_frame, 32);
                    
                    sws_scale(sws_ctx, frame->data, frame->linesize, 0, video_codec_ctx->height,
                             rgb_frame->data, rgb_frame->linesize);
                    
                    // Создаем VideoFrame
                    VideoFrame vframe;
                    vframe.width = rgb_frame->width;
                    vframe.height = rgb_frame->height;
                    vframe.timestamp = current_position_;
                    vframe.frame_number = frame->pts;
                    
                    size_t data_size = rgb_frame->linesize[0] * rgb_frame->height;
                    vframe.data.resize(data_size);
                    memcpy(vframe.data.data(), rgb_frame->data[0], data_size);
                    
                    // Применяем виниловые эффекты
                    vframe = applyVinylEffects(vframe);
                    
                    // Отправляем кадр через коллбэк
                    if (video_callback_) {
                        video_callback_(vframe);
                    }
                    
                    av_frame_free(&rgb_frame);
                    
                    // Обновляем позицию
                    current_position_ = current_position_.load() + frame_duration * playback_speed_.load();
                }
            }
        }
        
        // Обрабатываем аудио пакеты
        if (packet->stream_index == audio_stream && audio_codec_ctx) {
            int ret = avcodec_send_packet(audio_codec_ctx, packet);
            if (ret == 0) {
                ret = avcodec_receive_frame(audio_codec_ctx, frame);
                if (ret == 0) {
                    // Конвертируем аудио
                    float* output_buffer = nullptr;
                    int output_samples = swr_get_out_samples(swr_ctx, frame->nb_samples);
                    av_samples_alloc((uint8_t**)&output_buffer, nullptr, 2, output_samples, AV_SAMPLE_FMT_FLT, 0);
                    
                    swr_convert(swr_ctx, (uint8_t**)&output_buffer, output_samples,
                               (const uint8_t**)frame->data, frame->nb_samples);
                    
                    std::vector<float> audio_data(output_buffer, output_buffer + output_samples * 2);
                    
                    if (audio_callback_) {
                        audio_callback_(audio_data, 2);
                    }
                    
                    av_freep(&output_buffer);
                }
            }
        }
        
        av_packet_unref(packet);
        
        // Синхронизация времени
        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
        double target_time = current_position_ * 1000.0;
        
        if (elapsed < target_time) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>(target_time - elapsed)));
        }
    }
    
    // Очистка ресурсов
    av_frame_free(&frame);
    av_packet_free(&packet);
    sws_freeContext(sws_ctx);
    if (swr_ctx) swr_free(&swr_ctx);
    avcodec_free_context(&video_codec_ctx);
    if (audio_codec_ctx) avcodec_free_context(&audio_codec_ctx);
    avformat_close_input(&format_ctx);
    
    emitEvent("playback_ended");
}

VideoFrame VinylVideoPlayer::applyVinylEffects(const VideoFrame& original_frame) {
    VideoFrame frame = original_frame;
    
    // Применяем различные виниловые эффекты
    applySurfaceNoise(frame);
    applyWowFlutter(frame);
    applyNeedlePressure(frame);
    applyGrooveDepth(frame);
    applyEQCurve(frame);
    
    return frame;
}

void VinylVideoPlayer::applySurfaceNoise(VideoFrame& frame) {
    if (vinyl_params_.surface_noise <= 0.0) return;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> noise_dist(0.0f, vinyl_params_.surface_noise);
    
    for (size_t i = 0; i < frame.data.size(); i += 3) { // RGB
        float noise = noise_dist(gen);
        frame.data[i] = std::clamp(frame.data[i] + noise * 255.0f, 0.0f, 255.0f);     // R
        frame.data[i + 1] = std::clamp(frame.data[i + 1] + noise * 255.0f, 0.0f, 255.0f); // G
        frame.data[i + 2] = std::clamp(frame.data[i + 2] + noise * 255.0f, 0.0f, 255.0f); // B
    }
}

void VinylVideoPlayer::applyWowFlutter(VideoFrame& frame) {
    if (vinyl_params_.wow_flutter <= 0.0) return;
    
    // Имитация вибрации от проигрывателя
    double flutter_freq = 0.1 + vinyl_params_.wow_flutter * 10.0;
    double flutter_amplitude = vinyl_params_.wow_flutter * 5.0;
    
    double phase = current_position_ * flutter_freq * 2.0 * M_PI;
    double offset_x = std::sin(phase) * flutter_amplitude;
    double offset_y = std::cos(phase * 1.3) * flutter_amplitude * 0.7;
    
    // Применяем смещение (упрощенная версия)
    for (size_t i = 0; i < frame.data.size(); i += 3) {
        // Здесь должна быть более сложная логика смещения пикселей
        // Для простоты просто добавляем небольшое искажение
        float distortion = static_cast<float>(std::sin(phase + i * 0.01) * flutter_amplitude * 0.1);
        frame.data[i] = std::clamp(frame.data[i] + distortion, 0.0f, 255.0f);
        frame.data[i + 1] = std::clamp(frame.data[i + 1] + distortion, 0.0f, 255.0f);
        frame.data[i + 2] = std::clamp(frame.data[i + 2] + distortion, 0.0f, 255.0f);
    }
}

void VinylVideoPlayer::applyNeedlePressure(VideoFrame& frame) {
    // Имитация влияния давления иглы на качество
    float pressure_factor = 1.0f - (vinyl_params_.needle_pressure - 1.0f) * 0.1f;
    pressure_factor = std::clamp(pressure_factor, 0.5f, 1.0f);
    
    for (size_t i = 0; i < frame.data.size(); i++) {
        frame.data[i] = static_cast<uint8_t>(frame.data[i] * pressure_factor);
    }
}

void VinylVideoPlayer::applyGrooveDepth(VideoFrame& frame) {
    // Имитация влияния глубины канавки на контрастность
    float depth_factor = 0.8f + vinyl_params_.groove_depth * 0.4f;
    
    for (size_t i = 0; i < frame.data.size(); i += 3) {
        // Увеличиваем контрастность
        float r = (frame.data[i] / 255.0f - 0.5f) * depth_factor + 0.5f;
        float g = (frame.data[i + 1] / 255.0f - 0.5f) * depth_factor + 0.5f;
        float b = (frame.data[i + 2] / 255.0f - 0.5f) * depth_factor + 0.5f;
        
        frame.data[i] = static_cast<uint8_t>(std::clamp(r * 255.0f, 0.0f, 255.0f));
        frame.data[i + 1] = static_cast<uint8_t>(std::clamp(g * 255.0f, 0.0f, 255.0f));
        frame.data[i + 2] = static_cast<uint8_t>(std::clamp(b * 255.0f, 0.0f, 255.0f));
    }
}

void VinylVideoPlayer::applyEQCurve(VideoFrame& frame) {
    // Простая имитация эквализации
    float eq_factor = vinyl_params_.eq_curve;
    
    for (size_t i = 0; i < frame.data.size(); i += 3) {
        // Применяем разные коэффициенты к RGB каналам
        frame.data[i] = static_cast<uint8_t>(std::clamp(frame.data[i] * eq_factor, 0.0f, 255.0f));         // R
        frame.data[i + 1] = static_cast<uint8_t>(std::clamp(frame.data[i + 1] * eq_factor * 0.9f, 0.0f, 255.0f)); // G
        frame.data[i + 2] = static_cast<uint8_t>(std::clamp(frame.data[i + 2] * eq_factor * 1.1f, 0.0f, 255.0f)); // B
    }
}

void VinylVideoPlayer::setNeedlePressure(double pressure) {
    vinyl_params_.needle_pressure = std::clamp(pressure, 0.5, 5.0);
}

void VinylVideoPlayer::setTurntableSpeed(double rpm) {
    vinyl_params_.turntable_speed = rpm;
}

void VinylVideoPlayer::setWowFlutter(double percentage) {
    vinyl_params_.wow_flutter = std::clamp(percentage, 0.0, 2.0);
}

void VinylVideoPlayer::setSurfaceNoise(double level) {
    vinyl_params_.surface_noise = std::clamp(level, 0.0, 0.3);
}

void VinylVideoPlayer::setGrooveDepth(double depth) {
    vinyl_params_.groove_depth = std::clamp(depth, 0.05, 0.2);
}

void VinylVideoPlayer::setMonoMode(bool enabled) {
    vinyl_params_.mono_mode = enabled;
}

void VinylVideoPlayer::setEQCurve(double curve) {
    vinyl_params_.eq_curve = std::clamp(curve, 0.5, 2.0);
}

void VinylVideoPlayer::setVideoFrameCallback(VideoFrameCallback callback) {
    video_callback_ = callback;
}

void VinylVideoPlayer::setAudioFrameCallback(AudioFrameCallback callback) {
    audio_callback_ = callback;
}

void VinylVideoPlayer::setPlaybackEventCallback(PlaybackEventCallback callback) {
    event_callback_ = callback;
}

void VinylVideoPlayer::setPlaybackSpeed(double speed) {
    playback_speed_ = std::clamp(speed, 0.25, 4.0);
}

void VinylVideoPlayer::setLoopEnabled(bool enabled) {
    loop_enabled_ = enabled;
}

VinylVideoPlayer::PlaybackState VinylVideoPlayer::getState() const {
    return state_;
}

double VinylVideoPlayer::getCurrentPosition() const {
    return current_position_;
}

double VinylVideoPlayer::getDuration() const {
    return video_info_.duration;
}

const VideoInfo& VinylVideoPlayer::getVideoInfo() const {
    return video_info_;
}

const VinylVideoPlayer::VinylParameters& VinylVideoPlayer::getVinylParameters() const {
    return vinyl_params_;
}

void VinylVideoPlayer::emitEvent(const std::string& event) {
    if (event_callback_) {
        event_callback_(event);
    }
}

bool VinylVideoPlayer::isVideoFile(const std::string& filepath) const {
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return (ext == "mp4" || ext == "avi" || ext == "mkv" || ext == "mov" || 
            ext == "wmv" || ext == "flv" || ext == "webm");
}

bool VinylVideoPlayer::isVinylRecording(const std::string& filepath) const {
    std::string filename = filepath.substr(filepath.find_last_of('/') + 1);
    std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
    
    return (filename.find("vinyl") != std::string::npos ||
            filename.find("record") != std::string::npos ||
            filename.find("lp") != std::string::npos ||
            filename.find("78") != std::string::npos ||
            filename.find("45") != std::string::npos ||
            filename.find("33") != std::string::npos);
}

// Реализация VinylUtils
namespace VinylUtils {

VinylUtils::VinylType VinylUtils::detectVinylType(const std::string& filepath) {
    std::string filename = filepath.substr(filepath.find_last_of('/') + 1);
    std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
    
    if (filename.find("78") != std::string::npos || filename.find("shellac") != std::string::npos) {
        return VinylUtils::VinylType::SHELLAC_78;
    } else if (filename.find("45") != std::string::npos || filename.find("single") != std::string::npos) {
        return VinylUtils::VinylType::VINYL_45;
    } else if (filename.find("16") != std::string::npos) {
        return VinylUtils::VinylType::VINYL_16;
    } else if (filename.find("33") != std::string::npos || filename.find("lp") != std::string::npos) {
        return VinylUtils::VinylType::VINYL_33;
    }
    
    return VinylUtils::VinylType::UNKNOWN;
}

VinylVideoPlayer::VinylParameters VinylUtils::getRecommendedSettings(VinylUtils::VinylType type) {
    VinylVideoPlayer::VinylParameters params;
    
    switch (type) {
        case VinylUtils::VinylType::SHELLAC_78:
            params.turntable_speed = 78.26;
            params.needle_pressure = 3.0;
            params.wow_flutter = 0.5;
            params.surface_noise = 0.15;
            params.mono_mode = true;
            params.eq_curve = 0.8;
            break;
            
        case VinylUtils::VinylType::VINYL_45:
            params.turntable_speed = 45.0;
            params.needle_pressure = 2.0;
            params.wow_flutter = 0.2;
            params.surface_noise = 0.08;
            params.mono_mode = false;
            params.eq_curve = 1.0;
            break;
            
        case VinylUtils::VinylType::VINYL_33:
            params.turntable_speed = 33.33;
            params.needle_pressure = 1.5;
            params.wow_flutter = 0.1;
            params.surface_noise = 0.05;
            params.mono_mode = false;
            params.eq_curve = 1.0;
            break;
            
        case VinylUtils::VinylType::VINYL_16:
            params.turntable_speed = 16.67;
            params.needle_pressure = 2.5;
            params.wow_flutter = 0.3;
            params.surface_noise = 0.12;
            params.mono_mode = true;
            params.eq_curve = 0.9;
            break;
            
        default:
            // Стандартные настройки
            break;
    }
    
    return params;
}

VideoFrame VinylUtils::enhanceVinylRecording(const VideoFrame& frame, VinylUtils::VinylType type) {
    VideoFrame enhanced = frame;
    
    // Удаляем артефакты в зависимости от типа пластинки
    switch (type) {
        case VinylUtils::VinylType::SHELLAC_78:
            removeCrackle(enhanced);
            removeHiss(enhanced);
            break;
            
        case VinylUtils::VinylType::VINYL_45:
        case VinylUtils::VinylType::VINYL_33:
            removeClick(enhanced);
            removePop(enhanced);
            break;
            
        case VinylUtils::VinylType::VINYL_16:
            removeClick(enhanced);
            removeCrackle(enhanced);
            break;
            
        default:
            break;
    }
    
    return enhanced;
}

void VinylUtils::removeClick(VideoFrame& frame) {
    // Простой алгоритм удаления кликов
    // В реальной реализации здесь был бы более сложный алгоритм
    for (size_t i = 3; i < frame.data.size() - 3; i += 3) {
        // Проверяем резкие изменения яркости
        float current = (frame.data[i] + frame.data[i + 1] + frame.data[i + 2]) / 3.0f;
        float prev = (frame.data[i - 3] + frame.data[i - 2] + frame.data[i - 1]) / 3.0f;
        float next = (frame.data[i + 3] + frame.data[i + 4] + frame.data[i + 5]) / 3.0f;
        
        if (std::abs(current - prev) > 50.0f && std::abs(current - next) > 50.0f) {
            // Интерполируем значение
            float interpolated = (prev + next) / 2.0f;
            frame.data[i] = frame.data[i + 1] = frame.data[i + 2] = static_cast<uint8_t>(interpolated);
        }
    }
}

void VinylUtils::removePop(VideoFrame& frame) {
    // Аналогично кликам, но для более крупных артефактов
    for (size_t i = 9; i < frame.data.size() - 9; i += 3) {
        float current = (frame.data[i] + frame.data[i + 1] + frame.data[i + 2]) / 3.0f;
        float avg_before = 0.0f, avg_after = 0.0f;
        
        // Вычисляем среднее значение до и после
        for (int j = -9; j < 0; j += 3) {
            avg_before += (frame.data[i + j] + frame.data[i + j + 1] + frame.data[i + j + 2]) / 3.0f;
        }
        for (int j = 3; j < 12; j += 3) {
            avg_after += (frame.data[i + j] + frame.data[i + j + 1] + frame.data[i + j + 2]) / 3.0f;
        }
        
        avg_before /= 3.0f;
        avg_after /= 3.0f;
        
        if (std::abs(current - avg_before) > 30.0f && std::abs(current - avg_after) > 30.0f) {
            float interpolated = (avg_before + avg_after) / 2.0f;
            frame.data[i] = frame.data[i + 1] = frame.data[i + 2] = static_cast<uint8_t>(interpolated);
        }
    }
}

void VinylUtils::removeCrackle(VideoFrame& frame) {
    // Удаление треска - более мягкий фильтр
    for (size_t i = 6; i < frame.data.size() - 6; i += 3) {
        float sum = 0.0f;
        int count = 0;
        
        // Вычисляем локальное среднее
        for (int j = -6; j <= 6; j += 3) {
            sum += (frame.data[i + j] + frame.data[i + j + 1] + frame.data[i + j + 2]) / 3.0f;
            count++;
        }
        
        float local_avg = sum / count;
        float current = (frame.data[i] + frame.data[i + 1] + frame.data[i + 2]) / 3.0f;
        
        if (std::abs(current - local_avg) > 20.0f) {
            // Применяем мягкую коррекцию
            float corrected = current * 0.7f + local_avg * 0.3f;
            frame.data[i] = frame.data[i + 1] = frame.data[i + 2] = static_cast<uint8_t>(corrected);
        }
    }
}

void VinylUtils::removeHiss(VideoFrame& frame) {
    // Удаление шипения - низкочастотный фильтр
    for (size_t i = 12; i < frame.data.size() - 12; i += 3) {
        float sum = 0.0f;
        int count = 0;
        
        // Более широкое окно для удаления высокочастотного шума
        for (int j = -12; j <= 12; j += 3) {
            sum += (frame.data[i + j] + frame.data[i + j + 1] + frame.data[i + j + 2]) / 3.0f;
            count++;
        }
        
        float smoothed = sum / count;
        float current = (frame.data[i] + frame.data[i + 1] + frame.data[i + 2]) / 3.0f;
        
        // Применяем сглаживание
        float filtered = current * 0.8f + smoothed * 0.2f;
        frame.data[i] = frame.data[i + 1] = frame.data[i + 2] = static_cast<uint8_t>(filtered);
    }
}

} // namespace VinylUtils
