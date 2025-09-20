#include "format_handler.hpp"
#include "gpu_processor.hpp"
#include <iostream>
#include <string>

using namespace FreeDomeSound;

int main() {
    std::cout << "=== FreeDomeSound Format & GPU Demo ===" << std::endl;
    
    // Создание обработчика форматов
    FormatHandler formatHandler;
    
    // Получение поддерживаемых форматов
    auto supportedFormats = formatHandler.getSupportedFormats();
    std::cout << "Поддерживаемые форматы: ";
    for (const auto& format : supportedFormats) {
        std::cout << format << " ";
    }
    std::cout << std::endl;
    
    // Создание тестовых аудио данных
    AudioData testAudio;
    testAudio.sampleRate = 48000;
    testAudio.channels = 2;
    testAudio.bitsPerSample = 16;
    testAudio.duration = 5.0f;
    
    // Установка пространственных данных для DAGA
    testAudio.spatialData.position[0] = 1.0f;  // x
    testAudio.spatialData.position[1] = 0.0f;  // y
    testAudio.spatialData.position[2] = 0.5f;  // z
    testAudio.spatialData.quantumResonance = 528.0f; // Solfeggio частота
    
    // Создание тестовых аудио данных (синусоида)
    size_t dataSize = testAudio.sampleRate * testAudio.channels * sizeof(int16_t) * testAudio.duration;
    testAudio.data.resize(dataSize);
    
    // Заполнение синусоидой
    int16_t* samples = reinterpret_cast<int16_t*>(testAudio.data.data());
    for (size_t i = 0; i < dataSize / sizeof(int16_t); ++i) {
        double t = static_cast<double>(i) / (testAudio.sampleRate * testAudio.channels);
        samples[i] = static_cast<int16_t>(32767.0 * sin(2.0 * M_PI * 440.0 * t));
    }
    
    // Сохранение в различных форматах
    std::cout << "\n--- Сохранение аудио файлов ---" << std::endl;
    
    if (formatHandler.saveAudioFile("test.wav", testAudio)) {
        std::cout << "✓ WAV файл сохранен" << std::endl;
    } else {
        std::cout << "✗ Ошибка сохранения WAV файла" << std::endl;
    }
    
    if (formatHandler.saveAudioFile("test.daga", testAudio)) {
        std::cout << "✓ DAGA файл сохранен" << std::endl;
    } else {
        std::cout << "✗ Ошибка сохранения DAGA файла" << std::endl;
    }
    
    // Загрузка аудио файлов
    std::cout << "\n--- Загрузка аудио файлов ---" << std::endl;
    
    AudioData loadedAudio;
    if (formatHandler.loadAudioFile("test.wav", loadedAudio)) {
        std::cout << "✓ WAV файл загружен" << std::endl;
        std::cout << "  Частота дискретизации: " << loadedAudio.sampleRate << std::endl;
        std::cout << "  Каналы: " << loadedAudio.channels << std::endl;
        std::cout << "  Бит на сэмпл: " << loadedAudio.bitsPerSample << std::endl;
        std::cout << "  Длительность: " << loadedAudio.duration << " сек" << std::endl;
    } else {
        std::cout << "✗ Ошибка загрузки WAV файла" << std::endl;
    }
    
    if (formatHandler.loadAudioFile("test.daga", loadedAudio)) {
        std::cout << "✓ DAGA файл загружен" << std::endl;
        std::cout << "  Частота дискретизации: " << loadedAudio.sampleRate << std::endl;
        std::cout << "  Каналы: " << loadedAudio.channels << std::endl;
        std::cout << "  Квантовый резонанс: " << loadedAudio.spatialData.quantumResonance << " Гц" << std::endl;
        std::cout << "  Позиция: [" << loadedAudio.spatialData.position[0] << ", " 
                  << loadedAudio.spatialData.position[1] << ", " 
                  << loadedAudio.spatialData.position[2] << "]" << std::endl;
    } else {
        std::cout << "✗ Ошибка загрузки DAGA файла" << std::endl;
    }
    
    // GPU обработка
    std::cout << "\n--- GPU обработка ---" << std::endl;
    
    GPUProcessor gpuProcessor;
    
    // Попытка инициализации различных GPU
    std::vector<GPUType> gpuTypes = {
        GPUType::NVIDIA,
        GPUType::AMD,
        GPUType::INTEL,
        GPUType::APPLE,
        GPUType::SOFTWARE
    };
    
    bool gpuInitialized = false;
    for (const auto& gpuType : gpuTypes) {
        if (gpuProcessor.initialize(gpuType)) {
            std::cout << "✓ GPU инициализирован: ";
            switch (gpuType) {
                case GPUType::NVIDIA: std::cout << "NVIDIA"; break;
                case GPUType::AMD: std::cout << "AMD"; break;
                case GPUType::INTEL: std::cout << "Intel"; break;
                case GPUType::APPLE: std::cout << "Apple"; break;
                case GPUType::SOFTWARE: std::cout << "Software"; break;
            }
            std::cout << std::endl;
            gpuInitialized = true;
            break;
        }
    }
    
    if (!gpuInitialized) {
        std::cout << "✗ Не удалось инициализировать GPU" << std::endl;
        return 1;
    }
    
    // Тестирование различных режимов обработки
    std::cout << "\n--- Тестирование режимов обработки ---" << std::endl;
    
    AudioData processedAudio;
    std::vector<ProcessingMode> processingModes = {
        ProcessingMode::SPATIAL_PROCESSING,
        ProcessingMode::QUANTUM_RESONANCE,
        ProcessingMode::HOLOGRAPHIC_AUDIO,
        ProcessingMode::CONSCIOUSNESS_INTEGRATION
    };
    
    for (const auto& mode : processingModes) {
        if (gpuProcessor.processAudio(testAudio, processedAudio, mode)) {
            std::cout << "✓ Обработка завершена: ";
            switch (mode) {
                case ProcessingMode::SPATIAL_PROCESSING:
                    std::cout << "Пространственная обработка"; break;
                case ProcessingMode::QUANTUM_RESONANCE:
                    std::cout << "Квантовый резонанс"; break;
                case ProcessingMode::HOLOGRAPHIC_AUDIO:
                    std::cout << "Голографическое аудио"; break;
                case ProcessingMode::CONSCIOUSNESS_INTEGRATION:
                    std::cout << "Интеграция сознания"; break;
            }
            std::cout << std::endl;
        } else {
            std::cout << "✗ Ошибка обработки" << std::endl;
        }
    }
    
    // Сохранение обработанного аудио
    if (formatHandler.saveAudioFile("processed.daga", processedAudio)) {
        std::cout << "✓ Обработанное аудио сохранено в processed.daga" << std::endl;
    }
    
    std::cout << "\n=== Демо завершено ===" << std::endl;
    return 0;
}
