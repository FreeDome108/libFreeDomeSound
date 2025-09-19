#include "anantasound_core.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace AnantaSound;

void test_quantum_sound_field() {
    std::cout << "Testing QuantumSoundField creation..." << std::endl;
    
    QuantumSoundField field;
    field.amplitude = std::complex<double>(1.0, 0.5);
    field.phase = M_PI / 4.0;
    field.frequency = 432.0;
    field.quantum_state = QuantumSoundState::COHERENT;
    field.position = {1.0, M_PI/4, M_PI/2, 1.0};
    field.timestamp = std::chrono::high_resolution_clock::now();
    
    assert(std::abs(field.amplitude.real() - 1.0) < 1e-6);
    assert(std::abs(field.amplitude.imag() - 0.5) < 1e-6);
    assert(std::abs(field.frequency - 432.0) < 1e-6);
    assert(field.quantum_state == QuantumSoundState::COHERENT);
    
    std::cout << "✓ QuantumSoundField test passed" << std::endl;
}

void test_interference_field() {
    std::cout << "Testing InterferenceField..." << std::endl;
    
    SphericalCoord center{1.0, M_PI/4, M_PI/4, 1.0};
    InterferenceField field(InterferenceFieldType::CONSTRUCTIVE, center, 2.0);
    
    QuantumSoundField source_field;
    source_field.amplitude = std::complex<double>(1.0, 0.0);
    source_field.phase = 0.0;
    source_field.frequency = 432.0;
    source_field.quantum_state = QuantumSoundState::COHERENT;
    source_field.position = center;
    source_field.timestamp = std::chrono::high_resolution_clock::now();
    
    field.addSourceField(source_field);
    
    SphericalCoord test_position{1.5, M_PI/3, M_PI/3, 1.0};
    auto interference = field.calculateInterference(test_position, 0.0);
    
    assert(std::abs(interference) > 0.0);
    
    std::cout << "✓ InterferenceField test passed" << std::endl;
}

void test_dome_acoustic_resonator() {
    std::cout << "Testing DomeAcousticResonator..." << std::endl;
    
    DomeAcousticResonator resonator(3.0, 2.0);
    
    auto frequencies = resonator.calculateEigenFrequencies();
    assert(!frequencies.empty());
    
    double reverb_time = resonator.calculateReverbTime(440.0);
    assert(reverb_time > 0.0);
    
    std::cout << "✓ DomeAcousticResonator test passed" << std::endl;
}

void test_anantasound_core() {
    std::cout << "Testing AnantaSoundCore..." << std::endl;
    
    AnantaSoundCore core(3.0, 2.0);
    
    assert(core.initialize());
    
    SphericalCoord position{1.0, M_PI/4, M_PI/4, 1.0};
    auto field = core.createQuantumSoundField(432.0, position, QuantumSoundState::COHERENT);
    
    assert(std::abs(field.frequency - 432.0) < 1e-6);
    
    core.processSoundField(field);
    
    auto stats = core.getStatistics();
    assert(stats.active_fields >= 0);
    
    core.shutdown();
    
    std::cout << "✓ AnantaSoundCore test passed" << std::endl;
}


