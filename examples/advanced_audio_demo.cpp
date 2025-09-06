#include "anantasound_core.hpp"
#include "audio_analyzer.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>

using namespace AnantaSound;
namespace fs = std::filesystem;

class AdvancedAudioDemo {
private:
    AnantaSoundCore core_;
    std::vector<std::string> sample_files_;
    std::string samples_dir_;
    std::vector<std::unique_ptr<AudioAnalyzer>> analyzers_;
    
public:
    AdvancedAudioDemo() : core_(5.0, 3.0) {
        samples_dir_ = "../samples";
        loadSampleFiles();
    }
    
    void loadSampleFiles() {
        if (fs::exists(samples_dir_)) {
            for (const auto& entry : fs::directory_iterator(samples_dir_)) {
                if (entry.is_regular_file()) {
                    std::string ext = entry.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    
                    if (ext == ".wav" || ext == ".mp3" || ext == ".flac" || ext == ".aiff" || ext == ".ogg") {
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
    
    void analyzeAllSamples() {
        if (sample_files_.empty()) {
            std::cout << "No samples to analyze" << std::endl;
            return;
        }
        
        std::cout << "\n🔍 Starting Advanced Audio Analysis" << std::endl;
        std::cout << "=====================================" << std::endl;
        
        for (const auto& sample : sample_files_) {
            analyzeSample(sample);
            
            if (&sample != &sample_files_.back()) {
                std::cout << "\n⏸️  Pausing between analyses..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        
        std::cout << "\n🎉 All samples analyzed successfully!" << std::endl;
    }
    
    void analyzeSample(const std::string& sample_name) {
        std::cout << "\n🎵 Analyzing: " << sample_name << std::endl;
        
        std::string filepath = samples_dir_ + "/" + sample_name;
        
        // Создаем анализатор для этого файла
        auto analyzer = std::make_unique<AudioAnalyzer>();
        
        // Загружаем и анализируем файл
        if (!analyzer->loadAudioFile(filepath)) {
            std::cerr << "Failed to load audio file: " << sample_name << std::endl;
            return;
        }
        
        // Получаем информацию
        const auto& metadata = analyzer->getMetadata();
        const auto& info = analyzer->getAudioInfo();
        const auto& spectral = analyzer->getSpectralData();
        
        // Выводим детальную информацию
        std::cout << "  📊 File Information:" << std::endl;
        std::cout << "    Format: " << info.format << " (" << info.codec << ")" << std::endl;
        std::cout << "    Sample Rate: " << info.sample_rate << " Hz" << std::endl;
        std::cout << "    Channels: " << info.channels << std::endl;
        std::cout << "    Duration: " << std::fixed << std::setprecision(2) 
                  << info.duration_seconds << "s" << std::endl;
        std::cout << "    Total Samples: " << info.total_samples << std::endl;
        
        if (!metadata.title.empty()) {
            std::cout << "  🏷️  Metadata:" << std::endl;
            std::cout << "    Title: " << metadata.title << std::endl;
            if (!metadata.artist.empty()) std::cout << "    Artist: " << metadata.artist << std::endl;
            if (!metadata.album.empty()) std::cout << "    Album: " << metadata.album << std::endl;
            if (!metadata.genre.empty()) std::cout << "    Genre: " << metadata.genre << std::endl;
            if (metadata.year > 0) std::cout << "    Year: " << metadata.year << std::endl;
        }
        
        std::cout << "  🌊 Spectral Analysis:" << std::endl;
        std::cout << "    Dominant Frequency: " << std::fixed << std::setprecision(1) 
                  << spectral.dominant_frequency << " Hz" << std::endl;
        std::cout << "    Spectral Centroid: " << std::fixed << std::setprecision(1) 
                  << spectral.spectral_centroid << " Hz" << std::endl;
        std::cout << "    Spectral Bandwidth: " << std::fixed << std::setprecision(1) 
                  << spectral.spectral_bandwidth << " Hz" << std::endl;
        
        // Специальная обработка для FLAC файлов
        if (info.format == "FLAC") {
            std::cout << "  🎯 FLAC Quality Analysis:" << std::endl;
            if (AudioUtils::validateFLACQuality(filepath)) {
                std::cout << "    ✅ FLAC file is valid and high quality" << std::endl;
                
                // Анализ качества FLAC
                analyzeFLACQuality(filepath, info);
            } else {
                std::cout << "    ❌ FLAC file validation failed" << std::endl;
            }
        }
        
        // Квантовый анализ через anAntaSound
        performQuantumAnalysis(sample_name, info, spectral);
        
        // Экспортируем отчет анализа
        std::string report_path = "analysis_report_" + 
                                 fs::path(sample_name).stem().string() + ".txt";
        analyzer->exportAnalysisReport(report_path);
        
        // Сохраняем анализатор для дальнейшего использования
        analyzers_.push_back(std::move(analyzer));
    }
    
    void analyzeFLACQuality(const std::string& filepath, const AudioInfo& info) {
        // Дополнительный анализ качества FLAC
        std::cout << "    📈 Quality Metrics:" << std::endl;
        
        // Оценка битрейта (для FLAC это lossless)
        double file_size_mb = static_cast<double>(fs::file_size(filepath)) / (1024 * 1024);
        double duration_minutes = info.duration_seconds / 60.0;
        double compression_ratio = (file_size_mb / duration_minutes) / 10.0; // Примерная оценка
        
        std::cout << "      File Size: " << std::fixed << std::setprecision(2) 
                  << file_size_mb << " MB" << std::endl;
        std::cout << "      Compression Ratio: " << std::fixed << std::setprecision(2) 
                  << compression_ratio << " MB/min" << std::endl;
        
        // Оценка динамического диапазона
        if (info.bits_per_sample >= 16) {
            std::cout << "      Bit Depth: " << info.bits_per_sample << " bits (High Quality)" << std::endl;
        } else {
            std::cout << "      Bit Depth: " << info.bits_per_sample << " bits (Standard)" << std::endl;
        }
        
        // Оценка частоты дискретизации
        if (info.sample_rate >= 96000) {
            std::cout << "      Sample Rate: " << info.sample_rate << " Hz (Ultra High Quality)" << std::endl;
        } else if (info.sample_rate >= 48000) {
            std::cout << "      Sample Rate: " << info.sample_rate << " Hz (High Quality)" << std::endl;
        } else {
            std::cout << "      Sample Rate: " << info.sample_rate << " Hz (Standard)" << std::endl;
        }
    }
    
    void performQuantumAnalysis(const std::string& sample_name, const AudioInfo& info, 
                               const SpectralData& spectral) {
        std::cout << "  ⚛️  Quantum Acoustic Analysis:" << std::endl;
        
        // Создаем квантовое звуковое поле на основе характеристик файла
        SphericalCoord position{2.0, M_PI/3, M_PI/6, 0.0};
        
        // Определяем базовую частоту на основе спектрального анализа
        double base_frequency = spectral.dominant_frequency;
        if (base_frequency < 20.0) base_frequency = 432.0; // Дефолтная частота
        
        // Создаем основное звуковое поле
        QuantumSoundField main_field = core_.createQuantumSoundField(
            base_frequency, position, QuantumSoundState::COHERENT);
        
        std::cout << "    Base Frequency: " << std::fixed << std::setprecision(1) 
                  << base_frequency << " Hz" << std::endl;
        std::cout << "    Quantum State: COHERENT" << std::endl;
        
        // Обрабатываем поле
        core_.processSoundField(main_field);
        
        // Создаем гармоники на основе спектрального анализа
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
        
        // Симуляция обработки в реальном времени
        std::cout << "    Processing quantum acoustic system..." << std::endl;
        
        for (int frame = 0; frame < 20; ++frame) {
            core_.update(0.05); // 20 FPS
            
            auto stats = core_.getStatistics();
            if (frame % 5 == 0) {
                std::cout << "      Frame " << frame << ": Active fields: " << stats.active_fields 
                          << ", Coherence: " << std::fixed << std::setprecision(2) << stats.coherence_ratio << std::endl;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    
    void generateQualityReport() {
        if (analyzers_.empty()) {
            std::cout << "No analyzers available for quality report" << std::endl;
            return;
        }
        
        std::cout << "\n📊 Generating Quality Comparison Report" << std::endl;
        std::cout << "=====================================" << std::endl;
        
        // Сортируем файлы по качеству
        std::vector<std::pair<std::string, double>> quality_scores;
        
        for (size_t i = 0; i < analyzers_.size(); ++i) {
            const auto& analyzer = analyzers_[i];
            const auto& info = analyzer->getAudioInfo();
            const auto& spectral = analyzer->getSpectralData();
            
            // Простая оценка качества
            double quality_score = 0.0;
            
            // Оценка по частоте дискретизации
            if (info.sample_rate >= 96000) quality_score += 30;
            else if (info.sample_rate >= 48000) quality_score += 25;
            else if (info.sample_rate >= 44100) quality_score += 20;
            else quality_score += 10;
            
            // Оценка по битности
            if (info.bits_per_sample >= 24) quality_score += 25;
            else if (info.bits_per_sample >= 16) quality_score += 20;
            else quality_score += 10;
            
            // Оценка по формату
            if (info.format == "FLAC") quality_score += 25;
            else if (info.format == "WAV") quality_score += 20;
            else if (info.format == "AIFF") quality_score += 20;
            else if (info.format == "MP3") quality_score += 10;
            else quality_score += 5;
            
            // Оценка по спектральным характеристикам
            if (spectral.spectral_bandwidth > 5000) quality_score += 20;
            else if (spectral.spectral_bandwidth > 2000) quality_score += 15;
            else quality_score += 10;
            
            quality_scores.push_back({sample_files_[i], quality_score});
        }
        
        // Сортируем по качеству (по убыванию)
        std::sort(quality_scores.begin(), quality_scores.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        std::cout << "Quality Ranking (0-100 scale):" << std::endl;
        for (size_t i = 0; i < quality_scores.size(); ++i) {
            const auto& [filename, score] = quality_scores[i];
            std::cout << "  " << (i + 1) << ". " << filename << " - " 
                      << std::fixed << std::setprecision(1) << score << "/100" << std::endl;
        }
        
        // Сохраняем общий отчет
        std::ofstream report("quality_comparison_report.txt");
        if (report.is_open()) {
            report << "=== anAntaSound Quality Comparison Report ===" << std::endl;
            report << "Generated: " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
            report << std::endl;
            
            report << "Quality Ranking:" << std::endl;
            for (size_t i = 0; i < quality_scores.size(); ++i) {
                const auto& [filename, score] = quality_scores[i];
                report << "  " << (i + 1) << ". " << filename << " - " 
                       << std::fixed << std::setprecision(1) << score << "/100" << std::endl;
            }
            
            report.close();
            std::cout << "\nQuality report saved to: quality_comparison_report.txt" << std::endl;
        }
    }
    
    void interactiveMode() {
        if (sample_files_.empty()) {
            std::cout << "No samples available for interactive mode" << std::endl;
            return;
        }
        
        std::cout << "\n🎮 Interactive Mode - Choose sample to analyze:" << std::endl;
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
                    analyzeSample(sample_files_[index]);
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
    std::cout << "anAntaSound Advanced Audio Analysis Demo" << std::endl;
    std::cout << "Version: " << getVersion() << std::endl;
    std::cout << "Build Info: " << getBuildInfo() << std::endl;
    
    AdvancedAudioDemo demo;
    
    if (!demo.initialize()) {
        return 1;
    }
    
    std::cout << "\nChoose analysis mode:" << std::endl;
    std::cout << "1. Analyze all samples automatically" << std::endl;
    std::cout << "2. Interactive mode (choose samples)" << std::endl;
    std::cout << "3. Generate quality comparison report" << std::endl;
    std::cout << "Enter choice (1, 2, or 3): ";
    
    std::string choice;
    std::getline(std::cin, choice);
    
    if (choice == "1") {
        demo.analyzeAllSamples();
        demo.generateQualityReport();
    } else if (choice == "2") {
        demo.interactiveMode();
    } else if (choice == "3") {
        demo.analyzeAllSamples();
        demo.generateQualityReport();
    } else {
        std::cout << "Invalid choice, analyzing all samples automatically" << std::endl;
        demo.analyzeAllSamples();
        demo.generateQualityReport();
    }
    
    demo.shutdown();
    return 0;
}
