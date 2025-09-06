#include "anantasound_core.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <filesystem>
#include <vector>
#include <string>

using namespace AnantaSound;
namespace fs = std::filesystem;

class SamplesPlayer {
private:
    AnantaSoundCore core_;
    std::vector<std::string> sample_files_;
    size_t current_sample_index_;
    bool is_playing_;
    
public:
    SamplesPlayer() : core_(5.0, 3.0), current_sample_index_(0), is_playing_(false) {
        loadSampleFiles();
    }
    
    void loadSampleFiles() {
        std::string samples_dir = "../samples";
        if (fs::exists(samples_dir)) {
            for (const auto& entry : fs::directory_iterator(samples_dir)) {
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
        return true;
    }
    
    void playSample(const std::string& sample_name) {
        std::cout << "\n🎵 Playing: " << sample_name << std::endl;
        
        // Создаем квантовое звуковое поле для сэмпла
        SphericalCoord position{2.0, M_PI/3, M_PI/6, 0.0};
        
        // Разные частоты для разных типов сэмплов
        double base_frequency = 432.0; // Базовая частота
        if (sample_name.find("Hanuman") != std::string::npos) {
            base_frequency = 396.0; // Частота корневой чакры и стабильности
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
        
        std::cout << "  Frequency: " << main_field.frequency << " Hz" << std::endl;
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
        
        // Симуляция воспроизведения
        std::cout << "  🎧 Playing sample with quantum acoustic processing..." << std::endl;
        
        for (int frame = 0; frame < 30; ++frame) {
            core_.update(0.033); // ~30 FPS
            
            auto stats = core_.getStatistics();
            if (frame % 10 == 0) {
                std::cout << "    Frame " << frame << ": Active fields: " << stats.active_fields 
                          << ", Coherence: " << std::fixed << std::setprecision(2) << stats.coherence_ratio << std::endl;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
        
        std::cout << "  ✅ Sample playback completed" << std::endl;
    }
    
    void playAllSamples() {
        if (sample_files_.empty()) {
            std::cout << "No samples to play" << std::endl;
            return;
        }
        
        std::cout << "\n🎼 Starting anAntaSound Samples Demo" << std::endl;
        std::cout << "=====================================" << std::endl;
        
        for (const auto& sample : sample_files_) {
            playSample(sample);
            
            // Пауза между сэмплами
            if (&sample != &sample_files_.back()) {
                std::cout << "\n⏸️  Pausing between samples..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
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
        std::cout << "anAntaSound core shutdown" << std::endl;
    }
};

int main() {
    std::cout << "anAntaSound Samples Demo Application" << std::endl;
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
