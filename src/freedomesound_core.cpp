#include "anantasound_core.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>

namespace AnantaSound {

// InterferenceField implementation
InterferenceField::InterferenceField(InterferenceFieldType type, SphericalCoord center, double radius)
    : type_(type), center_(center), field_radius_(radius) {
}

void InterferenceField::addSourceField(const QuantumSoundField& field) {
    std::lock_guard<std::mutex> lock(field_mutex_);
    source_fields_.push_back(field);
}

std::complex<double> InterferenceField::calculateInterference(const SphericalCoord& position, double time) const {
    std::lock_guard<std::mutex> lock(field_mutex_);
    
    if (source_fields_.empty()) {
        return std::complex<double>(0.0, 0.0);
    }
    
    std::complex<double> total_field(0.0, 0.0);
    
    for (const auto& field : source_fields_) {
        // Calculate distance from source to position
        double dx = position.r * sin(position.theta) * cos(position.phi) - 
                   field.position.r * sin(field.position.theta) * cos(field.position.phi);
        double dy = position.r * sin(position.theta) * sin(position.phi) - 
                   field.position.r * sin(field.position.theta) * sin(field.position.phi);
        double dz = position.height - field.position.height;
        
        double distance = std::sqrt(dx*dx + dy*dy + dz*dz);
        
        // Calculate phase delay
        double phase_delay = 2.0 * M_PI * field.frequency * distance / 343.0; // 343 m/s is speed of sound
        
        // Apply quantum state effects
        std::complex<double> quantum_factor(1.0, 0.0);
        switch (field.quantum_state) {
            case QuantumSoundState::COHERENT:
                quantum_factor = std::complex<double>(1.0, 0.0);
                break;
            case QuantumSoundState::SUPERPOSITION:
                quantum_factor = std::complex<double>(0.707, 0.707);
                break;
            case QuantumSoundState::ENTANGLED:
                quantum_factor = std::complex<double>(0.5, 0.866);
                break;
            case QuantumSoundState::COLLAPSED:
                quantum_factor = std::complex<double>(0.0, 1.0);
                break;
            default:
                quantum_factor = std::complex<double>(1.0, 0.0);
                break;
        }
        
        // Calculate field contribution
        std::complex<double> field_contribution = field.amplitude * quantum_factor * 
            std::exp(std::complex<double>(0.0, -phase_delay));
        
        total_field += field_contribution;
    }
    
    // Apply interference type effects
    switch (type_) {
        case InterferenceFieldType::CONSTRUCTIVE:
            return total_field;
        case InterferenceFieldType::DESTRUCTIVE:
            return -total_field;
        case InterferenceFieldType::PHASE_MODULATED:
            return total_field * std::exp(std::complex<double>(0.0, M_PI / 4.0));
        case InterferenceFieldType::AMPLITUDE_MODULATED:
            return total_field * (1.0 + 0.5 * std::sin(2.0 * M_PI * 10.0 * time));
        case InterferenceFieldType::QUANTUM_ENTANGLED:
            return total_field * std::complex<double>(std::cos(M_PI / 6.0), std::sin(M_PI / 6.0));
        default:
            return total_field;
    }
}

QuantumSoundField InterferenceField::quantumSuperposition(const std::vector<QuantumSoundField>& fields) const {
    if (fields.empty()) {
        return QuantumSoundField{};
    }
    
    std::complex<double> total_amplitude(0.0, 0.0);
    double total_phase = 0.0;
    double total_frequency = 0.0;
    
    for (const auto& field : fields) {
        total_amplitude += field.amplitude;
        total_phase += field.phase;
        total_frequency += field.frequency;
    }
    
    // Normalize
    size_t field_count = fields.size();
    total_amplitude /= static_cast<double>(field_count);
    total_phase /= static_cast<double>(field_count);
    total_frequency /= static_cast<double>(field_count);
    
    // Create superposition field
    QuantumSoundField superposition;
    superposition.amplitude = total_amplitude;
    superposition.phase = total_phase;
    superposition.frequency = total_frequency;
    superposition.quantum_state = QuantumSoundState::SUPERPOSITION;
    superposition.position = center_;
    
    return superposition;
}

void InterferenceField::updateQuantumState(double dt) {
    std::lock_guard<std::mutex> lock(field_mutex_);
    
    for (auto& field : source_fields_) {
        // Simple quantum state evolution
        switch (field.quantum_state) {
            case QuantumSoundState::EXCITED:
                // Decay to ground state
                if (dt > 0.1) {
                    field.quantum_state = QuantumSoundState::GROUND;
                }
                break;
            case QuantumSoundState::SUPERPOSITION:
                // Maintain superposition for a while
                break;
            case QuantumSoundState::ENTANGLED:
                // Entangled states are stable
                break;
            default:
                break;
        }
    }
}

void InterferenceField::createQuantumEntanglement(size_t field1_idx, size_t field2_idx) {
    std::lock_guard<std::mutex> lock(field_mutex_);
    
    if (field1_idx < source_fields_.size() && field2_idx < source_fields_.size()) {
        source_fields_[field1_idx].quantum_state = QuantumSoundState::ENTANGLED;
        source_fields_[field2_idx].quantum_state = QuantumSoundState::ENTANGLED;
        entangled_pairs_.emplace_back(field1_idx, field2_idx);
    }
}

size_t InterferenceField::getEntangledPairsCount() const {
    std::lock_guard<std::mutex> lock(field_mutex_);
    return entangled_pairs_.size();
}

// DomeAcousticResonator implementation
DomeAcousticResonator::DomeAcousticResonator(double radius, double height)
    : dome_radius_(radius), dome_height_(height) {
    // Calculate fundamental resonant frequencies
    resonant_frequencies_ = calculateEigenFrequencies();
}

std::vector<double> DomeAcousticResonator::calculateEigenFrequencies() const {
    std::vector<double> frequencies;
    
    // Fundamental frequency (0,1) mode
    double f01 = 343.0 / (2.0 * M_PI) * std::sqrt(std::pow(1.84 / dome_radius_, 2) + 
                                                   std::pow(M_PI / dome_height_, 2));
    frequencies.push_back(f01);
    
    // Higher order modes
    for (int n = 1; n <= 5; ++n) {
        for (int m = 0; m <= 3; ++m) {
            double jnm = 0.0;
            if (n == 1 && m == 0) jnm = 3.83;
            else if (n == 2 && m == 0) jnm = 7.02;
            else if (n == 0 && m == 1) jnm = 1.84;
            else if (n == 1 && m == 1) jnm = 5.33;
            else continue;
            
            double freq = 343.0 / (2.0 * M_PI) * std::sqrt(std::pow(jnm / dome_radius_, 2) + 
                                                           std::pow(m * M_PI / dome_height_, 2));
            frequencies.push_back(freq);
        }
    }
    
    std::sort(frequencies.begin(), frequencies.end());
    return frequencies;
}

void DomeAcousticResonator::setMaterialProperties(const std::map<double, double>& properties) {
    acoustic_properties_ = properties;
}

double DomeAcousticResonator::calculateReverbTime(double frequency) const {
    // Default reverb time calculation
    double rt60 = 0.161 * dome_radius_ * dome_height_ / (0.1 * dome_radius_ + 0.1 * dome_height_);
    
    // Adjust based on material properties if available
    auto it = acoustic_properties_.find(frequency);
    if (it != acoustic_properties_.end()) {
        rt60 *= it->second;
    }
    
    return rt60;
}

// AnantaSoundCore implementation
AnantaSoundCore::AnantaSoundCore(double radius, double height)
    : dome_radius_(radius)
    , dome_height_(height)
    , quantum_uncertainty_(0.1)
    , is_initialized_(false) {
    
    dome_resonator_ = std::make_unique<DomeAcousticResonator>(radius, height);
}

AnantaSoundCore::~AnantaSoundCore() {
    shutdown();
}

bool AnantaSoundCore::initialize() {
    if (is_initialized_) {
        return true;
    }
    
    try {
        // Initialize dome resonator
        if (dome_resonator_) {
            // Set default material properties
            std::map<double, double> properties;
            properties[440.0] = 1.0; // Standard A note
            properties[880.0] = 0.8; // A octave
            dome_resonator_->setMaterialProperties(properties);
        }
        
        is_initialized_ = true;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize AnantaSoundCore: " << e.what() << std::endl;
        return false;
    }
}

void AnantaSoundCore::shutdown() {
    if (!is_initialized_) {
        return;
    }
    
    // Clear all fields
    {
        std::lock_guard<std::mutex> lock(core_mutex_);
        interference_fields_.clear();
        sound_fields_.clear();
    }
    
    is_initialized_ = false;
}

void AnantaSoundCore::addInterferenceField(std::unique_ptr<InterferenceField> field) {
    if (!is_initialized_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(core_mutex_);
    interference_fields_.push_back(std::move(field));
}

void AnantaSoundCore::removeInterferenceField(size_t field_index) {
    if (!is_initialized_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(core_mutex_);
    if (field_index < interference_fields_.size()) {
        interference_fields_.erase(interference_fields_.begin() + field_index);
    }
}

QuantumSoundField AnantaSoundCore::createQuantumSoundField(double frequency, 
                                                          const SphericalCoord& position,
                                                          QuantumSoundState state) {
    QuantumSoundField field;
    field.amplitude = std::complex<double>(1.0, 0.0);
    field.frequency = frequency;
    field.phase = 0.0;
    field.quantum_state = state;
    field.position = position;
    field.timestamp = std::chrono::high_resolution_clock::now();
    
    return field;
}

void AnantaSoundCore::processSoundField(const QuantumSoundField& input_field) {
    if (!is_initialized_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(core_mutex_);
    
    // Store the field
    sound_fields_[input_field.position] = input_field;
    
    // Apply quantum uncertainty
    if (quantum_uncertainty_ > 0.0) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::normal_distribution<double> dist(0.0, quantum_uncertainty_);
        
        // Add quantum noise to amplitude
        double noise = dist(gen);
        sound_fields_[input_field.position].amplitude += std::complex<double>(noise, noise);
    }
}

std::vector<QuantumSoundField> AnantaSoundCore::getOutputFields() const {
    if (!is_initialized_) {
        return {};
    }
    
    std::lock_guard<std::mutex> lock(core_mutex_);
    
    std::vector<QuantumSoundField> output_fields;
    output_fields.reserve(sound_fields_.size());
    
    for (const auto& [pos, field] : sound_fields_) {
        output_fields.push_back(field);
    }
    
    return output_fields;
}

void AnantaSoundCore::update(double dt) {
    if (!is_initialized_) {
        return;
    }
    
    // Update interference fields
    for (auto& field : interference_fields_) {
        if (field) {
            field->updateQuantumState(dt);
        }
    }
    
    // Update quantum effects
    static double time_accumulator = 0.0;
    time_accumulator += dt;
    
    if (time_accumulator >= 0.016) { // ~60 FPS
        // Simulate quantum decoherence
        for (auto& [pos, field] : sound_fields_) {
            if (field.quantum_state == QuantumSoundState::SUPERPOSITION) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                static std::uniform_real_distribution<double> dist(0.0, 1.0);
                
                if (dist(gen) < 0.05) { // 5% chance of decoherence
                    field.quantum_state = QuantumSoundState::GROUND;
                }
            }
        }
        time_accumulator = 0.0;
    }
}

AnantaSoundCore::SystemStatistics AnantaSoundCore::getStatistics() const {
    SystemStatistics stats;
    
    if (!is_initialized_) {
        return stats;
    }
    
    std::lock_guard<std::mutex> lock(core_mutex_);
    
    stats.active_fields = sound_fields_.size();
    
    // Count entangled pairs from interference fields
    stats.entangled_pairs = 0;
    for (const auto& field : interference_fields_) {
        if (field) {
            stats.entangled_pairs += field->getEntangledPairsCount();
        }
    }
    
    // Calculate coherence ratio based on quantum states
    stats.coherence_ratio = calculateCoherenceRatio();
    
    // Calculate energy efficiency based on field amplitudes
    stats.energy_efficiency = calculateEnergyEfficiency();
    
    // Check QRD connection status
    stats.qrd_connected = checkQRDConnection();
    
    // Count active mechanical devices
    stats.mechanical_devices_active = countActiveMechanicalDevices();
    
    return stats;
}

// Helper methods implementation
double AnantaSoundCore::calculateCoherenceRatio() const {
    if (sound_fields_.empty()) {
        return 0.0;
    }
    
    size_t coherent_fields = 0;
    size_t total_fields = sound_fields_.size();
    
    for (const auto& [pos, field] : sound_fields_) {
        if (field.quantum_state == QuantumSoundState::COHERENT || 
            field.quantum_state == QuantumSoundState::SUPERPOSITION) {
            coherent_fields++;
        }
    }
    
    return static_cast<double>(coherent_fields) / static_cast<double>(total_fields);
}

double AnantaSoundCore::calculateEnergyEfficiency() const {
    if (sound_fields_.empty()) {
        return 1.0;
    }
    
    double total_energy = 0.0;
    double max_possible_energy = 0.0;
    
    for (const auto& [pos, field] : sound_fields_) {
        double field_energy = std::abs(field.amplitude);
        total_energy += field_energy;
        max_possible_energy += 1.0; // Assuming max amplitude is 1.0
    }
    
    if (max_possible_energy == 0.0) {
        return 1.0;
    }
    
    return total_energy / max_possible_energy;
}

bool AnantaSoundCore::checkQRDConnection() const {
    // Simulate QRD connection check
    // In a real implementation, this would check actual hardware connection
    return !sound_fields_.empty() && interference_fields_.size() > 0;
}

size_t AnantaSoundCore::countActiveMechanicalDevices() const {
    // Simulate mechanical device counting
    // In a real implementation, this would check actual device status
    size_t device_count = 0;
    
    // Count devices based on active fields and their quantum states
    for (const auto& [pos, field] : sound_fields_) {
        if (field.quantum_state == QuantumSoundState::EXCITED ||
            field.quantum_state == QuantumSoundState::ENTANGLED) {
            device_count++;
        }
    }
    
    return device_count;
}

// QuantumAcousticProcessor implementation
QuantumAcousticProcessor::QuantumAcousticProcessor(size_t max_fields)
    : processing_enabled_(true) {
    processing_thread_ = std::thread(&QuantumAcousticProcessor::processingLoop, this);
}

QuantumAcousticProcessor::~QuantumAcousticProcessor() {
    processing_enabled_ = false;
    if (processing_thread_.joinable()) {
        processing_thread_.join();
    }
}

void QuantumAcousticProcessor::addField(const QuantumSoundField& field) {
    std::lock_guard<std::mutex> lock(fields_mutex_);
    fields_.push_back(field);
}

std::vector<QuantumSoundField> QuantumAcousticProcessor::getProcessedFields() const {
    std::lock_guard<std::mutex> lock(fields_mutex_);
    return fields_;
}

void QuantumAcousticProcessor::setProcessingEnabled(bool enabled) {
    processing_enabled_ = enabled;
}

void QuantumAcousticProcessor::processingLoop() {
    while (processing_enabled_) {
        std::lock_guard<std::mutex> lock(fields_mutex_);
        
        for (auto& field : fields_) {
            // Apply quantum processing
            field.amplitude *= std::exp(std::complex<double>(0.0, field.phase));
            
            // Update quantum state
            if (field.quantum_state == QuantumSoundState::SUPERPOSITION) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                static std::uniform_real_distribution<double> dist(0.0, 1.0);
                
                if (dist(gen) < 0.1) { // 10% chance of collapse
                    field.quantum_state = QuantumSoundState::COLLAPSED;
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
}



// Global functions
std::string getVersion() {
    return "2.1.0";
}

std::string getBuildInfo() {
    std::string info = "anAntaSound v" + getVersion() + "\n";
    info += "Platform: ";
    
#ifdef _WIN32
    info += "Windows";
#elif defined(__APPLE__)
    info += "macOS/iOS";
#elif defined(__ANDROID__)
    info += "Android";
#elif defined(__linux__)
    info += "Linux";
#else
    info += "Unknown";
#endif
    
    info += "\nCompiler: ";
#ifdef __clang__
    info += "Clang";
#elif defined(__GNUC__)
    info += "GCC";
#elif defined(_MSC_VER)
    info += "MSVC";
#else
    info += "Unknown";
#endif
    
    info += "\nC++ Standard: 17";
    
    return info;
}

} // namespace AnantaSound
