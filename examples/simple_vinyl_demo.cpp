#include "anantasound_core.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include <random>

using namespace AnantaSound;

class SimpleVinylDemo {
private:
    AnantaSoundCore sound_core_;
    std::mt19937 rng_;
    std::uniform_real_distribution<double> noise_dist_;
    
    // Параметры винилового проигрывателя
    struct VinylSettings {
        double turntable_speed = 33.33;  // RPM
        double needle_pressure = 2.0;    // граммы
        double wow_flutter = 0.1;        // процент
        double surface_noise = 0.05;     // уровень шума
        bool mono_mode = false;
    };
    
    VinylSettings vinyl_settings_;
    
public:
    SimpleVinylDemo() : sound_core_(5.0, 3.0), rng_(std::random_device{}()), 
                       noise_dist_(0.0, 1.0) {
        std::cout << "🎵 Simple Vinyl Demo Initialized" << std::endl;
    }
    
    bool initialize() {
        if (!sound_core_.initialize()) {
            std::cerr << "Failed to initialize anAntaSound core" << std::endl;
            return false;
        }
        std::cout << "✅ anAntaSound core initialized successfully" << std::endl;
        return true;
    }
    
    void demonstrateVinylEffects() {
        std::cout << "\n🎧 Vinyl Effects Demonstration" << std::endl;
        std::cout << "===============================" << std::endl;
        
        // Демонстрируем разные типы пластинок
        std::vector<std::pair<std::string, VinylSettings>> vinyl_types = {
            {"33 RPM Long Play", {33.33, 2.0, 0.1, 0.05, false}},
            {"45 RPM Single", {45.0, 2.0, 0.15, 0.08, false}},
            {"78 RPM Shellac", {78.26, 3.0, 0.5, 0.15, true}},
            {"16 RPM Transcription", {16.67, 1.5, 0.2, 0.1, false}}
        };
        
        for (const auto& vinyl_type : vinyl_types) {
            std::cout << "\n🎵 Testing: " << vinyl_type.first << std::endl;
            vinyl_settings_ = vinyl_type.second;
            
            // Симулируем воспроизведение пластинки
            simulateVinylPlayback(10.0); // 10 секунд
            
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    
    void simulateVinylPlayback(double duration) {
        std::cout << "  📀 Playing vinyl record..." << std::endl;
        std::cout << "    Speed: " << vinyl_settings_.turntable_speed << " RPM" << std::endl;
        std::cout << "    Needle pressure: " << vinyl_settings_.needle_pressure << "g" << std::endl;
        std::cout << "    Wow & flutter: " << vinyl_settings_.wow_flutter * 100 << "%" << std::endl;
        std::cout << "    Surface noise: " << vinyl_settings_.surface_noise * 100 << "%" << std::endl;
        std::cout << "    Mono mode: " << (vinyl_settings_.mono_mode ? "Yes" : "No") << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        double elapsed = 0.0;
        
        while (elapsed < duration) {
            // Создаем квантовое звуковое поле на основе виниловых параметров
            createVinylSoundField(elapsed);
            
            // Обновляем систему
            sound_core_.update(0.1);
            
            // Показываем прогресс каждые 2 секунды
            if (static_cast<int>(elapsed) % 2 == 0 && elapsed > 0) {
                double progress = (elapsed / duration) * 100.0;
                std::cout << "    📈 Progress: " << std::fixed << std::setprecision(1) 
                          << progress << "%" << std::endl;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            auto current_time = std::chrono::high_resolution_clock::now();
            elapsed = std::chrono::duration<double>(current_time - start_time).count();
        }
        
        std::cout << "  ✅ Playback completed" << std::endl;
    }
    
    void createVinylSoundField(double time) {
        // Базовые частоты для разных типов пластинок
        double base_frequency = 440.0; // A4
        
        // Корректировка частоты в зависимости от скорости
        if (vinyl_settings_.turntable_speed > 40.0) {
            base_frequency *= 1.35; // 45 RPM
        } else if (vinyl_settings_.turntable_speed > 70.0) {
            base_frequency *= 2.35; // 78 RPM
        } else if (vinyl_settings_.turntable_speed < 20.0) {
            base_frequency *= 0.5; // 16 RPM
        }
        
        // Добавляем wow & flutter эффект
        double flutter = std::sin(time * 2.0 * M_PI * 0.5) * vinyl_settings_.wow_flutter;
        base_frequency *= (1.0 + flutter);
        
        // Добавляем поверхностный шум
        double noise = noise_dist_(rng_) * vinyl_settings_.surface_noise;
        base_frequency += noise * 100.0;
        
        // Создаем сферические координаты
        SphericalCoord position{2.0 + noise, M_PI/3 + flutter, M_PI/6, time};
        
        // Создаем квантовое звуковое поле
        QuantumSoundField vinyl_field = sound_core_.createQuantumSoundField(
            base_frequency, position, QuantumSoundState::COHERENT);
        
        // Обрабатываем поле
        sound_core_.processSoundField(vinyl_field);
    }
    
    void demonstrateVideoDataExtraction() {
        std::cout << "\n📹 Video Data Extraction from Vinyl" << std::endl;
        std::cout << "====================================" << std::endl;
        
        std::cout << "🎯 Simulating video data extraction from vinyl grooves..." << std::endl;
        
        // Симулируем извлечение видеоданных из канавок винила
        for (int frame = 0; frame < 30; ++frame) {
            double time = frame / 30.0;
            
            // Создаем "видео кадр" на основе аудио данных
            VideoFrameData video_data = extractVideoFromAudio(time);
            
            // Создаем квантовое поле на основе видео данных
            createVideoBasedSoundField(video_data, time);
            
            // Обновляем систему
            sound_core_.update(0.033); // ~30 FPS
            
            if (frame % 10 == 0) {
                std::cout << "  📊 Frame " << frame << ": " 
                          << video_data.width << "x" << video_data.height 
                          << ", brightness: " << std::fixed << std::setprecision(2) 
                          << video_data.brightness << std::endl;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
        
        std::cout << "✅ Video extraction simulation completed" << std::endl;
    }
    
    struct VideoFrameData {
        int width = 640;
        int height = 480;
        double brightness = 0.5;
        std::vector<uint8_t> data;
        
        VideoFrameData() {
            data.resize(width * height * 3); // RGB
            // Заполняем случайными данными
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            for (size_t i = 0; i < data.size(); ++i) {
                data[i] = static_cast<uint8_t>(dist(gen) * 255);
            }
        }
    };
    
    VideoFrameData extractVideoFromAudio(double time) {
        VideoFrameData frame;
        
        // Симулируем извлечение видеоданных из аудио
        // В реальности это было бы сложный процесс анализа канавок
        
        // Яркость зависит от амплитуды звука
        double audio_amplitude = std::sin(time * 2.0 * M_PI * 2.0) * 0.5 + 0.5;
        frame.brightness = audio_amplitude;
        
        // Создаем паттерн на основе частоты
        double frequency = 440.0 + std::sin(time * 2.0 * M_PI * 0.5) * 200.0;
        
        // Генерируем визуальный паттерн
        for (int y = 0; y < frame.height; ++y) {
            for (int x = 0; x < frame.width; ++x) {
                int index = (y * frame.width + x) * 3;
                
                // Создаем волновой паттерн
                double wave = std::sin((x + y) * 0.01 + time * 2.0 * M_PI * frequency / 1000.0);
                double intensity = (wave + 1.0) * 0.5 * frame.brightness;
                
                frame.data[index] = static_cast<uint8_t>(intensity * 255);     // R
                frame.data[index + 1] = static_cast<uint8_t>(intensity * 200); // G
                frame.data[index + 2] = static_cast<uint8_t>(intensity * 150); // B
            }
        }
        
        return frame;
    }
    
    void createVideoBasedSoundField(const VideoFrameData& video_data, double time) {
        // Создаем звуковое поле на основе видео данных
        SphericalCoord position{3.0, M_PI/4, M_PI/3, time};
        
        // Частота зависит от яркости кадра
        double frequency = 220.0 + video_data.brightness * 400.0;
        
        QuantumSoundField video_field = sound_core_.createQuantumSoundField(
            frequency, position, QuantumSoundState::EXCITED);
        
        sound_core_.processSoundField(video_field);
    }
    
    void demonstratePlaybackHistory() {
        std::cout << "\n📚 Playback History Analysis" << std::endl;
        std::cout << "============================" << std::endl;
        
        std::cout << "🎯 Simulating analysis of vinyl playback history..." << std::endl;
        
        // Симулируем анализ истории прослушиваний
        std::vector<std::string> playback_sessions = {
            "First play - 2023-01-15",
            "Second play - 2023-02-20", 
            "Third play - 2023-03-10",
            "Fourth play - 2023-04-05"
        };
        
        for (size_t i = 0; i < playback_sessions.size(); ++i) {
            std::cout << "\n  📀 Session " << (i + 1) << ": " << playback_sessions[i] << std::endl;
            
            // Симулируем ухудшение качества с каждым прослушиванием
            double degradation = 1.0 + i * 0.1;
            vinyl_settings_.surface_noise *= degradation;
            vinyl_settings_.wow_flutter *= degradation;
            
            std::cout << "    Quality degradation: " << std::fixed << std::setprecision(1) 
                      << (degradation - 1.0) * 100 << "%" << std::endl;
            
            // Короткое воспроизведение для демонстрации
            simulateVinylPlayback(3.0);
        }
        
        std::cout << "\n✅ Playback history analysis completed" << std::endl;
    }
    
    void printStatistics() {
        std::cout << "\n📊 System Statistics" << std::endl;
        std::cout << "====================" << std::endl;
        
        auto stats = sound_core_.getStatistics();
        std::cout << "Active quantum fields: " << stats.active_fields << std::endl;
        std::cout << "Coherence ratio: " << std::fixed << std::setprecision(3) 
                  << stats.coherence_ratio << std::endl;
        std::cout << "Total processing time: " << std::fixed << std::setprecision(2) 
                  << "N/A" << "s" << std::endl;
    }
    
    void shutdown() {
        sound_core_.shutdown();
        std::cout << "Simple Vinyl Demo shutdown complete" << std::endl;
    }
};

int main() {
    std::cout << "🎬 anAntaSound Simple Vinyl Demo" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "Version: " << getVersion() << std::endl;
    std::cout << "Build Info: " << getBuildInfo() << std::endl;
    
    SimpleVinylDemo demo;
    
    if (!demo.initialize()) {
        return 1;
    }
    
    std::cout << "\nChoose demo mode:" << std::endl;
    std::cout << "1. Vinyl effects demonstration" << std::endl;
    std::cout << "2. Video data extraction simulation" << std::endl;
    std::cout << "3. Playback history analysis" << std::endl;
    std::cout << "4. Run all demonstrations" << std::endl;
    std::cout << "Enter choice (1-4): ";
    
    std::string choice;
    std::getline(std::cin, choice);
    
    if (choice == "1") {
        demo.demonstrateVinylEffects();
    } else if (choice == "2") {
        demo.demonstrateVideoDataExtraction();
    } else if (choice == "3") {
        demo.demonstratePlaybackHistory();
    } else if (choice == "4") {
        demo.demonstrateVinylEffects();
        demo.demonstrateVideoDataExtraction();
        demo.demonstratePlaybackHistory();
    } else {
        std::cout << "Invalid choice, running all demonstrations" << std::endl;
        demo.demonstrateVinylEffects();
        demo.demonstrateVideoDataExtraction();
        demo.demonstratePlaybackHistory();
    }
    
    demo.printStatistics();
    demo.shutdown();
    
    return 0;
}
