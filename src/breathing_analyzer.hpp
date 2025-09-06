#pragma once

#include "audio_analyzer.hpp"
#include <vector>
#include <deque>
#include <memory>
#include <mutex>
#include <chrono>

namespace AnantaSound {

// Состояния дыхания
enum class BreathingState {
    NORMAL,         // Нормальное дыхание
    DEEP,           // Глубокое дыхание
    SHALLOW,        // Поверхностное дыхание
    RAPID,          // Учащенное дыхание
    IRREGULAR,      // Нерегулярное дыхание
    HOLDING,        // Задержка дыхания
    UNKNOWN         // Неизвестно
};

// Паттерны дыхания
enum class BreathingPattern {
    REGULAR,        // Регулярный ритм
    IRREGULAR,      // Нерегулярный ритм
    CYCLICAL,       // Циклический (медитативный)
    STRESSED,       // Стрессовое дыхание
    EXERCISE,       // Дыхание при физической нагрузке
    RELAXED,        // Расслабленное дыхание
    UNKNOWN         // Неизвестно
};

// Результат анализа дыхания
struct BreathingAnalysisResult {
    BreathingState current_state;
    BreathingPattern pattern;
    double breathing_rate;              // Частота дыхания (вдохов в минуту)
    double breathing_depth;             // Глубина дыхания (0.0 - 1.0)
    double breathing_regularity;        // Регулярность дыхания (0.0 - 1.0)
    double stress_level;                // Уровень стресса (0.0 - 1.0)
    double relaxation_level;            // Уровень расслабления (0.0 - 1.0)
    std::vector<double> breathing_cycle; // Полный цикл дыхания
    std::chrono::high_resolution_clock::time_point timestamp;
    
    BreathingAnalysisResult() : current_state(BreathingState::UNKNOWN),
                               pattern(BreathingPattern::UNKNOWN),
                               breathing_rate(0.0), breathing_depth(0.0),
                               breathing_regularity(0.0), stress_level(0.0),
                               relaxation_level(0.0),
                               timestamp(std::chrono::high_resolution_clock::now()) {}
};

// Анализатор дыхания
class BreathingAnalyzer {
private:
    std::unique_ptr<AudioAnalyzer> audio_analyzer_;
    mutable std::mutex analyzer_mutex_;
    
    // Параметры анализа
    size_t sample_rate_;
    size_t analysis_window_size_;
    double min_breathing_frequency_;    // Минимальная частота дыхания (Гц)
    double max_breathing_frequency_;    // Максимальная частота дыхания (Гц)
    
    // История для анализа паттернов
    std::deque<BreathingAnalysisResult> analysis_history_;
    std::deque<double> breathing_rate_history_;
    std::deque<double> amplitude_history_;
    size_t history_size_;
    
    // Пороги для классификации
    double normal_breathing_rate_min_;  // Минимальная нормальная частота дыхания
    double normal_breathing_rate_max_;  // Максимальная нормальная частота дыхания
    double deep_breathing_threshold_;   // Порог для глубокого дыхания
    double shallow_breathing_threshold_; // Порог для поверхностного дыхания
    double rapid_breathing_threshold_;  // Порог для учащенного дыхания
    double irregularity_threshold_;     // Порог для нерегулярности
    
public:
    BreathingAnalyzer(size_t fft_size = 1024, size_t sample_rate = 44100);
    ~BreathingAnalyzer() = default;
    
    // Инициализация анализатора
    bool initialize();
    
    // Анализ дыхания по аудио сигналу
    BreathingAnalysisResult analyzeBreathing(const std::vector<double>& audio_buffer);
    
    // Анализ дыхания с перекрытием окон
    std::vector<BreathingAnalysisResult> analyzeBreathingWithOverlap(const std::vector<double>& audio_buffer);
    
    // Получение текущего состояния дыхания
    BreathingState getCurrentBreathingState() const;
    
    // Получение паттерна дыхания
    BreathingPattern getBreathingPattern() const;
    
    // Получение средней частоты дыхания
    double getAverageBreathingRate() const;
    
    // Получение уровня стресса
    double getStressLevel() const;
    
    // Получение уровня расслабления
    double getRelaxationLevel() const;
    
    // Настройка порогов
    void setBreathingRateThresholds(double min_normal, double max_normal);
    void setDepthThresholds(double deep_threshold, double shallow_threshold);
    void setRapidBreathingThreshold(double threshold);
    void setIrregularityThreshold(double threshold);
    
    // Получение статистики
    struct BreathingStatistics {
        double average_breathing_rate;
        double average_stress_level;
        double average_relaxation_level;
        BreathingState most_common_state;
        BreathingPattern most_common_pattern;
        size_t total_analyses;
    };
    
    BreathingStatistics getStatistics() const;
    
private:
    // Основные методы анализа
    BreathingState classifyBreathingState(double rate, double depth, double regularity) const;
    BreathingPattern classifyBreathingPattern(const std::vector<double>& rate_history) const;
    
    // Анализ частоты дыхания
    double calculateBreathingRate(const AudioAnalysisResult& analysis) const;
    
    // Анализ глубины дыхания
    double calculateBreathingDepth(const AudioAnalysisResult& analysis) const;
    
    // Анализ регулярности дыхания
    double calculateBreathingRegularity(const std::vector<double>& rate_history) const;
    
    // Анализ уровня стресса
    double calculateStressLevel(double rate, double depth, double regularity) const;
    
    // Анализ уровня расслабления
    double calculateRelaxationLevel(double rate, double depth, double regularity) const;
    
    // Выделение дыхательного цикла
    std::vector<double> extractBreathingCycle(const std::vector<double>& audio_buffer) const;
    
    // Обновление истории
    void updateHistory(const BreathingAnalysisResult& result);
    
    // Фильтрация дыхательных частот
    std::vector<double> filterBreathingFrequencies(const std::vector<double>& audio_buffer) const;
    
    // Поиск пиков дыхания
    std::vector<size_t> findBreathingPeaks(const std::vector<double>& filtered_audio) const;
    
    // Расчет интервалов между вдохами
    std::vector<double> calculateBreathingIntervals(const std::vector<size_t>& peaks) const;
    
    // Инициализация порогов по умолчанию
    void initializeDefaultThresholds();
};

} // namespace AnantaSound

