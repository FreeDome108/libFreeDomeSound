#include "adaptive_audio_processor.hpp"
#include "audio_analyzer.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>

using namespace AnantaSound;

// Генерация тестового аудио сигнала
std::vector<double> generateTestAudio(double frequency, double duration, size_t sample_rate) {
    std::vector<double> audio;
    size_t num_samples = static_cast<size_t>(duration * sample_rate);
    audio.reserve(num_samples);
    
    for (size_t i = 0; i < num_samples; ++i) {
        double t = static_cast<double>(i) / sample_rate;
        double sample = 0.5 * std::sin(2.0 * M_PI * frequency * t);
        
        // Добавляем немного шума для реалистичности
        sample += 0.1 * (static_cast<double>(rand()) / RAND_MAX - 0.5);
        
        audio.push_back(sample);
    }
    
    return audio;
}

// Генерация сложного аудио сигнала с несколькими частотами
std::vector<double> generateComplexAudio(double duration, size_t sample_rate) {
    std::vector<double> audio;
    size_t num_samples = static_cast<size_t>(duration * sample_rate);
    audio.reserve(num_samples);
    
    std::vector<double> frequencies = {220.0, 440.0, 880.0, 1320.0}; // A3, A4, A5, E6
    
    for (size_t i = 0; i < num_samples; ++i) {
        double t = static_cast<double>(i) / sample_rate;
        double sample = 0.0;
        
        for (size_t j = 0; j < frequencies.size(); ++j) {
            double amplitude = 0.2 / (j + 1); // Убывающая амплитуда
            sample += amplitude * std::sin(2.0 * M_PI * frequencies[j] * t);
        }
        
        // Добавляем модуляцию для имитации дыхания
        double breathing_modulation = 1.0 + 0.3 * std::sin(2.0 * M_PI * 0.2 * t); // 0.2 Hz
        sample *= breathing_modulation;
        
        // Добавляем шум
        sample += 0.05 * (static_cast<double>(rand()) / RAND_MAX - 0.5);
        
        audio.push_back(sample);
    }
    
    return audio;
}

void printEmotionalState(EmotionalState emotion) {
    switch (emotion) {
        case EmotionalState::CALM:
            std::cout << "Спокойствие";
            break;
        case EmotionalState::EXCITED:
            std::cout << "Возбуждение";
            break;
        case EmotionalState::STRESSED:
            std::cout << "Стресс";
            break;
        case EmotionalState::FOCUSED:
            std::cout << "Сосредоточенность";
            break;
        case EmotionalState::RELAXED:
            std::cout << "Расслабление";
            break;
        case EmotionalState::UNKNOWN:
            std::cout << "Неизвестно";
            break;
    }
}

void printAdaptationParameters(const AdaptationParameters& params) {
    std::cout << "  Громкость: " << params.volume_multiplier << std::endl;
    std::cout << "  Темп: " << params.tempo_multiplier << std::endl;
    std::cout << "  Усиление басов: " << params.bass_boost << std::endl;
    std::cout << "  Усиление высоких: " << params.treble_boost << std::endl;
    std::cout << "  Реверберация: " << params.reverb_amount << std::endl;
    std::cout << "  Эхо: " << params.echo_delay << std::endl;
}

int main() {
    std::cout << "=== Демонстрация адаптивной системы звука ===" << std::endl;
    std::cout << "AnantaSound v2.1.0" << std::endl;
    std::cout << std::endl;
    
    // Инициализация процессора
    AdaptiveAudioProcessor processor(1024, 44100);
    
    if (!processor.initialize()) {
        std::cerr << "Ошибка инициализации процессора!" << std::endl;
        return 1;
    }
    
    std::cout << "Процессор инициализирован успешно" << std::endl;
    std::cout << std::endl;
    
    // Тест 1: Простой синусоидальный сигнал
    std::cout << "=== Тест 1: Простой синусоидальный сигнал (440 Гц) ===" << std::endl;
    auto simple_audio = generateTestAudio(440.0, 2.0, 44100);
    
    auto result1 = processor.processAudio(simple_audio);
    std::cout << "Обнаруженная эмоция: ";
    printEmotionalState(result1.detected_emotion);
    std::cout << " (уверенность: " << result1.confidence << ")" << std::endl;
    std::cout << "Примененные параметры адаптации:" << std::endl;
    printAdaptationParameters(result1.applied_parameters);
    std::cout << std::endl;
    
    // Тест 2: Сложный сигнал с несколькими частотами
    std::cout << "=== Тест 2: Сложный аудио сигнал ===" << std::endl;
    auto complex_audio = generateComplexAudio(3.0, 44100);
    
    auto result2 = processor.processAudio(complex_audio);
    std::cout << "Обнаруженная эмоция: ";
    printEmotionalState(result2.detected_emotion);
    std::cout << " (уверенность: " << result2.confidence << ")" << std::endl;
    std::cout << "Примененные параметры адаптации:" << std::endl;
    printAdaptationParameters(result2.applied_parameters);
    std::cout << std::endl;
    
    // Тест 3: Серия быстрых изменений
    std::cout << "=== Тест 3: Серия быстрых изменений ===" << std::endl;
    
    std::vector<double> frequencies = {220.0, 440.0, 880.0, 1760.0};
    std::vector<std::string> emotion_names = {"Низкие частоты", "Средние частоты", "Высокие частоты", "Очень высокие частоты"};
    
    for (size_t i = 0; i < frequencies.size(); ++i) {
        std::cout << "--- " << emotion_names[i] << " (" << frequencies[i] << " Гц) ---" << std::endl;
        
        auto test_audio = generateTestAudio(frequencies[i], 1.0, 44100);
        auto result = processor.processAudio(test_audio);
        
        std::cout << "Обнаруженная эмоция: ";
        printEmotionalState(result.detected_emotion);
        std::cout << " (уверенность: " << result.confidence << ")" << std::endl;
        
        // Показываем только основные параметры
        std::cout << "  Громкость: " << result.applied_parameters.volume_multiplier 
                  << ", Темп: " << result.applied_parameters.tempo_multiplier << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    std::cout << std::endl;
    
    // Тест 4: Демонстрация пользовательских пресетов
    std::cout << "=== Тест 4: Пользовательские пресеты ===" << std::endl;
    
    // Создаем пользовательский пресет для "творческого вдохновения"
    AdaptationParameters creative_preset;
    creative_preset.volume_multiplier = 1.1;
    creative_preset.tempo_multiplier = 1.05;
    creative_preset.bass_boost = 0.3;
    creative_preset.treble_boost = 0.4;
    creative_preset.reverb_amount = 0.2;
    creative_preset.echo_delay = 0.1;
    
    // Применяем пользовательский пресет
    auto custom_audio = generateComplexAudio(2.0, 44100);
    auto custom_result = processor.processAudioWithParameters(custom_audio, creative_preset);
    
    std::cout << "Обработка с пользовательским пресетом 'Творческое вдохновение':" << std::endl;
    printAdaptationParameters(creative_preset);
    std::cout << "Размер обработанного аудио: " << custom_result.size() << " сэмплов" << std::endl;
    std::cout << std::endl;
    
    // Тест 5: Статистика процессора
    std::cout << "=== Тест 5: Статистика процессора ===" << std::endl;
    auto stats = processor.getStatistics();
    
    std::cout << "Общее количество обработанных сэмплов: " << stats.total_processed_samples << std::endl;
    std::cout << "Наиболее частая эмоция: ";
    printEmotionalState(stats.most_common_emotion);
    std::cout << std::endl;
    std::cout << "Средняя уверенность: " << stats.average_confidence << std::endl;
    std::cout << "Средняя корректировка громкости: " << stats.average_volume_adjustment << std::endl;
    std::cout << "Средняя корректировка темпа: " << stats.average_tempo_adjustment << std::endl;
    std::cout << std::endl;
    
    // Тест 6: Демонстрация чувствительности
    std::cout << "=== Тест 6: Настройка чувствительности ===" << std::endl;
    
    std::vector<double> sensitivities = {0.3, 0.7, 1.0};
    auto test_audio = generateTestAudio(440.0, 1.0, 44100);
    
    for (double sensitivity : sensitivities) {
        processor.setAdaptationSensitivity(sensitivity);
        auto result = processor.processAudio(test_audio);
        
        std::cout << "Чувствительность: " << sensitivity 
                  << " -> Эмоция: ";
        printEmotionalState(result.detected_emotion);
        std::cout << " (уверенность: " << result.confidence << ")" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== Демонстрация завершена ===" << std::endl;
    std::cout << "Система успешно проанализировала различные аудио сигналы" << std::endl;
    std::cout << "и адаптировала их в соответствии с обнаруженными эмоциональными состояниями." << std::endl;
    
    return 0;
}

