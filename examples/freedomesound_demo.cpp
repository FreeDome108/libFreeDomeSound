#include "anantasound_core.hpp"
#include <iostream>
#include <chrono>
#include <thread>

using namespace AnantaSound;

int main() {
    std::cout << "anAntaSound Demo Application" << std::endl;
    std::cout << "Version: " << getVersion() << std::endl;
    std::cout << "Build Info: " << getBuildInfo() << std::endl;
    
    // Создание основной системы
    AnantaSoundCore core(3.0, 2.0);  // Радиус 3м, высота 2м
    
    if (!core.initialize()) {
        std::cerr << "Failed to initialize anAntaSound core" << std::endl;
        return 1;
    }
    
    std::cout << "anAntaSound core initialized successfully" << std::endl;
    
    // Создание квантового звукового поля
    SphericalCoord position{1.0, M_PI/4, M_PI/4, 1.0};
    QuantumSoundField field = core.createQuantumSoundField(432.0, position, QuantumSoundState::COHERENT);
    
    std::cout << "Created quantum sound field at frequency: " << field.frequency << " Hz" << std::endl;
    
    // Обработка поля
    core.processSoundField(field);
    
    // Создание интерференционного поля
    auto interference_field = std::make_unique<InterferenceField>(
        InterferenceFieldType::CONSTRUCTIVE, position, 2.0);
    
    interference_field->addSourceField(field);
    core.addInterferenceField(std::move(interference_field));
    
    std::cout << "Added interference field" << std::endl;
    
    // Симуляция обработки
    for (int i = 0; i < 10; ++i) {
        core.update(0.016); // 60 FPS
        
        auto stats = core.getStatistics();
        std::cout << "Frame " << i << ": Active fields: " << stats.active_fields 
                  << ", Coherence: " << stats.coherence_ratio << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    std::cout << "Demo completed successfully" << std::endl;
    core.shutdown();
    
    return 0;
}
