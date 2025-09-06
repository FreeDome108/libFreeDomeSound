#include "audio_analyzer.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iostream>

namespace AnantaSound {

AudioAnalyzer::AudioAnalyzer(size_t fft_size, size_t sample_rate)
    : fft_size_(fft_size)
    , sample_rate_(sample_rate)
    , min_frequency_(20.0)
    , max_frequency_(sample_rate_ / 2.0)
    , hop_size_(fft_size_ / 4) {
    
    fft_buffer_.resize(fft_size_);
    generateWindowFunction();
}

bool AudioAnalyzer::initialize() {
    if (fft_size_ == 0 || sample_rate_ == 0) {
        return false;
    }
    
    // Validate FFT size is power of 2
    if ((fft_size_ & (fft_size_ - 1)) != 0) {
        std::cerr << "FFT size must be a power of 2" << std::endl;
        return false;
    }
    
    generateWindowFunction();
    return true;
}

AudioAnalysisResult AudioAnalyzer::analyzeAudio(const std::vector<double>& audio_buffer) {
    std::lock_guard<std::mutex> lock(analysis_mutex_);
    
    AudioAnalysisResult result;
    
    if (audio_buffer.empty()) {
        return result;
    }
    
    // Prepare buffer for FFT (pad with zeros if necessary)
    std::vector<double> padded_buffer = audio_buffer;
    if (padded_buffer.size() < fft_size_) {
        padded_buffer.resize(fft_size_, 0.0);
    } else if (padded_buffer.size() > fft_size_) {
        padded_buffer.resize(fft_size_);
    }
    
    // Apply window function
    applyWindow(padded_buffer);
    
    // Copy to complex buffer
    for (size_t i = 0; i < fft_size_; ++i) {
        fft_buffer_[i] = std::complex<double>(padded_buffer[i], 0.0);
    }
    
    // Perform FFT
    performFFT(fft_buffer_);
    
    // Calculate spectra
    result.magnitude_spectrum = magnitudeSpectrum(fft_buffer_);
    result.phase_spectrum = phaseSpectrum(fft_buffer_);
    result.frequency_spectrum.resize(result.magnitude_spectrum.size());
    
    // Fill frequency spectrum
    for (size_t i = 0; i < result.frequency_spectrum.size(); ++i) {
        result.frequency_spectrum[i] = getFrequency(i);
    }
    
    // Calculate analysis features
    result.fundamental_frequency = calculateFundamentalFrequency(result.magnitude_spectrum);
    result.spectral_centroid = calculateSpectralCentroid(result.magnitude_spectrum);
    result.spectral_rolloff = calculateSpectralRolloff(result.magnitude_spectrum);
    result.zero_crossing_rate = calculateZeroCrossingRate(audio_buffer);
    result.tempo = estimateTempo(audio_buffer);
    result.volume_level = calculateVolumeLevel(audio_buffer);
    result.timestamp = std::chrono::high_resolution_clock::now();
    
    return result;
}

std::vector<AudioAnalysisResult> AudioAnalyzer::analyzeAudioWithOverlap(const std::vector<double>& audio_buffer) {
    std::vector<AudioAnalysisResult> results;
    
    if (audio_buffer.size() < fft_size_) {
        results.push_back(analyzeAudio(audio_buffer));
        return results;
    }
    
    for (size_t start = 0; start + fft_size_ <= audio_buffer.size(); start += hop_size_) {
        std::vector<double> window_buffer(audio_buffer.begin() + start, 
                                        audio_buffer.begin() + start + fft_size_);
        results.push_back(analyzeAudio(window_buffer));
    }
    
    return results;
}

size_t AudioAnalyzer::getFrequencyBin(double frequency) const {
    return static_cast<size_t>(frequency * fft_size_ / sample_rate_);
}

double AudioAnalyzer::getFrequency(size_t bin) const {
    return static_cast<double>(bin * sample_rate_) / static_cast<double>(fft_size_);
}

void AudioAnalyzer::setFrequencyRange(double min_freq, double max_freq) {
    min_frequency_ = std::max(0.0, min_freq);
    max_frequency_ = std::min(static_cast<double>(sample_rate_) / 2.0, max_freq);
}

void AudioAnalyzer::setHopSize(size_t hop_size) {
    hop_size_ = std::min(fft_size_, hop_size);
}

void AudioAnalyzer::performFFT(std::vector<std::complex<double>>& data) {
    size_t n = data.size();
    
    // Bit-reverse permutation
    for (size_t i = 1, j = 0; i < n; ++i) {
        size_t bit = n >> 1;
        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }
        j ^= bit;
        
        if (i < j) {
            std::swap(data[i], data[j]);
        }
    }
    
    // FFT computation
    for (size_t len = 2; len <= n; len <<= 1) {
        double angle = -2.0 * M_PI / len;
        std::complex<double> wlen(std::cos(angle), std::sin(angle));
        
        for (size_t i = 0; i < n; i += len) {
            std::complex<double> w(1.0, 0.0);
            for (size_t j = 0; j < len / 2; ++j) {
                std::complex<double> u = data[i + j];
                std::complex<double> v = data[i + j + len / 2] * w;
                data[i + j] = u + v;
                data[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }
}

void AudioAnalyzer::generateWindowFunction() {
    window_function_.resize(fft_size_);
    
    // Generate Hann window
    for (size_t i = 0; i < fft_size_; ++i) {
        window_function_[i] = 0.5 * (1.0 - std::cos(2.0 * M_PI * i / (fft_size_ - 1)));
    }
}

double AudioAnalyzer::calculateFundamentalFrequency(const std::vector<double>& magnitude_spectrum) const {
    if (magnitude_spectrum.empty()) {
        return 0.0;
    }
    
    // Find the peak in the magnitude spectrum
    auto max_it = std::max_element(magnitude_spectrum.begin(), magnitude_spectrum.end());
    if (max_it == magnitude_spectrum.end()) {
        return 0.0;
    }
    
    size_t peak_bin = std::distance(magnitude_spectrum.begin(), max_it);
    return getFrequency(peak_bin);
}

double AudioAnalyzer::calculateSpectralCentroid(const std::vector<double>& magnitude_spectrum) const {
    if (magnitude_spectrum.empty()) {
        return 0.0;
    }
    
    double weighted_sum = 0.0;
    double magnitude_sum = 0.0;
    
    for (size_t i = 0; i < magnitude_spectrum.size(); ++i) {
        double frequency = getFrequency(i);
        double magnitude = magnitude_spectrum[i];
        weighted_sum += frequency * magnitude;
        magnitude_sum += magnitude;
    }
    
    return magnitude_sum > 0.0 ? weighted_sum / magnitude_sum : 0.0;
}

double AudioAnalyzer::calculateSpectralRolloff(const std::vector<double>& magnitude_spectrum, double threshold) const {
    if (magnitude_spectrum.empty()) {
        return 0.0;
    }
    
    double total_energy = std::accumulate(magnitude_spectrum.begin(), magnitude_spectrum.end(), 0.0);
    double target_energy = total_energy * threshold;
    double cumulative_energy = 0.0;
    
    for (size_t i = 0; i < magnitude_spectrum.size(); ++i) {
        cumulative_energy += magnitude_spectrum[i];
        if (cumulative_energy >= target_energy) {
            return getFrequency(i);
        }
    }
    
    return getFrequency(magnitude_spectrum.size() - 1);
}

double AudioAnalyzer::calculateZeroCrossingRate(const std::vector<double>& audio_buffer) const {
    if (audio_buffer.size() < 2) {
        return 0.0;
    }
    
    size_t zero_crossings = 0;
    for (size_t i = 1; i < audio_buffer.size(); ++i) {
        if ((audio_buffer[i] >= 0.0) != (audio_buffer[i-1] >= 0.0)) {
            zero_crossings++;
        }
    }
    
    return static_cast<double>(zero_crossings) / static_cast<double>(audio_buffer.size() - 1);
}

double AudioAnalyzer::estimateTempo(const std::vector<double>& audio_buffer) const {
    // Simple tempo estimation based on zero crossing rate
    double zcr = calculateZeroCrossingRate(audio_buffer);
    double estimated_bpm = zcr * 60.0 * 2.0; // Rough conversion
    return std::max(60.0, std::min(200.0, estimated_bpm));
}

double AudioAnalyzer::calculateVolumeLevel(const std::vector<double>& audio_buffer) const {
    if (audio_buffer.empty()) {
        return 0.0;
    }
    
    // Calculate RMS (Root Mean Square) volume
    double sum_squares = 0.0;
    for (double sample : audio_buffer) {
        sum_squares += sample * sample;
    }
    
    double rms = std::sqrt(sum_squares / static_cast<double>(audio_buffer.size()));
    return std::min(1.0, rms);
}

void AudioAnalyzer::applyWindow(std::vector<double>& buffer) const {
    if (buffer.size() != window_function_.size()) {
        return;
    }
    
    for (size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] *= window_function_[i];
    }
}

std::vector<double> AudioAnalyzer::magnitudeSpectrum(const std::vector<std::complex<double>>& fft_result) const {
    std::vector<double> magnitude(fft_result.size() / 2 + 1);
    
    for (size_t i = 0; i < magnitude.size(); ++i) {
        magnitude[i] = std::abs(fft_result[i]);
    }
    
    return magnitude;
}

std::vector<double> AudioAnalyzer::phaseSpectrum(const std::vector<std::complex<double>>& fft_result) const {
    std::vector<double> phase(fft_result.size() / 2 + 1);
    
    for (size_t i = 0; i < phase.size(); ++i) {
        phase[i] = std::arg(fft_result[i]);
    }
    
    return phase;
}

} // namespace AnantaSound