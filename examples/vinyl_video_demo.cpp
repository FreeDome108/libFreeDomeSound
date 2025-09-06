#include "video_player.hpp"
#include "anantasound_core.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <filesystem>
#include <vector>
#include <string>
#include <sndfile.h>
#include <portaudio.h>

using namespace AnantaSound;
namespace fs = std::filesystem;

class VinylVideoDemo {
private:
    VinylVideoPlayer video_player_;
    AnantaSoundCore sound_core_;
    std::vector<std::string> video_files_;
    std::string videos_dir_;
    
    // Статистика воспроизведения
    struct PlaybackStats {
        size_t frames_processed;
        size_t audio_frames_processed;
        double total_playback_time;
        double average_frame_time;
        size_t vinyl_effects_applied;
        
        PlaybackStats() : frames_processed(0), audio_frames_processed(0), 
                         total_playback_time(0.0), average_frame_time(0.0), 
                         vinyl_effects_applied(0) {}
    };
    
    PlaybackStats stats_;
    
public:
    VinylVideoDemo() : sound_core_(5.0, 3.0), videos_dir_("../samples") {
        loadVideoFiles();
    }
    
    void loadVideoFiles() {
        if (fs::exists(videos_dir_)) {
            for (const auto& entry : fs::directory_iterator(videos_dir_)) {
                if (entry.is_regular_file()) {
                    std::string ext = entry.path().extension().string();
                    std::string filename = entry.path().filename().string();
                    
                    // Поддерживаем видео форматы
                    if (ext == ".mp4" || ext == ".avi" || ext == ".mkv" || 
                        ext == ".mov" || ext == ".wmv" || ext == ".flv" || 
                        ext == ".webm" || ext == ".wav" || ext == ".mp3" || 
                        ext == ".flac") {
                        video_files_.push_back(filename);
                    }
                }
            }
        }
        
        if (video_files_.empty()) {
            std::cout << "No video/audio files found in samples directory" << std::endl;
            std::cout << "Supported formats: mp4, avi, mkv, mov, wmv, flv, webm, wav, mp3, flac" << std::endl;
            return;
        }
        
        std::cout << "Found " << video_files_.size() << " video/audio files:" << std::endl;
        for (size_t i = 0; i < video_files_.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << video_files_[i] << std::endl;
        }
    }
    
    bool initialize() {
        if (!sound_core_.initialize()) {
            std::cerr << "Failed to initialize anAntaSound core" << std::endl;
            return false;
        }
        std::cout << "anAntaSound core initialized successfully" << std::endl;
        
        if (!video_player_.initialize()) {
            std::cerr << "Failed to initialize VinylVideoPlayer" << std::endl;
            return false;
        }
        std::cout << "VinylVideoPlayer initialized successfully" << std::endl;
        
        // Настраиваем коллбэки
        setupCallbacks();
        
        return true;
    }
    
    void setupCallbacks() {
        // Коллбэк для видео кадров
        video_player_.setVideoFrameCallback([this](const VideoFrame& frame) {
            this->onVideoFrame(frame);
        });
        
        // Коллбэк для аудио кадров
        video_player_.setAudioFrameCallback([this](const std::vector<float>& audio_data, int channels) {
            this->onAudioFrame(audio_data, channels);
        });
        
        // Коллбэк для событий воспроизведения
        video_player_.setPlaybackEventCallback([this](const std::string& event) {
            this->onPlaybackEvent(event);
        });
    }
    
    void onVideoFrame(const VideoFrame& frame) {
        stats_.frames_processed++;
        
        // Создаем квантовое звуковое поле на основе видео кадра
        SphericalCoord position{2.0, M_PI/3, M_PI/6, frame.timestamp};
        
        // Вычисляем доминирующую частоту на основе яркости кадра
        float brightness = calculateFrameBrightness(frame);
        double frequency = 220.0 + brightness * 400.0; // 220-620 Hz
        
        QuantumSoundField video_field = sound_core_.createQuantumSoundField(
            frequency, position, QuantumSoundState::COHERENT);
        
        sound_core_.processSoundField(video_field);
        
        // Обновляем статистику
        auto now = std::chrono::high_resolution_clock::now();
        static auto last_time = now;
        auto frame_time = std::chrono::duration_cast<std::chrono::microseconds>(now - last_time).count();
        stats_.average_frame_time = (stats_.average_frame_time * (stats_.frames_processed - 1) + frame_time) / stats_.frames_processed;
        last_time = now;
        
        // Выводим информацию о кадре каждые 30 кадров
        if (stats_.frames_processed % 30 == 0) {
            std::cout << "  📹 Frame " << stats_.frames_processed 
                      << ": " << frame.width << "x" << frame.height 
                      << ", brightness: " << std::fixed << std::setprecision(2) << brightness
                      << ", freq: " << frequency << " Hz" << std::endl;
        }
    }
    
    void onAudioFrame(const std::vector<float>& audio_data, int channels) {
        stats_.audio_frames_processed++;
        
        // Анализируем аудио данные и создаем квантовые поля
        if (!audio_data.empty()) {
            // Вычисляем RMS (среднеквадратичное значение)
            float rms = 0.0f;
            for (float sample : audio_data) {
                rms += sample * sample;
            }
            rms = std::sqrt(rms / audio_data.size());
            
            // Создаем звуковое поле на основе RMS
            SphericalCoord audio_position{3.0, M_PI/4, M_PI/3, video_player_.getCurrentPosition()};
            double audio_frequency = 440.0 + rms * 1000.0; // 440-1440 Hz
            
            QuantumSoundField audio_field = sound_core_.createQuantumSoundField(
                audio_frequency, audio_position, QuantumSoundState::EXCITED);
            
            sound_core_.processSoundField(audio_field);
            
            // Выводим информацию об аудио каждые 100 кадров
            if (stats_.audio_frames_processed % 100 == 0) {
                std::cout << "  🎵 Audio frame " << stats_.audio_frames_processed 
                          << ": RMS=" << std::fixed << std::setprecision(3) << rms
                          << ", freq=" << audio_frequency << " Hz" << std::endl;
            }
        }
    }
    
    void onPlaybackEvent(const std::string& event) {
        std::cout << "  📻 Event: " << event << std::endl;
        
        if (event == "playback_started") {
            stats_ = PlaybackStats(); // Сбрасываем статистику
        } else if (event == "playback_ended" || event == "playback_stopped") {
            printPlaybackStats();
        }
    }
    
    float calculateFrameBrightness(const VideoFrame& frame) {
        if (frame.data.empty()) return 0.0f;
        
        float total_brightness = 0.0f;
        int pixel_count = 0;
        
        // Вычисляем среднюю яркость кадра
        for (size_t i = 0; i < frame.data.size(); i += 3) { // RGB
            float r = frame.data[i] / 255.0f;
            float g = frame.data[i + 1] / 255.0f;
            float b = frame.data[i + 2] / 255.0f;
            
            // Используем формулу для вычисления яркости
            float brightness = 0.299f * r + 0.587f * g + 0.114f * b;
            total_brightness += brightness;
            pixel_count++;
        }
        
        return pixel_count > 0 ? total_brightness / pixel_count : 0.0f;
    }
    
    void printPlaybackStats() {
        std::cout << "\n📊 Playback Statistics:" << std::endl;
        std::cout << "  Frames processed: " << stats_.frames_processed << std::endl;
        std::cout << "  Audio frames processed: " << stats_.audio_frames_processed << std::endl;
        std::cout << "  Average frame time: " << std::fixed << std::setprecision(2) 
                  << stats_.average_frame_time << " μs" << std::endl;
        std::cout << "  Total playback time: " << std::fixed << std::setprecision(2) 
                  << video_player_.getCurrentPosition() << "s" << std::endl;
        
        auto sound_stats = sound_core_.getStatistics();
        std::cout << "  Active quantum fields: " << sound_stats.active_fields << std::endl;
        std::cout << "  Coherence ratio: " << std::fixed << std::setprecision(3) 
                  << sound_stats.coherence_ratio << std::endl;
    }
    
    void playVideo(const std::string& filename) {
        std::cout << "\n🎬 Playing Vinyl Video: " << filename << std::endl;
        std::cout << "===========================================" << std::endl;
        
        std::string filepath = videos_dir_ + "/" + filename;
        
        // Определяем, это виниловая запись или обычное видео
        bool is_vinyl = video_player_.isVinylRecording(filepath);
        
        if (is_vinyl) {
            std::cout << "  🎵 Detected vinyl recording - loading with vinyl effects..." << std::endl;
            if (!video_player_.loadVinylRecording(filepath)) {
                std::cerr << "Failed to load vinyl recording: " << filename << std::endl;
                return;
            }
        } else {
            std::cout << "  🎥 Loading as regular video..." << std::endl;
            if (!video_player_.loadVideo(filepath)) {
                std::cerr << "Failed to load video: " << filename << std::endl;
                return;
            }
        }
        
        // Показываем информацию о видео
        const VideoInfo& info = video_player_.getVideoInfo();
        std::cout << "  Resolution: " << info.width << "x" << info.height << std::endl;
        std::cout << "  Duration: " << std::fixed << std::setprecision(2) << info.duration << "s" << std::endl;
        std::cout << "  Frame rate: " << std::fixed << std::setprecision(2) << info.frame_rate << " fps" << std::endl;
        std::cout << "  Has audio: " << (info.has_audio ? "Yes" : "No") << std::endl;
        
        if (is_vinyl) {
            const auto& vinyl_params = video_player_.getVinylParameters();
            std::cout << "  Vinyl settings:" << std::endl;
            std::cout << "    Turntable speed: " << vinyl_params.turntable_speed << " RPM" << std::endl;
            std::cout << "    Needle pressure: " << vinyl_params.needle_pressure << "g" << std::endl;
            std::cout << "    Wow & flutter: " << std::fixed << std::setprecision(1) 
                      << vinyl_params.wow_flutter << "%" << std::endl;
            std::cout << "    Surface noise: " << std::fixed << std::setprecision(1) 
                      << vinyl_params.surface_noise * 100 << "%" << std::endl;
        }
        
        // Настраиваем параметры воспроизведения
        video_player_.setPlaybackSpeed(1.0);
        video_player_.setLoopEnabled(false);
        
        // Запускаем воспроизведение
        auto start_time = std::chrono::high_resolution_clock::now();
        
        if (!video_player_.play()) {
            std::cerr << "Failed to start playback" << std::endl;
            return;
        }
        
        // Ждем окончания воспроизведения
        while (video_player_.getState() == VinylVideoPlayer::PlaybackState::PLAYING) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // Обновляем квантовую акустическую систему
            sound_core_.update(0.1);
            
            // Показываем прогресс каждые 5 секунд
            static double last_progress = 0.0;
            double current_pos = video_player_.getCurrentPosition();
            if (current_pos - last_progress >= 5.0) {
                double progress = (current_pos / info.duration) * 100.0;
                std::cout << "  📈 Progress: " << std::fixed << std::setprecision(1) 
                          << progress << "% (" << current_pos << "s / " << info.duration << "s)" << std::endl;
                last_progress = current_pos;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        
        std::cout << "\n✅ Playback completed in " << total_time << "ms" << std::endl;
    }
    
    void playAllVideos() {
        if (video_files_.empty()) {
            std::cout << "No videos to play" << std::endl;
            return;
        }
        
        std::cout << "\n🎬 Starting Vinyl Video Demo" << std::endl;
        std::cout << "=============================" << std::endl;
        
        for (const auto& video : video_files_) {
            playVideo(video);
            
            // Пауза между видео
            if (&video != &video_files_.back()) {
                std::cout << "\n⏸️  Pausing between videos..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
        
        std::cout << "\n🎉 All videos played successfully!" << std::endl;
    }
    
    void interactiveMode() {
        if (video_files_.empty()) {
            std::cout << "No videos available for interactive mode" << std::endl;
            return;
        }
        
        std::cout << "\n🎮 Interactive Vinyl Video Mode" << std::endl;
        std::cout << "Enter video number (1-" << video_files_.size() << ") or 'q' to quit:" << std::endl;
        
        std::string input;
        while (true) {
            std::cout << "\n> ";
            std::getline(std::cin, input);
            
            if (input == "q" || input == "quit") {
                break;
            }
            
            try {
                size_t index = std::stoul(input) - 1;
                if (index < video_files_.size()) {
                    playVideo(video_files_[index]);
                } else {
                    std::cout << "Invalid video number. Please enter 1-" << video_files_.size() << std::endl;
                }
            } catch (const std::exception&) {
                std::cout << "Invalid input. Please enter a number or 'q' to quit" << std::endl;
            }
        }
    }
    
    void demonstrateVinylEffects() {
        std::cout << "\n🎵 Vinyl Effects Demonstration" << std::endl;
        std::cout << "===============================" << std::endl;
        
        if (video_files_.empty()) {
            std::cout << "No videos available for demonstration" << std::endl;
            return;
        }
        
        // Используем первое доступное видео
        std::string demo_video = video_files_[0];
        std::string filepath = videos_dir_ + "/" + demo_video;
        
        std::cout << "Demo video: " << demo_video << std::endl;
        
        // Демонстрируем разные настройки винила
        std::vector<std::pair<std::string, std::function<void()>>> effects = {
            {"Original (no effects)", [&]() {
                video_player_.setSurfaceNoise(0.0);
                video_player_.setWowFlutter(0.0);
                video_player_.setNeedlePressure(2.0);
            }},
            {"Light surface noise", [&]() {
                video_player_.setSurfaceNoise(0.02);
                video_player_.setWowFlutter(0.05);
                video_player_.setNeedlePressure(1.8);
            }},
            {"Heavy surface noise", [&]() {
                video_player_.setSurfaceNoise(0.1);
                video_player_.setWowFlutter(0.2);
                video_player_.setNeedlePressure(2.5);
            }},
            {"78 RPM Shellac", [&]() {
                video_player_.setTurntableSpeed(78.26);
                video_player_.setSurfaceNoise(0.15);
                video_player_.setWowFlutter(0.5);
                video_player_.setNeedlePressure(3.0);
                video_player_.setMonoMode(true);
            }},
            {"45 RPM Single", [&]() {
                video_player_.setTurntableSpeed(45.0);
                video_player_.setSurfaceNoise(0.08);
                video_player_.setWowFlutter(0.2);
                video_player_.setNeedlePressure(2.0);
                video_player_.setMonoMode(false);
            }}
        };
        
        for (const auto& effect : effects) {
            std::cout << "\n🎧 Testing: " << effect.first << std::endl;
            
            if (!video_player_.loadVideo(filepath)) {
                std::cerr << "Failed to load video for demonstration" << std::endl;
                continue;
            }
            
            // Применяем настройки эффекта
            effect.second();
            
            // Запускаем короткое воспроизведение (первые 10 секунд)
            video_player_.play();
            
            auto start = std::chrono::high_resolution_clock::now();
            while (video_player_.getCurrentPosition() < 10.0 && 
                   video_player_.getState() == VinylVideoPlayer::PlaybackState::PLAYING) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                sound_core_.update(0.1);
            }
            
            video_player_.stop();
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - start).count();
            
            std::cout << "  ✅ Effect applied for " << duration << "ms" << std::endl;
            
            // Пауза между эффектами
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        std::cout << "\n🎉 Vinyl effects demonstration completed!" << std::endl;
    }
    
    void shutdown() {
        video_player_.shutdown();
        sound_core_.shutdown();
        std::cout << "Vinyl Video Demo shutdown complete" << std::endl;
    }
};

int main() {
    std::cout << "🎬 anAntaSound Vinyl Video Player Demo" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "Version: " << getVersion() << std::endl;
    std::cout << "Build Info: " << getBuildInfo() << std::endl;
    
    VinylVideoDemo demo;
    
    if (!demo.initialize()) {
        return 1;
    }
    
    std::cout << "\nChoose demo mode:" << std::endl;
    std::cout << "1. Play all videos sequentially" << std::endl;
    std::cout << "2. Interactive mode (choose videos)" << std::endl;
    std::cout << "3. Vinyl effects demonstration" << std::endl;
    std::cout << "Enter choice (1, 2, or 3): ";
    
    std::string choice;
    std::getline(std::cin, choice);
    
    if (choice == "1") {
        demo.playAllVideos();
    } else if (choice == "2") {
        demo.interactiveMode();
    } else if (choice == "3") {
        demo.demonstrateVinylEffects();
    } else {
        std::cout << "Invalid choice, playing all videos sequentially" << std::endl;
        demo.playAllVideos();
    }
    
    demo.shutdown();
    return 0;
}
