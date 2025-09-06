#include "breathing_analyzer.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>

using namespace AnantaSound;

// Генерация симулированного дыхательного сигнала
std::vector<double> generateBreathingSignal(double breathing_rate, double depth, double duration, size_t sample_rate) {
    std::vector<double> signal;
    size_t num_samples = static_cast<size_t>(duration * sample_rate);
    signal.reserve(num_samples);
    
    double breathing_frequency = breathing_rate / 60.0; // Конвертируем в Гц
    
    for (size_t i = 0; i < num_samples; ++i) {
        double t = static_cast<double>(i) / sample_rate;
        
        // Основной дыхательный сигнал (синусоида)
        double breathing_signal = depth * std::sin(2.0 * M_PI * breathing_frequency * t);
        
        // Добавляем вариации для реалистичности
        double variation = 0.1 * std::sin(2.0 * M_PI * 0.1 * t); // Медленные вариации
        breathing_signal += variation;
        
        // Добавляем шум
        breathing_signal += 0.05 * (static_cast<double>(rand()) / RAND_MAX - 0.5);
        
        signal.push_back(breathing_signal);
    }
    
    return signal;
}

// Генерация стрессового дыхания
std::vector<double> generateStressedBreathing(double duration, size_t sample_rate) {
    std::vector<double> signal;
    size_t num_samples = static_cast<size_t>(duration * sample_rate);
    signal.reserve(num_samples);
    
    for (size_t i = 0; i < num_samples; ++i) {
        double t = static_cast<double>(i) / sample_rate;
        
        // Быстрое, нерегулярное дыхание
        double base_frequency = 0.4; // 24 вдоха в минуту
        double irregularity = 0.1 * std::sin(2.0 * M_PI * 0.3 * t); // Нерегулярность
        double breathing_signal = 0.6 * std::sin(2.0 * M_PI * (base_frequency + irregularity) * t);
        
        // Добавляем шум
        breathing_signal += 0.1 * (static_cast<double>(rand()) / RAND_MAX - 0.5);
        
        signal.push_back(breathing_signal);
    }
    
    return signal;
}

// Генерация расслабленного дыхания
std::vector<double> generateRelaxedBreathing(double duration, size_t sample_rate) {
    std::vector<double> signal;
    size_t num_samples = static_cast<size_t>(duration * sample_rate);
    signal.reserve(num_samples);
    
    for (size_t i = 0; i < num_samples; ++i) {
        double t = static_cast<double>(i) / sample_rate;
        
        // Медленное, глубокое дыхание
        double breathing_frequency = 0.15; // 9 вдохов в минуту
        double breathing_signal = 0.8 * std::sin(2.0 * M_PI * breathing_frequency * t);
        
        // Добавляем небольшой шум
        breathing_signal += 0.02 * (static_cast<double>(rand()) / RAND_MAX - 0.5);
        
        signal.push_back(breathing_signal);
    }
    
    return signal;
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

void printBreathingPattern(BreathingPattern pattern) {
    switch (pattern) {
        case BreathingPattern::REGULAR:
            std::cout << "Регулярный";
            break;
        case BreathingPattern::IRREGULAR:
            std::cout << "Нерегулярный";
            break;
        case BreathingPattern::CYCLICAL:
            std::cout << "Циклический";
            break;
        case BreathingPattern::STRESSED:
            std::cout << "Стрессовый";
            break;
        case BreathingPattern::EXERCISE:
            std::cout << "При нагрузке";
            break;
        case BreathingPattern::RELAXED:
            std::cout << "Расслабленный";
            break;
        case BreathingPattern::UNKNOWN:
            std::cout << "Неизвестно";
            break;
    }
}

int main() {
    std::cout << "=== Демонстрация анализа дыхания ===" << std::endl;
    std::cout << "AnantaSound Breathing Analyzer v2.1.0" << std::endl;
    std::cout << std::endl;
    
    // Инициализация анализатора
    BreathingAnalyzer analyzer(1024, 44100);
    
    if (!analyzer.initialize()) {
        std::cerr << "Ошибка инициализации анализатора дыхания!" << std::endl;
        return 1;
    }
    
    std::cout << "Анализатор дыхания инициализирован успешно" << std::endl;
    std::cout << std::endl;
    
    // Тест 1: Нормальное дыхание
    std::cout << "=== Тест 1: Нормальное дыхание (15 вдохов/мин) ===" << std::endl;
    auto normal_breathing = generateBreathingSignal(15.0, 0.6, 10.0, 44100);
    
    auto result1 = analyzer.analyzeBreathing(normal_breathing);
    std::cout << "Состояние дыхания: ";
    printBreathingState(result1.current_state);
    std::cout << std::endl;
    std::cout << "Паттерн дыхания: ";
    printBreathingPattern(result1.pattern);
    std::cout << std::endl;
    std::cout << "Частота дыхания: " << result1.breathing_rate << " вдохов/мин" << std::endl;
    std::cout << "Глубина дыхания: " << result1.breathing_depth << std::endl;
    std::cout << "Регулярность: " << result1.breathing_regularity << std::endl;
    std::cout << "Уровень стресса: " << result1.stress_level << std::endl;
    std::cout << "Уровень расслабления: " << result1.relaxation_level << std::endl;
    std::cout << std::endl;
    
    // Тест 2: Стрессовое дыхание
    std::cout << "=== Тест 2: Стрессовое дыхание ===" << std::endl;
    auto stressed_breathing = generateStressedBreathing(10.0, 44100);
    
    auto result2 = analyzer.analyzeBreathing(stressed_breathing);
    std::cout << "Состояние дыхания: ";
    printBreathingState(result2.current_state);
    std::cout << std::endl;
    std::cout << "Паттерн дыхания: ";
    printBreathingPattern(result2.pattern);
    std::cout << std::endl;
    std::cout << "Частота дыхания: " << result2.breathing_rate << " вдохов/мин" << std::endl;
    std::cout << "Глубина дыхания: " << result2.breathing_depth << std::endl;
    std::cout << "Регулярность: " << result2.breathing_regularity << std::endl;
    std::cout << "Уровень стресса: " << result2.stress_level << std::endl;
    std::cout << "Уровень расслабления: " << result2.relaxation_level << std::endl;
    std::cout << std::endl;
    
    // Тест 3: Расслабленное дыхание
    std::cout << "=== Тест 3: Расслабленное дыхание ===" << std::endl;
    auto relaxed_breathing = generateRelaxedBreathing(10.0, 44100);
    
    auto result3 = analyzer.analyzeBreathing(relaxed_breathing);
    std::cout << "Состояние дыхания: ";
    printBreathingState(result3.current_state);
    std::cout << std::endl;
    std::cout << "Паттерн дыхания: ";
    printBreathingPattern(result3.pattern);
    std::cout << std::endl;
    std::cout << "Частота дыхания: " << result3.breathing_rate << " вдохов/мин" << std::endl;
    std::cout << "Глубина дыхания: " << result3.breathing_depth << std::endl;
    std::cout << "Регулярность: " << result3.breathing_regularity << std::endl;
    std::cout << "Уровень стресса: " << result3.stress_level << std::endl;
    std::cout << "Уровень расслабления: " << result3.relaxation_level << std::endl;
    std::cout << std::endl;
    
    // Тест 4: Серия различных частот дыхания
    std::cout << "=== Тест 4: Серия различных частот дыхания ===" << std::endl;
    
    std::vector<double> breathing_rates = {6.0, 12.0, 18.0, 24.0, 30.0};
    std::vector<std::string> descriptions = {"Очень медленное", "Медленное", "Нормальное", "Быстрое", "Очень быстрое"};
    
    for (size_t i = 0; i < breathing_rates.size(); ++i) {
        std::cout << "--- " << descriptions[i] << " (" << breathing_rates[i] << " вдохов/мин) ---" << std::endl;
        
        auto test_breathing = generateBreathingSignal(breathing_rates[i], 0.6, 5.0, 44100);
        auto result = analyzer.analyzeBreathing(test_breathing);
        
        std::cout << "Обнаруженная частота: " << result.breathing_rate << " вдохов/мин" << std::endl;
        std::cout << "Состояние: ";
        printBreathingState(result.current_state);
        std::cout << std::endl;
        std::cout << "Стресс: " << result.stress_level << ", Расслабление: " << result.relaxation_level << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    std::cout << std::endl;
    
    // Тест 5: Анализ с перекрытием окон
    std::cout << "=== Тест 5: Анализ с перекрытием окон ===" << std::endl;
    auto long_breathing = generateBreathingSignal(16.0, 0.7, 15.0, 44100);
    
    auto results = analyzer.analyzeBreathingWithOverlap(long_breathing);
    std::cout << "Получено " << results.size() << " результатов анализа" << std::endl;
    
    // Статистика по результатам
    double avg_rate = 0.0, avg_stress = 0.0, avg_relaxation = 0.0;
    for (const auto& result : results) {
        avg_rate += result.breathing_rate;
        avg_stress += result.stress_level;
        avg_relaxation += result.relaxation_level;
    }
    
    avg_rate /= results.size();
    avg_stress /= results.size();
    avg_relaxation /= results.size();
    
    std::cout << "Средняя частота дыхания: " << avg_rate << " вдохов/мин" << std::endl;
    std::cout << "Средний уровень стресса: " << avg_stress << std::endl;
    std::cout << "Средний уровень расслабления: " << avg_relaxation << std::endl;
    std::cout << std::endl;
    
    // Тест 6: Статистика анализатора
    std::cout << "=== Тест 6: Статистика анализатора ===" << std::endl;
    auto stats = analyzer.getStatistics();
    
    std::cout << "Средняя частота дыхания: " << stats.average_breathing_rate << " вдохов/мин" << std::endl;
    std::cout << "Средний уровень стресса: " << stats.average_stress_level << std::endl;
    std::cout << "Средний уровень расслабления: " << stats.average_relaxation_level << std::endl;
    std::cout << "Наиболее частое состояние: ";
    printBreathingState(stats.most_common_state);
    std::cout << std::endl;
    std::cout << "Наиболее частый паттерн: ";
    printBreathingPattern(stats.most_common_pattern);
    std::cout << std::endl;
    std::cout << "Общее количество анализов: " << stats.total_analyses << std::endl;
    std::cout << std::endl;
    
    // Тест 7: Настройка порогов
    std::cout << "=== Тест 7: Настройка порогов ===" << std::endl;
    
    // Устанавливаем более строгие пороги
    analyzer.setBreathingRateThresholds(10.0, 18.0); // Уже нормальный диапазон
    analyzer.setRapidBreathingThreshold(22.0); // Более высокий порог для быстрого дыхания
    
    auto test_breathing = generateBreathingSignal(20.0, 0.6, 5.0, 44100);
    auto result = analyzer.analyzeBreathing(test_breathing);
    
    std::cout << "После настройки порогов:" << std::endl;
    std::cout << "Частота дыхания: " << result.breathing_rate << " вдохов/мин" << std::endl;
    std::cout << "Состояние: ";
    printBreathingState(result.current_state);
    std::cout << std::endl;
    std::cout << "Стресс: " << result.stress_level << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Демонстрация анализа дыхания завершена ===" << std::endl;
    std::cout << "Система успешно проанализировала различные паттерны дыхания" << std::endl;
    std::cout << "и определила соответствующие эмоциональные состояния." << std::endl;
    
    return 0;
}

