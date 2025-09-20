#include "format_handler.hpp"
#include <fstream>
#include <iostream>
#include <cstring>

namespace FreeDomeSound {

FormatHandler::FormatHandler() {
    // Инициализация обработчика форматов
}

FormatHandler::~FormatHandler() {
    // Очистка ресурсов
}

bool FormatHandler::loadAudioFile(const std::string& filename, AudioData& audioData) {
    std::string extension = getFileExtension(filename);
    
    if (extension == "wav") {
        return loadWAV(filename, audioData);
    } else if (extension == "mp3") {
        return loadMP3(filename, audioData);
    } else if (extension == "flac") {
        return loadFLAC(filename, audioData);
    } else if (extension == "ogg") {
        return loadOGG(filename, audioData);
    } else if (extension == "aac") {
        return loadAAC(filename, audioData);
    } else if (extension == "daga") {
        return loadDAGA(filename, audioData);
    }
    
    return false;
}

bool FormatHandler::saveAudioFile(const std::string& filename, const AudioData& audioData) {
    std::string extension = getFileExtension(filename);
    
    if (extension == "wav") {
        return saveWAV(filename, audioData);
    } else if (extension == "mp3") {
        return saveMP3(filename, audioData);
    } else if (extension == "flac") {
        return saveFLAC(filename, audioData);
    } else if (extension == "ogg") {
        return saveOGG(filename, audioData);
    } else if (extension == "aac") {
        return saveAAC(filename, audioData);
    } else if (extension == "daga") {
        return saveDAGA(filename, audioData);
    }
    
    return false;
}

std::string FormatHandler::getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    
    std::string extension = filename.substr(dotPos + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension;
}

bool FormatHandler::loadWAV(const std::string& filename, AudioData& audioData) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Чтение WAV заголовка
    WAVHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
    
    // Проверка формата WAV
    if (memcmp(header.riff, "RIFF", 4) != 0 || memcmp(header.wave, "WAVE", 4) != 0) {
        return false;
    }
    
    audioData.sampleRate = header.sampleRate;
    audioData.channels = header.numChannels;
    audioData.bitsPerSample = header.bitsPerSample;
    audioData.duration = static_cast<float>(header.dataSize) / (header.sampleRate * header.numChannels * (header.bitsPerSample / 8));
    
    // Чтение аудио данных
    audioData.data.resize(header.dataSize);
    file.read(reinterpret_cast<char*>(audioData.data.data()), header.dataSize);
    
    return true;
}

bool FormatHandler::saveWAV(const std::string& filename, const AudioData& audioData) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    WAVHeader header;
    memcpy(header.riff, "RIFF", 4);
    header.fileSize = 36 + audioData.data.size();
    memcpy(header.wave, "WAVE", 4);
    memcpy(header.fmt, "fmt ", 4);
    header.fmtSize = 16;
    header.audioFormat = 1; // PCM
    header.numChannels = audioData.channels;
    header.sampleRate = audioData.sampleRate;
    header.byteRate = audioData.sampleRate * audioData.channels * (audioData.bitsPerSample / 8);
    header.blockAlign = audioData.channels * (audioData.bitsPerSample / 8);
    header.bitsPerSample = audioData.bitsPerSample;
    memcpy(header.data, "data", 4);
    header.dataSize = audioData.data.size();
    
    file.write(reinterpret_cast<const char*>(&header), sizeof(WAVHeader));
    file.write(reinterpret_cast<const char*>(audioData.data.data()), audioData.data.size());
    
    return true;
}

bool FormatHandler::loadMP3(const std::string& filename, AudioData& audioData) {
    // TODO: Реализовать загрузку MP3 через FFmpeg
    // Пока заглушка
    return false;
}

bool FormatHandler::saveMP3(const std::string& filename, const AudioData& audioData) {
    // TODO: Реализовать сохранение MP3 через FFmpeg
    // Пока заглушка
    return false;
}

bool FormatHandler::loadFLAC(const std::string& filename, AudioData& audioData) {
    // TODO: Реализовать загрузку FLAC
    // Пока заглушка
    return false;
}

bool FormatHandler::saveFLAC(const std::string& filename, const AudioData& audioData) {
    // TODO: Реализовать сохранение FLAC
    // Пока заглушка
    return false;
}

bool FormatHandler::loadOGG(const std::string& filename, AudioData& audioData) {
    // TODO: Реализовать загрузку OGG
    // Пока заглушка
    return false;
}

bool FormatHandler::saveOGG(const std::string& filename, const AudioData& audioData) {
    // TODO: Реализовать сохранение OGG
    // Пока заглушка
    return false;
}

bool FormatHandler::loadAAC(const std::string& filename, AudioData& audioData) {
    // TODO: Реализовать загрузку AAC
    // Пока заглушка
    return false;
}

bool FormatHandler::saveAAC(const std::string& filename, const AudioData& audioData) {
    // TODO: Реализовать сохранение AAC
    // Пока заглушка
    return false;
}

bool FormatHandler::loadDAGA(const std::string& filename, AudioData& audioData) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Чтение DAGA заголовка
    DAGAHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(DAGAHeader));
    
    // Проверка формата DAGA
    if (memcmp(header.magic, "DAGA", 4) != 0) {
        return false;
    }
    
    audioData.sampleRate = header.sampleRate;
    audioData.channels = header.channels;
    audioData.bitsPerSample = header.bitsPerSample;
    audioData.duration = header.duration;
    
    // Загрузка пространственных данных
    audioData.spatialData.position = header.position;
    audioData.spatialData.orientation = header.orientation;
    audioData.spatialData.quantumResonance = header.quantumResonance;
    
    // Чтение аудио данных
    audioData.data.resize(header.dataSize);
    file.read(reinterpret_cast<char*>(audioData.data.data()), header.dataSize);
    
    return true;
}

bool FormatHandler::saveDAGA(const std::string& filename, const AudioData& audioData) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    DAGAHeader header;
    memcpy(header.magic, "DAGA", 4);
    header.version = 1;
    header.sampleRate = audioData.sampleRate;
    header.channels = audioData.channels;
    header.bitsPerSample = audioData.bitsPerSample;
    header.duration = audioData.duration;
    header.dataSize = audioData.data.size();
    
    // Сохранение пространственных данных
    header.position = audioData.spatialData.position;
    header.orientation = audioData.spatialData.orientation;
    header.quantumResonance = audioData.spatialData.quantumResonance;
    
    file.write(reinterpret_cast<const char*>(&header), sizeof(DAGAHeader));
    file.write(reinterpret_cast<const char*>(audioData.data.data()), audioData.data.size());
    
    return true;
}

std::vector<std::string> FormatHandler::getSupportedFormats() const {
    return {"wav", "mp3", "flac", "ogg", "aac", "daga"};
}

bool FormatHandler::isFormatSupported(const std::string& extension) const {
    auto supportedFormats = getSupportedFormats();
    std::string lowerExt = extension;
    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
    
    return std::find(supportedFormats.begin(), supportedFormats.end(), lowerExt) != supportedFormats.end();
}

} // namespace FreeDomeSound
