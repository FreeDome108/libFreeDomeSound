#pragma once

#include <complex>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <string>
#include <cmath>
#include <thread>
#include <atomic>

namespace AnantaSound {

// Перечисления для квантовых состояний
enum class QuantumSoundState {
    GROUND,         // Основное состояние
    EXCITED,        // Возбужденное состояние
    SUPERPOSITION,  // Суперпозиция состояний
    COHERENT,       // Когерентное состояние
    ENTANGLED,      // Запутанное состояние
    COLLAPSED       // Коллапсированное состояние
};

// Сферические координаты
struct SphericalCoord {
    double r;       // Радиус
    double theta;   // Полярный угол (0 до π)
    double phi;     // Азимутальный угол (0 до 2π)
    double t;       // Время
    double height;  // Высота (для совместимости с кодом)
    
    SphericalCoord(double radius = 0.0, double polar = 0.0, double azimuth = 0.0, double time = 0.0, double h = 0.0)
        : r(radius), theta(polar), phi(azimuth), t(time), height(h) {}
    
    // Оператор сравнения для использования в std::map
    bool operator<(const SphericalCoord& other) const {
        if (r != other.r) return r < other.r;
        if (theta != other.theta) return theta < other.theta;
        if (phi != other.phi) return phi < other.phi;
        if (t != other.t) return t < other.t;
        return height < other.height;
    }
};

// Квантовое звуковое поле
struct QuantumSoundField {
    std::complex<double> amplitude;    // Амплитуда (комплексная)
    double phase;                       // Фаза
    double frequency;                   // Частота
    QuantumSoundState quantum_state;   // Квантовое состояние
    SphericalCoord position;           // Позиция в пространстве
    std::chrono::high_resolution_clock::time_point timestamp; // Временная метка
    
    QuantumSoundField() : amplitude(0.0, 0.0), phase(0.0), frequency(0.0), 
                          quantum_state(QuantumSoundState::GROUND), 
                          position(), timestamp(std::chrono::high_resolution_clock::now()) {}
};

// Типы интерференции
enum class InterferenceFieldType {
    CONSTRUCTIVE,   // Конструктивная интерференция
    DESTRUCTIVE,    // Деструктивная интерференция
    MIXED,          // Смешанная интерференция
    PHASE_MODULATED,    // Фазово-модулированная
    AMPLITUDE_MODULATED, // Амплитудно-модулированная
    QUANTUM_ENTANGLED    // Квантово-запутанная
};

// Интерференционное поле
class InterferenceField {
private:
    InterferenceFieldType type_;
    SphericalCoord center_;
    double radius_;
    std::vector<QuantumSoundField> source_fields_;
    std::vector<std::pair<size_t, size_t>> entangled_pairs_;
    double field_radius_;
    mutable std::mutex field_mutex_;

public:
    InterferenceField(InterferenceFieldType type, SphericalCoord center, double radius);
    
    // Добавить источник звукового поля
    void addSourceField(const QuantumSoundField& field);
    
    // Вычислить результирующую интерференцию в точке
    std::complex<double> calculateInterference(const SphericalCoord& position, double time) const;
    
    // Квантовая суперпозиция полей
    QuantumSoundField quantumSuperposition(const std::vector<QuantumSoundField>& fields) const;
    
    // Обновить поле с учетом квантовых эффектов
    void updateQuantumState(double dt);
    
    // Создать квантовую запутанность между полями
    void createQuantumEntanglement(size_t field1_idx, size_t field2_idx);
    
    // Получить количество запутанных пар
    size_t getEntangledPairsCount() const;
};

// Акустический резонатор для купола
class DomeAcousticResonator {
private:
    double dome_radius_;
    double dome_height_;
    std::vector<double> resonant_frequencies_;
    std::map<double, double> acoustic_properties_;

public:
    DomeAcousticResonator(double radius, double height);
    
    // Вычислить собственные частоты купола
    std::vector<double> calculateEigenFrequencies() const;
    
    // Моделирование акустических свойств материалов
    void setMaterialProperties(const std::map<double, double>& properties);
    
    // Вычислить время реверберации
    double calculateReverbTime(double frequency) const;
    
    // Оптимизация частотной характеристики
    void optimizeFrequencyResponse(const std::vector<double>& target_frequencies);
};

// Квантовый акустический процессор
class QuantumAcousticProcessor {
private:
    std::vector<QuantumSoundField> fields_;
    std::atomic<bool> processing_enabled_;
    std::thread processing_thread_;
    mutable std::mutex fields_mutex_;

public:
    explicit QuantumAcousticProcessor(size_t max_fields);
    ~QuantumAcousticProcessor();
    
    void addField(const QuantumSoundField& field);
    std::vector<QuantumSoundField> getProcessedFields() const;
    void setProcessingEnabled(bool enabled);
    
private:
    void processingLoop();
};



// Основной класс AnantaSound
class AnantaSoundCore {
private:
    std::vector<std::unique_ptr<InterferenceField>> interference_fields_;
    std::unique_ptr<DomeAcousticResonator> dome_resonator_;
    std::map<SphericalCoord, QuantumSoundField> sound_fields_;
    mutable std::mutex core_mutex_;
    
    // Параметры системы
    double dome_radius_;
    double dome_height_;
    double quantum_uncertainty_;
    bool is_initialized_;

public:
    AnantaSoundCore(double radius, double height);
    ~AnantaSoundCore();
    
    // Инициализация системы
    bool initialize();
    void shutdown();
    
    // Управление интерференционными полями
    void addInterferenceField(std::unique_ptr<InterferenceField> field);
    void removeInterferenceField(size_t field_index);
    
    // Создание квантовых звуковых полей
    QuantumSoundField createQuantumSoundField(double frequency, 
                                              const SphericalCoord& position,
                                              QuantumSoundState state);
    
    // Обработка звукового поля
    void processSoundField(const QuantumSoundField& input_field);
    
    // Получение результирующего звукового поля
    std::vector<QuantumSoundField> getOutputFields() const;
    
    // Обновление системы
    void update(double dt);
    
    // Получение статистики системы
    struct SystemStatistics {
        size_t active_fields;
        size_t entangled_pairs;
        double coherence_ratio;
        double energy_efficiency;
        bool qrd_connected;
        size_t mechanical_devices_active;
    };
    
    SystemStatistics getStatistics() const;
    
private:
    // Helper methods for statistics calculation
    double calculateCoherenceRatio() const;
    double calculateEnergyEfficiency() const;
    bool checkQRDConnection() const;
    size_t countActiveMechanicalDevices() const;
};

// Объявления внешних функций
std::string getVersion();
std::string getBuildInfo();

// Включение видео плеера только если включена поддержка видео
#ifdef ANANTASOUND_ENABLE_VIDEO
#include "video_player.hpp"
#endif

} // namespace AnantaSound
