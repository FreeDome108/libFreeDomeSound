#include "anantasound_core.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <filesystem>
#include <vector>
#include <string>
#include <sndfile.h>
#include <portaudio.h>
#include <cmath>

using namespace AnantaSound;
namespace fs = std::filesystem;

class AudioPlayer {
private:
    PaStream* stream_;
    std::vector<float> audio_buffer_;
    size_t current_position_;
    bool is_playing_;
    
public:
    AudioPlayer() : stream_(nullptr), current_position_(0), is_playing_(false) {}
    
    ~AudioPlayer() {
        if (stream_) {
            Pa_CloseStream(stream_);
        }
    }
    
    bool initialize() {
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
            return false;
        }
        
        err = Pa_OpenDefaultStream(&stream_, 
                                  0,          // input channels
                                  2,          // output channels (stereo)
                                  paFloat32,  // sample format
                                  44100,      // sample rate
                                  256,        // frames per buffer
                                  audioCallback,
                                  this);
        
        if (err != paNoError) {
            std::cerr << "PortAudio stream open failed: " << Pa_GetErrorText(err) << std::endl;
            return false;
        }
        
        err = Pa_StartStream(stream_);
        if (err != paNoError) {
            std::cerr << "PortAudio stream start failed: " << Pa_GetErrorText(err) << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool loadAudioFile(const std::string& filepath) {
        SF_INFO sfinfo;
        sfinfo.format = 0;
        
        SNDFILE* sndfile = sf_open(filepath.c_str(), SFM_READ, &sfinfo);
        if (!sndfile) {
            std::cerr << "Failed to open audio file: " << sf_strerror(nullptr) << std::endl;
            return false;
        }
        
        // Читаем аудио данные
        std::vector<float> temp_buffer(sfinfo.frames * sfinfo.channels);
        sf_count_t frames_read = sf_readf_float(sndfile, temp_buffer.data(), sfinfo.frames);
        sf_close(sndfile);
        
        if (frames_read <= 0) {
            std::cerr << "Failed to read audio data" << std::endl;
            return false;
        }
        
        // Конвертируем в стерео если нужно
        audio_buffer_.clear();
        if (sfinfo.channels == 1) {
            // Моно в стерео
            for (size_t i = 0; i < frames_read; ++i) {
                float sample = temp_buffer[i];
                audio_buffer_.push_back(sample); // левый канал
                audio_buffer_.push_back(sample); // правый канал
            }
        } else {
            // Уже стерео
            audio_buffer_ = temp_buffer;
        }
        
        current_position_ = 0;
        std::cout << "  Loaded audio: " << frames_read << " frames, " 
                  << sfinfo.samplerate << " Hz, " << sfinfo.channels << " channels" << std::endl;
        
        return true;
    }
    
    void play() {
        if (audio_buffer_.empty()) {
            std::cerr << "No audio loaded" << std::endl;
            return;
        }
        
        is_playing_ = true;
        current_position_ = 0;
        std::cout << "  🎵 Playing audio..." << std::endl;
        
        // Ждем окончания воспроизведения
        size_t total_samples = audio_buffer_.size() / 2; // стерео
        size_t samples_per_second = 44100;
        double duration = static_cast<double>(total_samples) / samples_per_second;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>(duration * 1000)));
        
        is_playing_ = false;
        std::cout << "  ✅ Audio playback completed" << std::endl;
    }
    
    void stop() {
        is_playing_ = false;
    }
    
    bool isPlaying() const {
        return is_playing_;
    }
    
private:
    static int audioCallback(const void* inputBuffer, void* outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void* userData) {
        AudioPlayer* player = static_cast<AudioPlayer*>(userData);
        float* out = static_cast<float*>(outputBuffer);
        
        if (!player->isPlaying()) {
            // Воспроизводим тишину
            for (unsigned long i = 0; i < framesPerBuffer * 2; ++i) {
                out[i] = 0.0f;
            }
            return paContinue;
        }
        
        // Воспроизводим аудио
        for (unsigned long i = 0; i < framesPerBuffer; ++i) {
            if (player->current_position_ < player->audio_buffer_.size() - 1) {
                out[i * 2] = player->audio_buffer_[player->current_position_];     // левый канал
                out[i * 2 + 1] = player->audio_buffer_[player->current_position_ + 1]; // правый канал
                player->current_position_ += 2;
            } else {
                out[i * 2] = 0.0f;
                out[i * 2 + 1] = 0.0f;
            }
        }
        
        return paContinue;
    }
};

class SamplesPlayer {
private:
    AnantaSoundCore core_;
    AudioPlayer audio_player_;
    std::vector<std::string> sample_files_;
    std::string samples_dir_;
    
public:
    SamplesPlayer() : core_(5.0, 3.0) {
        samples_dir_ = "../samples";
        loadSampleFiles();
    }
    
    void loadSampleFiles() {
        if (fs::exists(samples_dir_)) {
            for (const auto& entry : fs::directory_iterator(samples_dir_)) {
                if (entry.is_regular_file()) {
                    std::string ext = entry.path().extension().string();
                    if (ext == ".wav" || ext == ".mp3" || ext == ".flac") {
                        sample_files_.push_back(entry.path().filename().string());
                    }
                }
            }
        }
        
        if (sample_files_.empty()) {
            std::cout << "No sample files found in samples directory" << std::endl;
            return;
        }
        
        std::cout << "Found " << sample_files_.size() << " sample files:" << std::endl;
        for (const auto& file : sample_files_) {
            std::cout << "  - " << file << std::endl;
        }
    }
    
    bool initialize() {
        if (!core_.initialize()) {
            std::cerr << "Failed to initialize anAntaSound core" << std::endl;
            return false;
        }
        std::cout << "anAntaSound core initialized successfully" << std::endl;
        
        if (!audio_player_.initialize()) {
            std::cerr << "Failed to initialize audio player" << std::endl;
            return false;
        }
        std::cout << "Audio player initialized successfully" << std::endl;
        
        return true;
    }
    
    void playSample(const std::string& sample_name) {
        std::cout << "\n🎵 Playing: " << sample_name << std::endl;
        
        // Загружаем аудио файл
        std::string filepath = samples_dir_ + "/" + sample_name;
        if (!audio_player_.loadAudioFile(filepath)) {
            std::cerr << "Failed to load audio file: " << sample_name << std::endl;
            return;
        }
        
        // Создаем квантовое звуковое поле для сэмпла
        SphericalCoord position{2.0, M_PI/3, M_PI/6, 0.0};
        
        // Разные частоты для разных типов сэмплов
        double base_frequency = 432.0; // Базовая частота
        if (sample_name.find("Hanuman") != std::string::npos) {
            base_frequency = 528.0; // Частота любви и трансформации
        } else if (sample_name.find("Nitai") != std::string::npos) {
            base_frequency = 639.0; // Частота гармонии отношений
        } else if (sample_name.find("Shrita") != std::string::npos) {
            base_frequency = 741.0; // Частота интуиции и пробуждения
        } else if (sample_name.find("КАСТА") != std::string::npos || sample_name.find("Вокруг ШУМ") != std::string::npos) {
            base_frequency = 396.0; // Частота корневой чакры и стабильности
        }
        
        // Создаем основное звуковое поле
        QuantumSoundField main_field = core_.createQuantumSoundField(
            base_frequency, position, QuantumSoundState::COHERENT);
        
        std::cout << "  Frequency: " << base_frequency << " Hz" << std::endl;
        std::cout << "  Position: r=" << position.r << ", θ=" << position.theta 
                  << ", φ=" << position.phi << std::endl;
        
        // Обрабатываем поле
        core_.processSoundField(main_field);
        
        // Создаем гармоники
        for (int i = 1; i <= 3; ++i) {
            double harmonic_freq = base_frequency * (i + 1);
            SphericalCoord harmonic_pos{position.r + i * 0.5, position.theta, position.phi + i * 0.1, 0.0};
            
            QuantumSoundField harmonic_field = core_.createQuantumSoundField(
                harmonic_freq, harmonic_pos, QuantumSoundState::EXCITED);
            
            core_.processSoundField(harmonic_field);
        }
        
        // Создаем интерференционное поле
        auto interference_field = std::make_unique<InterferenceField>(
            InterferenceFieldType::CONSTRUCTIVE, position, 3.0);
        
        interference_field->addSourceField(main_field);
        core_.addInterferenceField(std::move(interference_field));
        
        // Воспроизводим реальный аудио файл
        audio_player_.play();
        
        // Симуляция обработки в реальном времени
        std::cout << "  🎧 Processing with quantum acoustic system..." << std::endl;
        
        for (int frame = 0; frame < 30; ++frame) {
            core_.update(0.033); // ~30 FPS
            
            auto stats = core_.getStatistics();
            if (frame % 10 == 0) {
                std::cout << "    Frame " << frame << ": Active fields: " << stats.active_fields 
                          << ", Coherence: " << std::fixed << std::setprecision(2) << stats.coherence_ratio << std::endl;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
    }
    
    void playAllSamples() {
        if (sample_files_.empty()) {
            std::cout << "No samples to play" << std::endl;
            return;
        }
        
        std::cout << "\n🎼 Starting anAntaSound Audio Samples Demo" << std::endl;
        std::cout << "===========================================" << std::endl;
        
        for (const auto& sample : sample_files_) {
            playSample(sample);
            
            // Пауза между сэмплами
            if (&sample != &sample_files_.back()) {
                std::cout << "\n⏸️  Pausing between samples..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        
        std::cout << "\n🎉 All samples played successfully!" << std::endl;
    }
    
    void interactiveMode() {
        if (sample_files_.empty()) {
            std::cout << "No samples available for interactive mode" << std::endl;
            return;
        }
        
        std::cout << "\n🎮 Interactive Mode - Choose sample to play:" << std::endl;
        std::cout << "Enter sample number (1-" << sample_files_.size() << ") or 'q' to quit:" << std::endl;
        
        std::string input;
        while (true) {
            std::cout << "\n> ";
            std::getline(std::cin, input);
            
            if (input == "q" || input == "quit") {
                break;
            }
            
            try {
                size_t index = std::stoul(input) - 1;
                if (index < sample_files_.size()) {
                    playSample(sample_files_[index]);
                } else {
                    std::cout << "Invalid sample number. Please enter 1-" << sample_files_.size() << std::endl;
                }
            } catch (const std::exception&) {
                std::cout << "Invalid input. Please enter a number or 'q' to quit" << std::endl;
            }
        }
    }
    
    void shutdown() {
        core_.shutdown();
        Pa_Terminate();
        std::cout << "anAntaSound core and audio system shutdown" << std::endl;
    }
};

int main() {
    std::cout << "anAntaSound Audio Samples Demo Application" << std::endl;
    std::cout << "Version: " << getVersion() << std::endl;
    std::cout << "Build Info: " << getBuildInfo() << std::endl;
    
    SamplesPlayer player;
    
    if (!player.initialize()) {
        return 1;
    }
    
    std::cout << "\nChoose playback mode:" << std::endl;
    std::cout << "1. Play all samples sequentially" << std::endl;
    std::cout << "2. Interactive mode (choose samples)" << std::endl;
    std::cout << "Enter choice (1 or 2): ";
    
    std::string choice;
    std::getline(std::cin, choice);
    
    if (choice == "1") {
        player.playAllSamples();
    } else if (choice == "2") {
        player.interactiveMode();
    } else {
        std::cout << "Invalid choice, playing all samples sequentially" << std::endl;
        player.playAllSamples();
    }
    
    player.shutdown();
    return 0;
}
