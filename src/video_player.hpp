#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>

namespace AnantaSound {

// Структура для информации о видео
struct VideoInfo {
    std::string filename;
    int width;
    int height;
    double duration;        // в секундах
    double frame_rate;      // кадров в секунду
    std::string codec;
    int audio_channels;
    int audio_sample_rate;
    bool has_audio;
    bool has_video;
    
    VideoInfo() : width(0), height(0), duration(0.0), frame_rate(0.0),
                  audio_channels(0), audio_sample_rate(0), has_audio(false), has_video(false) {}
};

// Структура для кадра видео
struct VideoFrame {
    std::vector<uint8_t> data;
    int width;
    int height;
    double timestamp;
    int64_t frame_number;
    
    VideoFrame() : width(0), height(0), timestamp(0.0), frame_number(0) {}
};

// Типы коллбэков
using VideoFrameCallback = std::function<void(const VideoFrame&)>;
using AudioFrameCallback = std::function<void(const std::vector<float>&, int channels)>;
using PlaybackEventCallback = std::function<void(const std::string& event)>;

// Основной класс для воспроизведения видео
class VinylVideoPlayer {
private:
    // Состояние воспроизведения
    enum class PlaybackState {
        STOPPED,
        PLAYING,
        PAUSED,
        SEEKING,
        ERROR
    };
    
    std::atomic<PlaybackState> state_;
    std::atomic<bool> should_stop_;
    std::thread playback_thread_;
    mutable std::mutex player_mutex_;
    
    // Информация о видео
    VideoInfo video_info_;
    std::string video_filepath_;
    
    // Коллбэки
    VideoFrameCallback video_callback_;
    AudioFrameCallback audio_callback_;
    PlaybackEventCallback event_callback_;
    
    // Параметры воспроизведения
    std::atomic<double> playback_speed_;
    std::atomic<double> current_position_;
    std::atomic<bool> loop_enabled_;
    
    // Специфичные для винила параметры
public:
    struct VinylParameters {
        double needle_pressure;     // Давление иглы (граммы)
        double turntable_speed;     // Скорость вращения (об/мин)
        double wow_flutter;         // Виброподвижность (процент)
        double surface_noise;       // Уровень шума поверхности
        double groove_depth;        // Глубина канавки
        bool mono_mode;             // Моно режим для старых пластинок
        double eq_curve;            // Кривая эквализации (RIAA, NAB, etc.)
        
        VinylParameters() : needle_pressure(2.0), turntable_speed(33.33), 
                           wow_flutter(0.1), surface_noise(0.05),
                           groove_depth(0.1), mono_mode(false), eq_curve(1.0) {}
    };
    
    VinylParameters vinyl_params_;
    
public:
    VinylVideoPlayer();
    ~VinylVideoPlayer();
    
    // Инициализация и управление
    bool initialize();
    void shutdown();
    
    // Загрузка видео файла
    bool loadVideo(const std::string& filepath);
    bool loadVinylRecording(const std::string& filepath);
    
    // Управление воспроизведением
    bool play();
    bool pause();
    bool stop();
    bool seek(double position_seconds);
    
    // Получение состояния
    PlaybackState getState() const;
    double getCurrentPosition() const;
    double getDuration() const;
    const VideoInfo& getVideoInfo() const;
    
    // Установка коллбэков
    void setVideoFrameCallback(VideoFrameCallback callback);
    void setAudioFrameCallback(AudioFrameCallback callback);
    void setPlaybackEventCallback(PlaybackEventCallback callback);
    
    // Параметры воспроизведения
    void setPlaybackSpeed(double speed);
    void setLoopEnabled(bool enabled);
    
    // Специфичные для винила настройки
    void setNeedlePressure(double pressure);
    void setTurntableSpeed(double rpm);
    void setWowFlutter(double percentage);
    void setSurfaceNoise(double level);
    void setGrooveDepth(double depth);
    void setMonoMode(bool enabled);
    void setEQCurve(double curve);
    
    const VinylParameters& getVinylParameters() const;
    
    // Обработка видео с виниловыми эффектами
    VideoFrame applyVinylEffects(const VideoFrame& original_frame);
    
private:
    // Основной цикл воспроизведения
    void playbackLoop();
    
    // Обработка видео кадров
    void processVideoFrame(const VideoFrame& frame);
    
    // Обработка аудио данных
    void processAudioFrame(const std::vector<float>& audio_data, int channels);
    
    // Виниловые эффекты
    void applySurfaceNoise(VideoFrame& frame);
    void applyWowFlutter(VideoFrame& frame);
    void applyNeedlePressure(VideoFrame& frame);
    void applyGrooveDepth(VideoFrame& frame);
    void applyEQCurve(VideoFrame& frame);
    
    // Вспомогательные методы
    bool isVideoFile(const std::string& filepath) const;
    bool isVinylRecording(const std::string& filepath) const;
    void emitEvent(const std::string& event);
};

// Утилиты для работы с виниловыми записями
namespace VinylUtils {
    
    // Детекция типов виниловых записей
    enum class VinylType {
        SHELLAC_78,     // Шеллак 78 об/мин
        VINYL_33,       // Винил 33⅓ об/мин (LP)
        VINYL_45,       // Винил 45 об/мин (сингл)
        VINYL_16,       // Винил 16⅔ об/мин (редко)
        UNKNOWN
    };
    
    VinylType detectVinylType(const std::string& filepath);
    
    // Рекомендуемые настройки для разных типов пластинок
    VinylVideoPlayer::VinylParameters getRecommendedSettings(VinylType type);
    
    // Восстановление качества записи
    VideoFrame enhanceVinylRecording(const VideoFrame& frame, VinylType type);
    
    // Удаление артефактов
    void removeClick(VideoFrame& frame);
    void removePop(VideoFrame& frame);
    void removeCrackle(VideoFrame& frame);
    void removeHiss(VideoFrame& frame);
}

} // namespace AnantaSound
