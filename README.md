# 🎵 FreeDome Sound - Quantum Audio Processing Library

**FreeDome Sound** is an advanced quantum-enhanced audio processing library designed for immersive dome projection systems. It provides spatial audio processing, quantum feedback systems, consciousness integration capabilities, and specialized audio format support including the revolutionary **DAGA** format.

## 🌟 Key Features

### 🔬 Quantum Audio Technologies
- **Quantum Acoustic Processing** - Advanced quantum mechanics algorithms for audio
- **Quantum Feedback Systems** - Real-time quantum resonance processing
- **Consciousness Integration** - Audio-consciousness interface systems
- **108-Element Quantum Geometry** - Spatial audio based on quantum principles

### 🎧 Audio Processing Capabilities
- **Spatial Audio Processing** - 3D positional audio for dome environments
- **Ambisonic Processing** - Up to 3rd order ambisonics support
- **Adaptive Audio Processing** - Real-time audio adaptation
- **Breathing Analysis** - Biometric audio feedback integration

### 🎬 Advanced Media Support
- **DAGA Format** - Quantum audio format with spatial attributes
- **Video Processing** - FFmpeg-based video playback with audio effects
- **Vinyl Effects Simulation** - Authentic vinyl record sound processing
- **Multi-format Support** - WAV, MP3, FLAC, OGG, AAC

### 🏗️ Professional Features
- **Mechanical Devices Support** - Integration with physical audio hardware
- **QRD Integration** - Quantum Resonance Device connectivity
- **Cross-platform Support** - Linux, macOS, Windows, iOS, Android

## 🚀 Quick Start

### Prerequisites

- **CMake** 3.16+
- **C++17** compatible compiler
- **FFmpeg** (for video functionality)
- **PortAudio** and **libsndfile** (for audio I/O)
- **pkg-config**

### Installation

#### Using pkg-config (Recommended)
```bash
pkg-config --cflags --libs freedomesound
```

#### Using CMake
```cmake
find_package(FreeDomeSound REQUIRED)
target_link_libraries(your_target FreeDomeSound::freedomesound_core)
```

### Building from Source

```bash
# Clone the repository
git clone <repository-url>
cd FreeDomeSound

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DBUILD_SHARED_LIBS=ON \
         -DBUILD_EXAMPLES=ON \
         -DENABLE_QUANTUM_FEEDBACK=ON

# Build
make -j$(nproc)

# Install
sudo make install
```

## 📚 Documentation

### Format Specifications
- **[DAGA Format](docs/formats/DAGA_FORMAT.md)** - Quantum audio format specification
- **[Audio Formats](docs/formats/)** - Supported audio format documentation

### Quantum Technologies
- **[Quantum Physics](docs/quantum/QUANTUM_PHYSICS.md)** - Mathematical foundations
- **[Quantum Technologies](docs/quantum/QUANTUM_TECHNOLOGIES.md)** - Technical overview

## 🔧 Configuration Options

| Option | Description | Default |
|--------|-------------|---------|
| `BUILD_SHARED_LIBS` | Build shared libraries | ON |
| `BUILD_TESTS` | Build test suite | ON |
| `BUILD_EXAMPLES` | Build example applications | ON |
| `ENABLE_QUANTUM_FEEDBACK` | Enable quantum feedback systems | ON |
| `ENABLE_MECHANICAL_DEVICES` | Enable mechanical device support | ON |
| `ENABLE_QRD_INTEGRATION` | Enable QRD integration | ON |

## 🎵 DAGA Audio Format

FreeDome Sound introduces the **DAGA** (Dome Audio with Advanced Spatial Attributes) format:

### Key Features:
- **3D Spatial Positioning** - Precise audio positioning in 3D space
- **Quantum Resonance Integration** - Built-in quantum frequency support
- **Dome Optimization** - Specialized for dome projection environments
- **Ambisonic Support** - Full ambisonic audio processing
- **Real-time Processing** - Optimized for real-time applications

### Example Usage:
```cpp
#include <freedomesound/freedomesound_core.hpp>

// Load DAGA file
auto audio_source = FreeDomeSound::loadDAGA("audio.daga");

// Configure spatial positioning
audio_source.setPosition({0.0f, 0.0f, 1.0f});
audio_source.setOrientation({0.0f, 1.0f, 0.0f});

// Enable quantum resonance
audio_source.enableQuantumResonance(528.0f); // Solfeggio frequency

// Process audio
audio_source.process();
```

## 🧪 Examples and Demos

### Basic Audio Processing
```bash
./freedomesound_demo
```

### Quantum Acoustic Demo
```bash
./quantum_acoustic_demo
```

### Advanced Audio Features
```bash
./advanced_audio_demo
./adaptive_audio_demo
./breathing_analysis_demo
```

### Vinyl Effects Demo
```bash
./vinyl_video_demo
./simple_vinyl_demo
```

## 🏗️ Architecture

```
FreeDomeSound/
├── src/
│   ├── freedomesound_core.cpp          # Main library core
│   ├── audio_analyzer.cpp              # Audio analysis systems
│   ├── adaptive_audio_processor.cpp    # Adaptive processing
│   ├── quantum_feedback_system.cpp     # Quantum feedback
│   ├── consciousness_integration.cpp   # Consciousness interface
│   ├── mechanical_devices.cpp          # Hardware integration
│   └── qrd_integration.cpp            # QRD connectivity
├── docs/
│   ├── formats/                        # Format specifications
│   └── quantum/                        # Quantum documentation
├── examples/                           # Usage examples
├── tests/                              # Test suite
└── debian/                            # Debian packaging
```

## 📦 Debian/Ubuntu Packages

FreeDome Sound is available as Debian/Ubuntu packages:

### Runtime Library
```bash
sudo apt install libfreedomesound2
```

### Development Package
```bash
sudo apt install libfreedomesound-dev
```

### Command-line Tools
```bash
sudo apt install freedomesound-tools
```

## 🔊 Audio Processing Pipeline

1. **Input Processing** - Multi-format audio input support
2. **Quantum Analysis** - 108-element quantum geometry processing
3. **Spatial Processing** - 3D positional audio calculation
4. **Resonance Application** - Quantum frequency resonance
5. **Dome Optimization** - Specialized dome audio processing
6. **Output Rendering** - Multi-channel audio output

## 🌍 Platform Support

- **Linux** (x86_64, ARM64)
- **macOS** (Intel, Apple Silicon, Universal)
- **Windows** (x86_64)
- **iOS** (Device, Simulator)
- **Android** (ARM64, ARMv7, x86_64, x86)

## 🐛 Troubleshooting

### Build Issues
- Ensure C++17 compiler support
- Verify FFmpeg installation for video features
- Check pkg-config availability for dependencies

### Runtime Issues
- Verify audio device permissions
- Check DAGA file format validity
- Ensure quantum feedback system initialization

## 📄 License

FreeDome Sound is licensed under [License Name]. See LICENSE file for details.

## 🤝 Contributing

We welcome contributions to FreeDome Sound! Please see CONTRIBUTING.md for guidelines.

## 📞 Support

For support, please create an issue in the repository or contact the development team.

---

**FreeDome Sound** - Revolutionizing audio processing with quantum technology 🎵✨