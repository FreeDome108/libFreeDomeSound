#ifndef GPU_PROCESSOR_HPP
#define GPU_PROCESSOR_HPP

#include "format_handler.hpp"
#include <memory>

namespace FreeDomeSound {

// Типы GPU
enum class GPUType {
    NVIDIA,
    AMD,
    INTEL,
    APPLE,
    SOFTWARE
};

// Режимы обработки
enum class ProcessingMode {
    SPATIAL_PROCESSING,        // Пространственная обработка
    QUANTUM_RESONANCE,         // Квантовый резонанс
    HOLOGRAPHIC_AUDIO,         // Голографическое аудио
    CONSCIOUSNESS_INTEGRATION  // Интеграция сознания
};

class GPUProcessor {
public:
    GPUProcessor();
    ~GPUProcessor();
    
    // Инициализация
    bool initialize(GPUType gpuType);
    
    // Основная обработка
    bool processAudio(const AudioData& input, AudioData& output, ProcessingMode mode);
    
    // Специализированная обработка
    bool processSpatialAudio(const AudioData& input, AudioData& output);
    bool processQuantumResonance(const AudioData& input, AudioData& output);
    bool processHolographicAudio(const AudioData& input, AudioData& output);
    bool processConsciousnessIntegration(const AudioData& input, AudioData& output);
    
    // Утилиты
    void cleanup();
    bool isInitialized() const;
    GPUType getGPUType() const;
    
private:
    bool initialized_;
    GPUType gpuType_;
    void* gpuContext_; // Указатель на GPU контекст
    
    // CUDA реализации
    bool processSpatialAudioCUDA(const AudioData& input, AudioData& output);
    bool processQuantumResonanceCUDA(const AudioData& input, AudioData& output);
    bool processHolographicAudioCUDA(const AudioData& input, AudioData& output);
    bool processConsciousnessIntegrationCUDA(const AudioData& input, AudioData& output);
    
    // OpenCL реализации
    bool processSpatialAudioOpenCL(const AudioData& input, AudioData& output);
    bool processQuantumResonanceOpenCL(const AudioData& input, AudioData& output);
    bool processHolographicAudioOpenCL(const AudioData& input, AudioData& output);
    bool processConsciousnessIntegrationOpenCL(const AudioData& input, AudioData& output);
    
    // Metal реализации (macOS)
    bool processSpatialAudioMetal(const AudioData& input, AudioData& output);
    bool processQuantumResonanceMetal(const AudioData& input, AudioData& output);
    bool processHolographicAudioMetal(const AudioData& input, AudioData& output);
    bool processConsciousnessIntegrationMetal(const AudioData& input, AudioData& output);
    
    // Программные реализации (fallback)
    bool processSpatialAudioSoftware(const AudioData& input, AudioData& output);
    bool processQuantumResonanceSoftware(const AudioData& input, AudioData& output);
    bool processHolographicAudioSoftware(const AudioData& input, AudioData& output);
    bool processConsciousnessIntegrationSoftware(const AudioData& input, AudioData& output);
    
    // Инициализация конкретных GPU
    bool initializeCUDA();
    bool initializeOpenCL();
    bool initializeMetal();
    bool initializeSoftware();
};

} // namespace FreeDomeSound

#endif // GPU_PROCESSOR_HPP
