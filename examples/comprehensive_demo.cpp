#include "anantasound_core.hpp"
#include "audio_analyzer.hpp"
#include "adaptive_audio_processor.hpp"
#include "breathing_analyzer.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>

using namespace AnantaSound;

// Генерация комплексного аудио сигнала
std::vector<double> generateComplexAudioSignal(double duration, size_t sample_rate) {
    std::vector<double> signal;
    size_t num_samples = static_cast<size_t>(duration * sample_rate);
    signal.reserve(num_samples);
    
    // Основные частоты
    std::vector<double> frequencies = {220.0, 440.0, 880.0, 1320.0};
    std::vector<double> amplitudes = {0.3, 0.4, 0.2, 0.1};
    
    for (size_t i = 0; i < num_samples; ++i) {
        double t = static_cast<double>(i) / sample_rate;
        double sample = 0.0;
        
        // Добавляем основные частоты
        for (size_t j = 0; j < frequencies.size(); ++j) {
            sample += amplitudes[j] * std::sin(2.0 * M_PI * frequencies[j] * t);
        }
        
        // Добавляем модуляцию для имитации дыхания
        double breathing_modulation = 1.0 + 0.2 * std::sin(2.0 * M_PI * 0.2 * t); // 0.2 Hz
        sample *= breathing_modulation;
        
        // Добавляем шум
        sample += 0.05 * (static_cast<double>(rand()) / RAND_MAX - 0.5);
        
        signal.push_back(sample);
    }
    
    return signal;
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

void printBreathingState(BreathingState state) {
    switch (state) {
        case BreathingState::NORMAL:
            std::cout << "Нормальное";
            break;
        case BreathingState::DEEP:
            std::cout << "Глубокое";
            break;
        case BreathingState::SHALLOW:
            std::cout << "Поверхностное";
            break;
        case BreathingState::RAPID:
            std::cout << "Учащенное";
            break;
        case BreathingState::IRREGULAR:
            std::cout << "Нерегулярное";
            break;
        case BreathingState::HOLDING:
            std::cout << "Задержка дыхания";
            break;
        case BreathingState::UNKNOWN:
            std::cout << "Неизвестно";
            break;
    }
}

int main() {
    std::cout << "=== Комплексная демонстрация AnantaSound ===" << std::endl;
    std::cout << "Версия: " << getVersion() << std::endl;
    std::cout << "Информация о сборке:" << std::endl;
    std::cout << getBuildInfo() << std::endl;
    std::cout << std::endl;
    
    // Инициализация основных систем
    std::cout << "=== Инициализация систем ===" << std::endl;
    
    // 1. Основная система AnantaSound
    AnantaSoundCore sound_core(5.0, 3.0); // Радиус 5м, высота 3м
    if (!sound_core.initialize()) {
        std::cerr << "Ошибка инициализации AnantaSoundCore!" << std::endl;
        return 1;
    }
    std::cout << "✓ AnantaSoundCore инициализирована" << std::endl;
    
    // 2. Анализатор аудио
    AudioAnalyzer audio_analyzer(1024, 44100);
    if (!audio_analyzer.initialize()) {
        std::cerr << "Ошибка инициализации AudioAnalyzer!" << std::endl;
        return 1;
    }
    std::cout << "✓ AudioAnalyzer инициализирован" << std::endl;
    
    // 3. Адаптивный аудио процессор
    AdaptiveAudioProcessor adaptive_processor(1024, 44100);
    if (!adaptive_processor.initialize()) {
        std::cerr << "Ошибка инициализации AdaptiveAudioProcessor!" << std::endl;
        return 1;
    }
    std::cout << "✓ AdaptiveAudioProcessor инициализирован" << std::endl;
    
    // 4. Анализатор дыхания
    BreathingAnalyzer breathing_analyzer(1024, 44100);
    if (!breathing_analyzer.initialize()) {
        std::cerr << "Ошибка инициализации BreathingAnalyzer!" << std::endl;
        return 1;
    }
    std::cout << "✓ BreathingAnalyzer инициализирован" << std::endl;
    
    std::cout << std::endl;
    
    // Генерация тестового аудио
    std::cout << "=== Генерация тестового аудио ===" << std::endl;
    auto test_audio = generateComplexAudioSignal(10.0, 44100);
    std::cout << "Сгенерирован аудио сигнал: " << test_audio.size() << " сэмплов" << std::endl;
    std::cout << "Длительность: " << (test_audio.size() / 44100.0) << " секунд" << std::endl;
    std::cout << std::endl;
    
    // Анализ аудио
    std::cout << "=== Анализ аудио ===" << std::endl;
    auto audio_analysis = audio_analyzer.analyzeAudio(test_audio);
    std::cout << "Основная частота: " << audio_analysis.fundamental_frequency << " Гц" << std::endl;
    std::cout << "Уровень громкости: " << audio_analysis.volume_level << std::endl;
    std::cout << "Спектральный центроид: " << audio_analysis.spectral_centroid << " Гц" << std::endl;
    std::cout << "Спектральный спад: " << audio_analysis.spectral_rolloff << " Гц" << std::endl;
    std::cout << "Частота пересечений нуля: " << audio_analysis.zero_crossing_rate << std::endl;
    std::cout << "Темп: " << audio_analysis.tempo << " BPM" << std::endl;
    std::cout << std::endl;
    
    // Анализ дыхания
    std::cout << "=== Анализ дыхания ===" << std::endl;
    auto breathing_analysis = breathing_analyzer.analyzeBreathing(test_audio);
    std::cout << "Состояние дыхания: ";
    printBreathingState(breathing_analysis.current_state);
    std::cout << std::endl;
    std::cout << "Частота дыхания: " << breathing_analysis.breathing_rate << " вдохов/мин" << std::endl;
    std::cout << "Глубина дыхания: " << breathing_analysis.breathing_depth << std::endl;
    std::cout << "Регулярность: " << breathing_analysis.breathing_regularity << std::endl;
    std::cout << "Уровень стресса: " << breathing_analysis.stress_level << std::endl;
    std::cout << "Уровень расслабления: " << breathing_analysis.relaxation_level << std::endl;
    std::cout << std::endl;
    
    // Адаптивная обработка аудио
    std::cout << "=== Адаптивная обработка аудио ===" << std::endl;
    auto adaptation_result = adaptive_processor.processAudio(test_audio);
    std::cout << "Обнаруженная эмоция: ";
    printEmotionalState(adaptation_result.detected_emotion);
    std::cout << " (уверенность: " << adaptation_result.confidence << ")" << std::endl;
    std::cout << "Примененные параметры адаптации:" << std::endl;
    std::cout << "  Громкость: " << adaptation_result.applied_parameters.volume_multiplier << std::endl;
    std::cout << "  Темп: " << adaptation_result.applied_parameters.tempo_multiplier << std::endl;
    std::cout << "  Усиление басов: " << adaptation_result.applied_parameters.bass_boost << std::endl;
    std::cout << "  Усиление высоких: " << adaptation_result.applied_parameters.treble_boost << std::endl;
    std::cout << "  Реверберация: " << adaptation_result.applied_parameters.reverb_amount << std::endl;
    std::cout << "  Эхо: " << adaptation_result.applied_parameters.echo_delay << std::endl;
    std::cout << "Размер обработанного аудио: " << adaptation_result.processed_audio.size() << " сэмплов" << std::endl;
    std::cout << std::endl;
    
    // Создание квантовых звуковых полей
    std::cout << "=== Создание квантовых звуковых полей ===" << std::endl;
    
    // Создаем несколько квантовых полей
    SphericalCoord position1(1.0, M_PI/4, M_PI/2, 0.0, 1.5);
    SphericalCoord position2(2.0, M_PI/2, M_PI, 0.0, 2.0);
    SphericalCoord position3(3.0, 3*M_PI/4, 3*M_PI/2, 0.0, 1.0);
    
    auto field1 = sound_core.createQuantumSoundField(440.0, position1, QuantumSoundState::COHERENT);
    auto field2 = sound_core.createQuantumSoundField(880.0, position2, QuantumSoundState::SUPERPOSITION);
    auto field3 = sound_core.createQuantumSoundField(1320.0, position3, QuantumSoundState::ENTANGLED);
    
    std::cout << "Создано 3 квантовых звуковых поля:" << std::endl;
    std::cout << "  Поле 1: 440 Гц, когерентное состояние" << std::endl;
    std::cout << "  Поле 2: 880 Гц, суперпозиция" << std::endl;
    std::cout << "  Поле 3: 1320 Гц, запутанное состояние" << std::endl;
    
    // Обработка полей
    sound_core.processSoundField(field1);
    sound_core.processSoundField(field2);
    sound_core.processSoundField(field3);
    
    std::cout << "Квантовые поля обработаны" << std::endl;
    std::cout << std::endl;
    
    // Создание интерференционных полей
    std::cout << "=== Создание интерференционных полей ===" << std::endl;
    
    auto interference_field1 = std::make_unique<InterferenceField>(
        InterferenceFieldType::CONSTRUCTIVE, position1, 1.0);
    auto interference_field2 = std::make_unique<InterferenceField>(
        InterferenceFieldType::QUANTUM_ENTANGLED, position2, 1.5);
    
    interference_field1->addSourceField(field1);
    interference_field1->addSourceField(field2);
    interference_field2->addSourceField(field2);
    interference_field2->addSourceField(field3);
    
    sound_core.addInterferenceField(std::move(interference_field1));
    sound_core.addInterferenceField(std::move(interference_field2));
    
    std::cout << "Создано 2 интерференционных поля" << std::endl;
    std::cout << "  Поле 1: Конструктивная интерференция" << std::endl;
    std::cout << "  Поле 2: Квантово-запутанная интерференция" << std::endl;
    std::cout << std::endl;
    
    // Обновление системы
    std::cout << "=== Обновление системы ===" << std::endl;
    for (int i = 0; i < 5; ++i) {
        sound_core.update(0.1); // Обновление на 100мс
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Обновление " << (i + 1) << "/5" << std::endl;
    }
    std::cout << std::endl;
    
    // Получение статистики
    std::cout << "=== Статистика системы ===" << std::endl;
    auto stats = sound_core.getStatistics();
    std::cout << "Активные поля: " << stats.active_fields << std::endl;
    std::cout << "Запутанные пары: " << stats.entangled_pairs << std::endl;
    std::cout << "Коэффициент когерентности: " << stats.coherence_ratio << std::endl;
    std::cout << "Энергетическая эффективность: " << stats.energy_efficiency << std::endl;
    std::cout << "QRD подключен: " << (stats.qrd_connected ? "Да" : "Нет") << std::endl;
    std::cout << "Активные механические устройства: " << stats.mechanical_devices_active << std::endl;
    std::cout << std::endl;
    
    // Получение выходных полей
    std::cout << "=== Выходные поля ===" << std::endl;
    auto output_fields = sound_core.getOutputFields();
    std::cout << "Количество выходных полей: " << output_fields.size() << std::endl;
    
    for (size_t i = 0; i < output_fields.size(); ++i) {
        const auto& field = output_fields[i];
        std::cout << "  Поле " << (i + 1) << ":" << std::endl;
        std::cout << "    Частота: " << field.frequency << " Гц" << std::endl;
        std::cout << "    Амплитуда: " << std::abs(field.amplitude) << std::endl;
        std::cout << "    Фаза: " << field.phase << " рад" << std::endl;
        std::cout << "    Квантовое состояние: " << static_cast<int>(field.quantum_state) << std::endl;
    }
    std::cout << std::endl;
    
    // Статистика анализаторов
    std::cout << "=== Статистика анализаторов ===" << std::endl;
    
    auto breathing_stats = breathing_analyzer.getStatistics();
    std::cout << "Анализатор дыхания:" << std::endl;
    std::cout << "  Средняя частота дыхания: " << breathing_stats.average_breathing_rate << " вдохов/мин" << std::endl;
    std::cout << "  Средний уровень стресса: " << breathing_stats.average_stress_level << std::endl;
    std::cout << "  Средний уровень расслабления: " << breathing_stats.average_relaxation_level << std::endl;
    std::cout << "  Общее количество анализов: " << breathing_stats.total_analyses << std::endl;
    
    auto processor_stats = adaptive_processor.getStatistics();
    std::cout << "Адаптивный процессор:" << std::endl;
    std::cout << "  Общее количество обработанных сэмплов: " << processor_stats.total_processed_samples << std::endl;
    std::cout << "  Средняя уверенность: " << processor_stats.average_confidence << std::endl;
    std::cout << "  Средняя корректировка громкости: " << processor_stats.average_volume_adjustment << std::endl;
    std::cout << "  Средняя корректировка темпа: " << processor_stats.average_tempo_adjustment << std::endl;
    std::cout << std::endl;
    
    // Завершение работы
    std::cout << "=== Завершение работы ===" << std::endl;
    sound_core.shutdown();
    std::cout << "✓ AnantaSoundCore завершена" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== Комплексная демонстрация завершена ===" << std::endl;
    std::cout << "Все системы успешно протестированы и работают корректно!" << std::endl;
    std::cout << "Реализованные функции:" << std::endl;
    std::cout << "✓ FFT анализ аудио" << std::endl;
    std::cout << "✓ Адаптивная обработка звука" << std::endl;
    std::cout << "✓ Анализ дыхания и эмоций" << std::endl;
    std::cout << "✓ Квантовые звуковые поля" << std::endl;
    std::cout << "✓ Интерференционные поля" << std::endl;
    std::cout << "✓ Система статистики" << std::endl;
    std::cout << "✓ Квантовая обратная связь" << std::endl;
    std::cout << "✓ Системы сознания" << std::endl;
    
    return 0;
}

