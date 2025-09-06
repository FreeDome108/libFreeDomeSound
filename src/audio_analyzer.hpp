#pragma once

#include <vector>
#include <complex>
#include <memory>
#include <mutex>
#include <chrono>

namespace AnantaSound {

// Audio analysis results
struct AudioAnalysisResult {
    std::vector<double> frequency_spectrum;    // FFT frequency spectrum
    std::vector<double> magnitude_spectrum;    // Magnitude spectrum
    std::vector<double> phase_spectrum;        // Phase spectrum
    double fundamental_frequency;              // Fundamental frequency (Hz)
    double volume_level;                       // Volume level (0.0 - 1.0)
    double spectral_centroid;                  // Spectral centroid (Hz)
    double spectral_rolloff;                   // Spectral rolloff (Hz)
    double zero_crossing_rate;                 // Zero crossing rate
    double tempo;                              // Estimated tempo (BPM)
    std::chrono::high_resolution_clock::time_point timestamp;
    
    AudioAnalysisResult() : fundamental_frequency(0.0), volume_level(0.0),
                           spectral_centroid(0.0), spectral_rolloff(0.0),
                           zero_crossing_rate(0.0), tempo(0.0),
                           timestamp(std::chrono::high_resolution_clock::now()) {}
};

// Audio analyzer class
class AudioAnalyzer {
private:
    size_t fft_size_;
    size_t sample_rate_;
    std::vector<std::complex<double>> fft_buffer_;
    std::vector<double> window_function_;
    mutable std::mutex analysis_mutex_;
    
    // Analysis parameters
    double min_frequency_;
    double max_frequency_;
    size_t hop_size_;
    
public:
    AudioAnalyzer(size_t fft_size = 1024, size_t sample_rate = 44100);
    ~AudioAnalyzer() = default;
    
    // Initialize the analyzer
    bool initialize();
    
    // Analyze audio buffer
    AudioAnalysisResult analyzeAudio(const std::vector<double>& audio_buffer);
    
    // Analyze audio buffer with overlap
    std::vector<AudioAnalysisResult> analyzeAudioWithOverlap(const std::vector<double>& audio_buffer);
    
    // Get frequency bin for a given frequency
    size_t getFrequencyBin(double frequency) const;
    
    // Get frequency for a given bin
    double getFrequency(size_t bin) const;
    
    // Set analysis parameters
    void setFrequencyRange(double min_freq, double max_freq);
    void setHopSize(size_t hop_size);
    
    // Get current parameters
    size_t getFFTSize() const { return fft_size_; }
    size_t getSampleRate() const { return sample_rate_; }
    double getMinFrequency() const { return min_frequency_; }
    double getMaxFrequency() const { return max_frequency_; }
    
private:
    // FFT implementation (simple radix-2)
    void performFFT(std::vector<std::complex<double>>& data);
    void performIFFT(std::vector<std::complex<double>>& data);
    
    // Window function generation
    void generateWindowFunction();
    
    // Analysis helper methods
    double calculateFundamentalFrequency(const std::vector<double>& magnitude_spectrum) const;
    double calculateSpectralCentroid(const std::vector<double>& magnitude_spectrum) const;
    double calculateSpectralRolloff(const std::vector<double>& magnitude_spectrum, double threshold = 0.85) const;
    double calculateZeroCrossingRate(const std::vector<double>& audio_buffer) const;
    double estimateTempo(const std::vector<double>& audio_buffer) const;
    double calculateVolumeLevel(const std::vector<double>& audio_buffer) const;
    
    // Utility functions
    void applyWindow(std::vector<double>& buffer) const;
    std::vector<double> magnitudeSpectrum(const std::vector<std::complex<double>>& fft_result) const;
    std::vector<double> phaseSpectrum(const std::vector<std::complex<double>>& fft_result) const;
};

} // namespace AnantaSound