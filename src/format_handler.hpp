#ifndef FORMAT_HANDLER_HPP
#define FORMAT_HANDLER_HPP

#include <string>
#include <vector>
#include <cstring>
#include <algorithm>

namespace FreeDomeSound {

// Структура для хранения аудио данных
struct AudioData {
    std::vector<uint8_t> data;
    uint32_t sampleRate;
    uint16_t channels;
    uint16_t bitsPerSample;
    float duration;
    
    // Пространственные данные для DAGA формата
    struct SpatialData {
        float position[3];        // x, y, z позиция
        float orientation[3];     // x, y, z ориентация
        float quantumResonance;   // Квантовый резонанс
    } spatialData;
    
    AudioData() : sampleRate(44100), channels(2), bitsPerSample(16), duration(0.0f) {
        memset(spatialData.position, 0, sizeof(spatialData.position));
        memset(spatialData.orientation, 0, sizeof(spatialData.orientation));
        spatialData.quantumResonance = 440.0f;
    }
};

// WAV заголовок
struct WAVHeader {
    char riff[4];           // "RIFF"
    uint32_t fileSize;      // Размер файла - 8
    char wave[4];           // "WAVE"
    char fmt[4];            // "fmt "
    uint32_t fmtSize;       // Размер fmt блока
    uint16_t audioFormat;   // Формат аудио (1 = PCM)
    uint16_t numChannels;   // Количество каналов
    uint32_t sampleRate;    // Частота дискретизации
    uint32_t byteRate;      // Байт в секунду
    uint16_t blockAlign;    // Выравнивание блока
    uint16_t bitsPerSample; // Бит на сэмпл
    char data[4];           // "data"
    uint32_t dataSize;      // Размер аудио данных
};

// DAGA заголовок (квантовый аудио формат)
struct DAGAHeader {
    char magic[4];          // "DAGA"
    uint32_t version;       // Версия формата
    uint32_t sampleRate;    // Частота дискретизации
    uint16_t channels;      // Количество каналов
    uint16_t bitsPerSample; // Бит на сэмпл
    float duration;         // Длительность в секундах
    uint32_t dataSize;      // Размер аудио данных
    
    // Пространственные данные
    float position[3];      // x, y, z позиция
    float orientation[3];   // x, y, z ориентация
    float quantumResonance; // Квантовый резонанс
};

class FormatHandler {
public:
    FormatHandler();
    ~FormatHandler();
    
    // Основные методы загрузки и сохранения
    bool loadAudioFile(const std::string& filename, AudioData& audioData);
    bool saveAudioFile(const std::string& filename, const AudioData& audioData);
    
    // Утилиты
    std::string getFileExtension(const std::string& filename);
    std::vector<std::string> getSupportedFormats() const;
    bool isFormatSupported(const std::string& extension) const;
    
private:
    // Методы для конкретных форматов
    bool loadWAV(const std::string& filename, AudioData& audioData);
    bool saveWAV(const std::string& filename, const AudioData& audioData);
    
    bool loadMP3(const std::string& filename, AudioData& audioData);
    bool saveMP3(const std::string& filename, const AudioData& audioData);
    
    bool loadFLAC(const std::string& filename, AudioData& audioData);
    bool saveFLAC(const std::string& filename, const AudioData& audioData);
    
    bool loadOGG(const std::string& filename, AudioData& audioData);
    bool saveOGG(const std::string& filename, const AudioData& audioData);
    
    bool loadAAC(const std::string& filename, AudioData& audioData);
    bool saveAAC(const std::string& filename, const AudioData& audioData);
    
    bool loadDAGA(const std::string& filename, AudioData& audioData);
    bool saveDAGA(const std::string& filename, const AudioData& audioData);
};

} // namespace FreeDomeSound

#endif // FORMAT_HANDLER_HPP
