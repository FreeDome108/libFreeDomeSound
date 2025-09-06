#pragma once

#include "audio_analyzer.hpp"
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <map>

namespace AnantaSound {

// Эмоциональные состояния
enum class EmotionalState {
    CALM,           // Спокойствие
    EXCITED,        // Возбуждение
    STRESSED,       // Стресс
    FOCUSED,        // Сосредоточенность
    RELAXED,        // Расслабление
    UNKNOWN         // Неизвестно
};

// Параметры адаптации
struct AdaptationParameters {
    double volume_multiplier;      // Множитель громкости (0.0 - 2.0)
    double tempo_multiplier;       // Множитель темпа (0.5 - 2.0)
    double bass_boost;             // Усиление басов (0.0 - 1.0)
    double treble_boost;           // Усиление высоких частот (0.0 - 1.0)
    double reverb_amount;          // Количество реверберации (0.0 - 1.0)
    double echo_delay;             // Задержка эха (0.0 - 1.0)
    
    AdaptationParameters() : volume_multiplier(1.0), tempo_multiplier(1.0),
                           bass_boost(0.0), treble_boost(0.0),
                           reverb_amount(0.0), echo_delay(0.0) {}
};

// Результат адаптации
struct AdaptationResult {
    std::vector<double> processed_audio;
    EmotionalState detected_emotion;
    AdaptationParameters applied_parameters;
    double confidence;             // Уверенность в определении эмоции (0.0 - 1.0)
    std::chrono::high_resolution_clock::time_point timestamp;
    
    AdaptationResult() : detected_emotion(EmotionalState::UNKNOWN), confidence(0.0),
                        timestamp(std::chrono::high_resolution_clock::now()) {}
};

// Адаптивный аудио процессор
class AdaptiveAudioProcessor {
private:
    std::unique_ptr<AudioAnalyzer> audio_analyzer_;
    std::map<EmotionalState, AdaptationParameters> emotion_presets_;
    mutable std::mutex processor_mutex_;
    
    // Параметры анализа
    size_t analysis_window_size_;
    size_t sample_rate_;
    double adaptation_sensitivity_;    // Чувствительность адаптации (0.0 - 1.0)
    
    // История для сглаживания
    std::vector<EmotionalState> emotion_history_;
    std::vector<AdaptationParameters> parameter_history_;
    size_t history_size_;
    
public:
    AdaptiveAudioProcessor(size_t fft_size = 1024, size_t sample_rate = 44100);
    ~AdaptativeAudioProcessor() = default;
    
    // Инициализация процессора
    bool initialize();
    
    // Обработка аудио с адаптацией
    AdaptationResult processAudio(const std::vector<double>& input_audio);
    
    // Обработка аудио с предустановленными параметрами
    std::vector<double> processAudioWithParameters(const std::vector<double>& input_audio,
                                                  const AdaptationParameters& parameters);
    
    // Определение эмоционального состояния
    EmotionalState detectEmotionalState(const AudioAnalysisResult& analysis) const;
    
    // Получение параметров адаптации для эмоции
    AdaptationParameters getAdaptationParameters(EmotionalState emotion) const;
    
    // Установка пользовательских пресетов
    void setEmotionPreset(EmotionalState emotion, const AdaptationParameters& parameters);
    
    // Настройка чувствительности
    void setAdaptationSensitivity(double sensitivity);
    
    // Получение статистики
    struct ProcessorStatistics {
        size_t total_processed_samples;
        EmotionalState most_common_emotion;
        double average_confidence;
        double average_volume_adjustment;
        double average_tempo_adjustment;
    };
    
    ProcessorStatistics getStatistics() const;
    
private:
    // Инициализация пресетов эмоций
    void initializeEmotionPresets();
    
    // Сглаживание параметров адаптации
    AdaptationParameters smoothAdaptationParameters(const AdaptationParameters& new_params);
    
    // Применение эффектов к аудио
    std::vector<double> applyVolumeAdjustment(const std::vector<double>& audio, double multiplier) const;
    std::vector<double> applyTempoAdjustment(const std::vector<double>& audio, double multiplier) const;
    std::vector<double> applyBassBoost(const std::vector<double>& audio, double boost) const;
    std::vector<double> applyTrebleBoost(const std::vector<double>& audio, double boost) const;
    std::vector<double> applyReverb(const std::vector<double>& audio, double amount) const;
    std::vector<double> applyEcho(const std::vector<double>& audio, double delay) const;
    
    // Анализ паттернов дыхания
    EmotionalState analyzeBreathingPattern(const AudioAnalysisResult& analysis) const;
    
    // Анализ ритмических паттернов
    EmotionalState analyzeRhythmicPattern(const AudioAnalysisResult& analysis) const;
    
    // Анализ спектральных характеристик
    EmotionalState analyzeSpectralCharacteristics(const AudioAnalysisResult& analysis) const;
    
    // Обновление истории
    void updateHistory(EmotionalState emotion, const AdaptationParameters& parameters);
    
    // Получение наиболее частой эмоции из истории
    EmotionalState getMostCommonEmotion() const;
};

} // namespace AnantaSound

