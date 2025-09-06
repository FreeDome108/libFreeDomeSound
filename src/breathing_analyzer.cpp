#include "breathing_analyzer.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iostream>

namespace AnantaSound {

BreathingAnalyzer::BreathingAnalyzer(size_t fft_size, size_t sample_rate)
    : sample_rate_(sample_rate)
    , analysis_window_size_(fft_size)
    , min_breathing_frequency_(0.1)    // 0.1 Гц = 6 вдохов в минуту
    , max_breathing_frequency_(1.0)    // 1.0 Гц = 60 вдохов в минуту
    , history_size_(20) {
    
    audio_analyzer_ = std::make_unique<AudioAnalyzer>(fft_size, sample_rate);
    initializeDefaultThresholds();
}

bool BreathingAnalyzer::initialize() {
    if (!audio_analyzer_) {
        return false;
    }
    
    // Настройка анализатора для дыхательных частот
    audio_analyzer_->setFrequencyRange(min_breathing_frequency_, max_breathing_frequency_);
    
    return audio_analyzer_->initialize();
}

BreathingAnalysisResult BreathingAnalyzer::analyzeBreathing(const std::vector<double>& audio_buffer) {
    std::lock_guard<std::mutex> lock(analyzer_mutex_);
    
    BreathingAnalysisResult result;
    
    if (audio_buffer.empty() || !audio_analyzer_) {
        return result;
    }
    
    // Фильтрация дыхательных частот
    std::vector<double> filtered_audio = filterBreathingFrequencies(audio_buffer);
    
    // Анализ аудио
    AudioAnalysisResult audio_analysis = audio_analyzer_->analyzeAudio(filtered_audio);
    
    // Расчет основных параметров дыхания
    result.breathing_rate = calculateBreathingRate(audio_analysis);
    result.breathing_depth = calculateBreathingDepth(audio_analysis);
    result.breathing_regularity = calculateBreathingRegularity(breathing_rate_history_);
    
    // Классификация состояния и паттерна
    result.current_state = classifyBreathingState(result.breathing_rate, 
                                                 result.breathing_depth, 
                                                 result.breathing_regularity);
    result.pattern = classifyBreathingPattern(breathing_rate_history_);
    
    // Расчет уровней стресса и расслабления
    result.stress_level = calculateStressLevel(result.breathing_rate, 
                                              result.breathing_depth, 
                                              result.breathing_regularity);
    result.relaxation_level = calculateRelaxationLevel(result.breathing_rate, 
                                                      result.breathing_depth, 
                                                      result.breathing_regularity);
    
    // Извлечение дыхательного цикла
    result.breathing_cycle = extractBreathingCycle(filtered_audio);
    
    result.timestamp = std::chrono::high_resolution_clock::now();
    
    // Обновление истории
    updateHistory(result);
    
    return result;
}

std::vector<BreathingAnalysisResult> BreathingAnalyzer::analyzeBreathingWithOverlap(const std::vector<double>& audio_buffer) {
    std::vector<BreathingAnalysisResult> results;
    
    if (audio_buffer.size() < analysis_window_size_) {
        results.push_back(analyzeBreathing(audio_buffer));
        return results;
    }
    
    // Анализ с перекрытием окон
    size_t hop_size = analysis_window_size_ / 4;
    for (size_t start = 0; start + analysis_window_size_ <= audio_buffer.size(); start += hop_size) {
        std::vector<double> window_buffer(audio_buffer.begin() + start, 
                                        audio_buffer.begin() + start + analysis_window_size_);
        results.push_back(analyzeBreathing(window_buffer));
    }
    
    return results;
}

BreathingState BreathingAnalyzer::getCurrentBreathingState() const {
    std::lock_guard<std::mutex> lock(analyzer_mutex_);
    
    if (analysis_history_.empty()) {
        return BreathingState::UNKNOWN;
    }
    
    return analysis_history_.back().current_state;
}

BreathingPattern BreathingAnalyzer::getBreathingPattern() const {
    std::lock_guard<std::mutex> lock(analyzer_mutex_);
    
    if (analysis_history_.empty()) {
        return BreathingPattern::UNKNOWN;
    }
    
    return analysis_history_.back().pattern;
}

double BreathingAnalyzer::getAverageBreathingRate() const {
    std::lock_guard<std::mutex> lock(analyzer_mutex_);
    
    if (breathing_rate_history_.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(breathing_rate_history_.begin(), breathing_rate_history_.end(), 0.0);
    return sum / breathing_rate_history_.size();
}

double BreathingAnalyzer::getStressLevel() const {
    std::lock_guard<std::mutex> lock(analyzer_mutex_);
    
    if (analysis_history_.empty()) {
        return 0.0;
    }
    
    return analysis_history_.back().stress_level;
}

double BreathingAnalyzer::getRelaxationLevel() const {
    std::lock_guard<std::mutex> lock(analyzer_mutex_);
    
    if (analysis_history_.empty()) {
        return 0.0;
    }
    
    return analysis_history_.back().relaxation_level;
}

void BreathingAnalyzer::setBreathingRateThresholds(double min_normal, double max_normal) {
    std::lock_guard<std::mutex> lock(analyzer_mutex_);
    normal_breathing_rate_min_ = min_normal;
    normal_breathing_rate_max_ = max_normal;
}

void BreathingAnalyzer::setDepthThresholds(double deep_threshold, double shallow_threshold) {
    std::lock_guard<std::mutex> lock(analyzer_mutex_);
    deep_breathing_threshold_ = deep_threshold;
    shallow_breathing_threshold_ = shallow_threshold;
}

void BreathingAnalyzer::setRapidBreathingThreshold(double threshold) {
    std::lock_guard<std::mutex> lock(analyzer_mutex_);
    rapid_breathing_threshold_ = threshold;
}

void BreathingAnalyzer::setIrregularityThreshold(double threshold) {
    std::lock_guard<std::mutex> lock(analyzer_mutex_);
    irregularity_threshold_ = threshold;
}

BreathingAnalyzer::BreathingStatistics BreathingAnalyzer::getStatistics() const {
    std::lock_guard<std::mutex> lock(analyzer_mutex_);
    
    BreathingStatistics stats;
    
    if (analysis_history_.empty()) {
        return stats;
    }
    
    // Средняя частота дыхания
    stats.average_breathing_rate = getAverageBreathingRate();
    
    // Средний уровень стресса
    double stress_sum = 0.0;
    double relaxation_sum = 0.0;
    std::map<BreathingState, int> state_counts;
    std::map<BreathingPattern, int> pattern_counts;
    
    for (const auto& result : analysis_history_) {
        stress_sum += result.stress_level;
        relaxation_sum += result.relaxation_level;
        state_counts[result.current_state]++;
        pattern_counts[result.pattern]++;
    }
    
    stats.average_stress_level = stress_sum / analysis_history_.size();
    stats.average_relaxation_level = relaxation_sum / analysis_history_.size();
    
    // Наиболее частое состояние
    auto max_state = std::max_element(state_counts.begin(), state_counts.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });
    stats.most_common_state = max_state->first;
    
    // Наиболее частый паттерн
    auto max_pattern = std::max_element(pattern_counts.begin(), pattern_counts.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });
    stats.most_common_pattern = max_pattern->first;
    
    stats.total_analyses = analysis_history_.size();
    
    return stats;
}

BreathingState BreathingAnalyzer::classifyBreathingState(double rate, double depth, double regularity) const {
    // Классификация по частоте дыхания
    if (rate < normal_breathing_rate_min_) {
        if (depth > deep_breathing_threshold_) {
            return BreathingState::DEEP;
        } else {
            return BreathingState::HOLDING;
        }
    } else if (rate > rapid_breathing_threshold_) {
        return BreathingState::RAPID;
    } else if (rate > normal_breathing_rate_max_) {
        if (depth < shallow_breathing_threshold_) {
            return BreathingState::SHALLOW;
        } else {
            return BreathingState::RAPID;
        }
    }
    
    // Классификация по регулярности
    if (regularity < irregularity_threshold_) {
        return BreathingState::IRREGULAR;
    }
    
    // Классификация по глубине
    if (depth > deep_breathing_threshold_) {
        return BreathingState::DEEP;
    } else if (depth < shallow_breathing_threshold_) {
        return BreathingState::SHALLOW;
    }
    
    return BreathingState::NORMAL;
}

BreathingPattern BreathingAnalyzer::classifyBreathingPattern(const std::vector<double>& rate_history) const {
    if (rate_history.size() < 3) {
        return BreathingPattern::UNKNOWN;
    }
    
    // Анализ вариабельности частоты дыхания
    double mean_rate = std::accumulate(rate_history.begin(), rate_history.end(), 0.0) / rate_history.size();
    double variance = 0.0;
    
    for (double rate : rate_history) {
        variance += (rate - mean_rate) * (rate - mean_rate);
    }
    variance /= rate_history.size();
    double std_dev = std::sqrt(variance);
    double coefficient_of_variation = std_dev / mean_rate;
    
    // Классификация паттернов
    if (coefficient_of_variation < 0.1) {
        return BreathingPattern::REGULAR;
    } else if (coefficient_of_variation > 0.3) {
        return BreathingPattern::IRREGULAR;
    } else if (mean_rate < 8.0) { // Медленное дыхание
        return BreathingPattern::RELAXED;
    } else if (mean_rate > 20.0) { // Быстрое дыхание
        return BreathingPattern::STRESSED;
    } else if (mean_rate > 15.0) { // Умеренно быстрое дыхание
        return BreathingPattern::EXERCISE;
    }
    
    return BreathingPattern::CYCLICAL;
}

double BreathingAnalyzer::calculateBreathingRate(const AudioAnalysisResult& analysis) const {
    // Используем основную частоту как частоту дыхания
    double frequency_hz = analysis.fundamental_frequency;
    
    // Конвертируем в вдохи в минуту
    double breathing_rate = frequency_hz * 60.0;
    
    // Ограничиваем разумными пределами
    return std::max(4.0, std::min(60.0, breathing_rate));
}

double BreathingAnalyzer::calculateBreathingDepth(const AudioAnalysisResult& analysis) const {
    // Используем объем как индикатор глубины дыхания
    return std::min(1.0, analysis.volume_level * 2.0);
}

double BreathingAnalyzer::calculateBreathingRegularity(const std::vector<double>& rate_history) const {
    if (rate_history.size() < 2) {
        return 1.0; // Считаем регулярным, если недостаточно данных
    }
    
    // Расчет стандартного отклонения
    double mean = std::accumulate(rate_history.begin(), rate_history.end(), 0.0) / rate_history.size();
    double variance = 0.0;
    
    for (double rate : rate_history) {
        variance += (rate - mean) * (rate - mean);
    }
    variance /= rate_history.size();
    double std_dev = std::sqrt(variance);
    
    // Нормализуем к диапазону 0.0 - 1.0 (1.0 = очень регулярно)
    double regularity = 1.0 - std::min(1.0, std_dev / mean);
    return std::max(0.0, regularity);
}

double BreathingAnalyzer::calculateStressLevel(double rate, double depth, double regularity) const {
    double stress = 0.0;
    
    // Стресс от высокой частоты дыхания
    if (rate > normal_breathing_rate_max_) {
        stress += (rate - normal_breathing_rate_max_) / (rapid_breathing_threshold_ - normal_breathing_rate_max_);
    }
    
    // Стресс от нерегулярности
    stress += (1.0 - regularity) * 0.5;
    
    // Стресс от поверхностного дыхания
    if (depth < shallow_breathing_threshold_) {
        stress += (shallow_breathing_threshold_ - depth) / shallow_breathing_threshold_;
    }
    
    return std::min(1.0, stress);
}

double BreathingAnalyzer::calculateRelaxationLevel(double rate, double depth, double regularity) const {
    double relaxation = 0.0;
    
    // Расслабление от нормальной частоты дыхания
    if (rate >= normal_breathing_rate_min_ && rate <= normal_breathing_rate_max_) {
        relaxation += 0.4;
    }
    
    // Расслабление от регулярности
    relaxation += regularity * 0.3;
    
    // Расслабление от глубокого дыхания
    if (depth > deep_breathing_threshold_) {
        relaxation += (depth - deep_breathing_threshold_) * 0.3;
    }
    
    return std::min(1.0, relaxation);
}

std::vector<double> BreathingAnalyzer::extractBreathingCycle(const std::vector<double>& audio_buffer) const {
    // Поиск пиков дыхания
    std::vector<size_t> peaks = findBreathingPeaks(audio_buffer);
    
    if (peaks.size() < 2) {
        return audio_buffer; // Возвращаем исходный буфер, если недостаточно пиков
    }
    
    // Извлекаем один полный цикл между двумя пиками
    size_t cycle_start = peaks[0];
    size_t cycle_end = peaks[1];
    
    if (cycle_end > cycle_start && cycle_end < audio_buffer.size()) {
        return std::vector<double>(audio_buffer.begin() + cycle_start, 
                                 audio_buffer.begin() + cycle_end);
    }
    
    return audio_buffer;
}

void BreathingAnalyzer::updateHistory(const BreathingAnalysisResult& result) {
    analysis_history_.push_back(result);
    breathing_rate_history_.push_back(result.breathing_rate);
    amplitude_history_.push_back(result.breathing_depth);
    
    // Ограничиваем размер истории
    if (analysis_history_.size() > history_size_) {
        analysis_history_.pop_front();
    }
    if (breathing_rate_history_.size() > history_size_) {
        breathing_rate_history_.pop_front();
    }
    if (amplitude_history_.size() > history_size_) {
        amplitude_history_.pop_front();
    }
}

std::vector<double> BreathingAnalyzer::filterBreathingFrequencies(const std::vector<double>& audio_buffer) const {
    // Простой полосовой фильтр для дыхательных частот
    std::vector<double> filtered = audio_buffer;
    
    // Простое сглаживание для выделения низкочастотных компонентов
    for (size_t i = 1; i < filtered.size() - 1; ++i) {
        filtered[i] = 0.25 * (audio_buffer[i-1] + 2*audio_buffer[i] + audio_buffer[i+1]);
    }
    
    return filtered;
}

std::vector<size_t> BreathingAnalyzer::findBreathingPeaks(const std::vector<double>& filtered_audio) const {
    std::vector<size_t> peaks;
    
    if (filtered_audio.size() < 3) {
        return peaks;
    }
    
    // Поиск локальных максимумов
    for (size_t i = 1; i < filtered_audio.size() - 1; ++i) {
        if (filtered_audio[i] > filtered_audio[i-1] && 
            filtered_audio[i] > filtered_audio[i+1] &&
            filtered_audio[i] > 0.1) { // Минимальный порог
            peaks.push_back(i);
        }
    }
    
    return peaks;
}

std::vector<double> BreathingAnalyzer::calculateBreathingIntervals(const std::vector<size_t>& peaks) const {
    std::vector<double> intervals;
    
    if (peaks.size() < 2) {
        return intervals;
    }
    
    for (size_t i = 1; i < peaks.size(); ++i) {
        double interval_seconds = static_cast<double>(peaks[i] - peaks[i-1]) / sample_rate_;
        intervals.push_back(interval_seconds);
    }
    
    return intervals;
}

void BreathingAnalyzer::initializeDefaultThresholds() {
    normal_breathing_rate_min_ = 8.0;   // 8 вдохов в минуту
    normal_breathing_rate_max_ = 20.0;  // 20 вдохов в минуту
    deep_breathing_threshold_ = 0.7;    // 70% от максимальной амплитуды
    shallow_breathing_threshold_ = 0.3; // 30% от максимальной амплитуды
    rapid_breathing_threshold_ = 25.0;  // 25 вдохов в минуту
    irregularity_threshold_ = 0.7;      // 70% регулярности
}

} // namespace AnantaSound

