#include "adaptive_audio_processor.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iostream>

namespace AnantaSound {

AdaptiveAudioProcessor::AdaptiveAudioProcessor(size_t fft_size, size_t sample_rate)
    : analysis_window_size_(fft_size)
    , sample_rate_(sample_rate)
    , adaptation_sensitivity_(0.7)
    , history_size_(10) {
    
    audio_analyzer_ = std::make_unique<AudioAnalyzer>(fft_size, sample_rate);
    initializeEmotionPresets();
}

bool AdaptiveAudioProcessor::initialize() {
    if (!audio_analyzer_) {
        return false;
    }
    
    return audio_analyzer_->initialize();
}

AdaptationResult AdaptiveAudioProcessor::processAudio(const std::vector<double>& input_audio) {
    std::lock_guard<std::mutex> lock(processor_mutex_);
    
    AdaptationResult result;
    
    if (input_audio.empty() || !audio_analyzer_) {
        return result;
    }
    
    // Анализ входящего аудио
    AudioAnalysisResult analysis = audio_analyzer_->analyzeAudio(input_audio);
    
    // Определение эмоционального состояния
    result.detected_emotion = detectEmotionalState(analysis);
    
    // Получение параметров адаптации
    AdaptationParameters base_params = getAdaptationParameters(result.detected_emotion);
    
    // Сглаживание параметров с учетом истории
    result.applied_parameters = smoothAdaptationParameters(base_params);
    
    // Обработка аудио с адаптированными параметрами
    result.processed_audio = processAudioWithParameters(input_audio, result.applied_parameters);
    
    // Расчет уверенности в определении эмоции
    result.confidence = 0.8; // Простая реализация - всегда 80% уверенности
    
    // Обновление истории
    updateHistory(result.detected_emotion, result.applied_parameters);
    
    result.timestamp = std::chrono::high_resolution_clock::now();
    
    return result;
}

std::vector<double> AdaptiveAudioProcessor::processAudioWithParameters(
    const std::vector<double>& input_audio,
    const AdaptationParameters& parameters) {
    
    std::vector<double> processed_audio = input_audio;
    
    // Применение различных эффектов
    processed_audio = applyVolumeAdjustment(processed_audio, parameters.volume_multiplier);
    processed_audio = applyTempoAdjustment(processed_audio, parameters.tempo_multiplier);
    processed_audio = applyBassBoost(processed_audio, parameters.bass_boost);
    processed_audio = applyTrebleBoost(processed_audio, parameters.treble_boost);
    processed_audio = applyReverb(processed_audio, parameters.reverb_amount);
    processed_audio = applyEcho(processed_audio, parameters.echo_delay);
    
    return processed_audio;
}

EmotionalState AdaptiveAudioProcessor::detectEmotionalState(const AudioAnalysisResult& analysis) const {
    // Анализ различных характеристик для определения эмоции
    EmotionalState breathing_emotion = analyzeBreathingPattern(analysis);
    EmotionalState rhythmic_emotion = analyzeRhythmicPattern(analysis);
    EmotionalState spectral_emotion = analyzeSpectralCharacteristics(analysis);
    
    // Простое голосование между методами
    std::map<EmotionalState, int> votes;
    votes[breathing_emotion]++;
    votes[rhythmic_emotion]++;
    votes[spectral_emotion]++;
    
    // Найти наиболее частое состояние
    auto max_vote = std::max_element(votes.begin(), votes.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });
    
    return max_vote->first;
}

AdaptationParameters AdaptiveAudioProcessor::getAdaptationParameters(EmotionalState emotion) const {
    auto it = emotion_presets_.find(emotion);
    if (it != emotion_presets_.end()) {
        return it->second;
    }
    
    // Возвращаем нейтральные параметры для неизвестных эмоций
    return AdaptationParameters();
}

void AdaptiveAudioProcessor::setEmotionPreset(EmotionalState emotion, const AdaptationParameters& parameters) {
    std::lock_guard<std::mutex> lock(processor_mutex_);
    emotion_presets_[emotion] = parameters;
}

void AdaptiveAudioProcessor::setAdaptationSensitivity(double sensitivity) {
    std::lock_guard<std::mutex> lock(processor_mutex_);
    adaptation_sensitivity_ = std::max(0.0, std::min(1.0, sensitivity));
}

AdaptiveAudioProcessor::ProcessorStatistics AdaptiveAudioProcessor::getStatistics() const {
    std::lock_guard<std::mutex> lock(processor_mutex_);
    
    ProcessorStatistics stats;
    stats.total_processed_samples = 0; // TODO: Implement counter
    stats.most_common_emotion = getMostCommonEmotion();
    stats.average_confidence = 0.0; // TODO: Calculate from history
    stats.average_volume_adjustment = 0.0; // TODO: Calculate from history
    stats.average_tempo_adjustment = 0.0; // TODO: Calculate from history
    
    return stats;
}

void AdaptiveAudioProcessor::initializeEmotionPresets() {
    // Пресет для спокойствия
    AdaptationParameters calm_params;
    calm_params.volume_multiplier = 0.8;
    calm_params.tempo_multiplier = 0.9;
    calm_params.bass_boost = 0.2;
    calm_params.treble_boost = 0.1;
    calm_params.reverb_amount = 0.3;
    calm_params.echo_delay = 0.1;
    emotion_presets_[EmotionalState::CALM] = calm_params;
    
    // Пресет для возбуждения
    AdaptationParameters excited_params;
    excited_params.volume_multiplier = 1.2;
    excited_params.tempo_multiplier = 1.1;
    excited_params.bass_boost = 0.4;
    excited_params.treble_boost = 0.3;
    excited_params.reverb_amount = 0.1;
    excited_params.echo_delay = 0.0;
    emotion_presets_[EmotionalState::EXCITED] = excited_params;
    
    // Пресет для стресса
    AdaptationParameters stressed_params;
    stressed_params.volume_multiplier = 0.7;
    stressed_params.tempo_multiplier = 0.8;
    stressed_params.bass_boost = 0.1;
    stressed_params.treble_boost = 0.0;
    stressed_params.reverb_amount = 0.5;
    stressed_params.echo_delay = 0.2;
    emotion_presets_[EmotionalState::STRESSED] = stressed_params;
    
    // Пресет для сосредоточенности
    AdaptationParameters focused_params;
    focused_params.volume_multiplier = 1.0;
    focused_params.tempo_multiplier = 1.0;
    focused_params.bass_boost = 0.0;
    focused_params.treble_boost = 0.2;
    focused_params.reverb_amount = 0.0;
    focused_params.echo_delay = 0.0;
    emotion_presets_[EmotionalState::FOCUSED] = focused_params;
    
    // Пресет для расслабления
    AdaptationParameters relaxed_params;
    relaxed_params.volume_multiplier = 0.9;
    relaxed_params.tempo_multiplier = 0.85;
    relaxed_params.bass_boost = 0.3;
    relaxed_params.treble_boost = 0.0;
    relaxed_params.reverb_amount = 0.4;
    relaxed_params.echo_delay = 0.15;
    emotion_presets_[EmotionalState::RELAXED] = relaxed_params;
}

AdaptationParameters AdaptiveAudioProcessor::smoothAdaptationParameters(const AdaptationParameters& new_params) {
    if (parameter_history_.empty()) {
        return new_params;
    }
    
    // Простое сглаживание с предыдущими параметрами
    AdaptationParameters smoothed;
    double smoothing_factor = 0.3;
    
    smoothed.volume_multiplier = (1.0 - smoothing_factor) * new_params.volume_multiplier + 
                                smoothing_factor * parameter_history_.back().volume_multiplier;
    smoothed.tempo_multiplier = (1.0 - smoothing_factor) * new_params.tempo_multiplier + 
                               smoothing_factor * parameter_history_.back().tempo_multiplier;
    smoothed.bass_boost = (1.0 - smoothing_factor) * new_params.bass_boost + 
                         smoothing_factor * parameter_history_.back().bass_boost;
    smoothed.treble_boost = (1.0 - smoothing_factor) * new_params.treble_boost + 
                           smoothing_factor * parameter_history_.back().treble_boost;
    smoothed.reverb_amount = (1.0 - smoothing_factor) * new_params.reverb_amount + 
                            smoothing_factor * parameter_history_.back().reverb_amount;
    smoothed.echo_delay = (1.0 - smoothing_factor) * new_params.echo_delay + 
                         smoothing_factor * parameter_history_.back().echo_delay;
    
    return smoothed;
}

std::vector<double> AdaptiveAudioProcessor::applyVolumeAdjustment(const std::vector<double>& audio, double multiplier) const {
    std::vector<double> result = audio;
    for (double& sample : result) {
        sample *= multiplier;
        // Ограничение амплитуды
        sample = std::max(-1.0, std::min(1.0, sample));
    }
    return result;
}

std::vector<double> AdaptiveAudioProcessor::applyTempoAdjustment(const std::vector<double>& audio, double multiplier) const {
    if (std::abs(multiplier - 1.0) < 0.01) {
        return audio; // Нет изменений
    }
    
    // Простая реализация изменения темпа через интерполяцию
    std::vector<double> result;
    result.reserve(static_cast<size_t>(audio.size() / multiplier));
    
    for (double i = 0; i < audio.size(); i += multiplier) {
        size_t index = static_cast<size_t>(i);
        if (index < audio.size()) {
            result.push_back(audio[index]);
        }
    }
    
    return result;
}

std::vector<double> AdaptiveAudioProcessor::applyBassBoost(const std::vector<double>& audio, double boost) const {
    if (boost <= 0.0) {
        return audio;
    }
    
    // Простое усиление низких частот через фильтр
    std::vector<double> result = audio;
    double alpha = boost * 0.1; // Коэффициент усиления
    
    for (size_t i = 1; i < result.size(); ++i) {
        result[i] += alpha * (result[i] - result[i-1]);
        result[i] = std::max(-1.0, std::min(1.0, result[i]));
    }
    
    return result;
}

std::vector<double> AdaptiveAudioProcessor::applyTrebleBoost(const std::vector<double>& audio, double boost) const {
    if (boost <= 0.0) {
        return audio;
    }
    
    // Простое усиление высоких частот
    std::vector<double> result = audio;
    double alpha = boost * 0.1;
    
    for (size_t i = 1; i < result.size(); ++i) {
        result[i] += alpha * (result[i] - result[i-1]);
        result[i] = std::max(-1.0, std::min(1.0, result[i]));
    }
    
    return result;
}

std::vector<double> AdaptiveAudioProcessor::applyReverb(const std::vector<double>& audio, double amount) const {
    if (amount <= 0.0) {
        return audio;
    }
    
    // Простая реверберация через задержку и затухание
    std::vector<double> result = audio;
    size_t delay_samples = static_cast<size_t>(sample_rate_ * 0.1 * amount); // 100ms delay
    double decay = 0.3 * amount;
    
    for (size_t i = delay_samples; i < result.size(); ++i) {
        result[i] += decay * audio[i - delay_samples];
        result[i] = std::max(-1.0, std::min(1.0, result[i]));
    }
    
    return result;
}

std::vector<double> AdaptiveAudioProcessor::applyEcho(const std::vector<double>& audio, double delay) const {
    if (delay <= 0.0) {
        return audio;
    }
    
    std::vector<double> result = audio;
    size_t delay_samples = static_cast<size_t>(sample_rate_ * delay);
    double echo_level = 0.3;
    
    for (size_t i = delay_samples; i < result.size(); ++i) {
        result[i] += echo_level * audio[i - delay_samples];
        result[i] = std::max(-1.0, std::min(1.0, result[i]));
    }
    
    return result;
}

EmotionalState AdaptiveAudioProcessor::analyzeBreathingPattern(const AudioAnalysisResult& analysis) const {
    // Анализ паттернов дыхания по частоте и амплитуде
    if (analysis.fundamental_frequency < 0.5) { // Очень низкая частота - глубокое дыхание
        return EmotionalState::RELAXED;
    } else if (analysis.fundamental_frequency > 2.0) { // Высокая частота - учащенное дыхание
        return EmotionalState::EXCITED;
    } else if (analysis.volume_level > 0.7) { // Высокая громкость
        return EmotionalState::STRESSED;
    }
    
    return EmotionalState::CALM;
}

EmotionalState AdaptiveAudioProcessor::analyzeRhythmicPattern(const AudioAnalysisResult& analysis) const {
    // Анализ ритмических паттернов
    if (analysis.tempo > 120) { // Быстрый темп
        return EmotionalState::EXCITED;
    } else if (analysis.tempo < 80) { // Медленный темп
        return EmotionalState::RELAXED;
    } else if (analysis.zero_crossing_rate > 0.3) { // Высокая активность
        return EmotionalState::FOCUSED;
    }
    
    return EmotionalState::CALM;
}

EmotionalState AdaptiveAudioProcessor::analyzeSpectralCharacteristics(const AudioAnalysisResult& analysis) const {
    // Анализ спектральных характеристик
    if (analysis.spectral_centroid > 2000) { // Высокие частоты доминируют
        return EmotionalState::FOCUSED;
    } else if (analysis.spectral_centroid < 500) { // Низкие частоты доминируют
        return EmotionalState::RELAXED;
    } else if (analysis.spectral_rolloff > 4000) { // Широкий спектр
        return EmotionalState::EXCITED;
    }
    
    return EmotionalState::CALM;
}

void AdaptiveAudioProcessor::updateHistory(EmotionalState emotion, const AdaptationParameters& parameters) {
    emotion_history_.push_back(emotion);
    parameter_history_.push_back(parameters);
    
    // Ограничиваем размер истории
    if (emotion_history_.size() > history_size_) {
        emotion_history_.erase(emotion_history_.begin());
    }
    if (parameter_history_.size() > history_size_) {
        parameter_history_.erase(parameter_history_.begin());
    }
}

EmotionalState AdaptiveAudioProcessor::getMostCommonEmotion() const {
    if (emotion_history_.empty()) {
        return EmotionalState::UNKNOWN;
    }
    
    std::map<EmotionalState, int> counts;
    for (const auto& emotion : emotion_history_) {
        counts[emotion]++;
    }
    
    auto max_count = std::max_element(counts.begin(), counts.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });
    
    return max_count->first;
}

} // namespace AnantaSound

