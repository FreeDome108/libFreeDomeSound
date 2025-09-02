# anAntaSound - Quantum Acoustic Consciousness System

**anAntaSound** - это передовая система квантовой акустики и интеграции сознания, разработанная для создания гармоничных звуковых полей и квантовых резонансов.

## 🌟 Особенности

- **Квантовая акустическая обработка** - продвинутые алгоритмы квантовой механики для звука
- **Интеграция сознания** - система анализа и модуляции сознания через звуковые поля
- **QRD интеграция** - поддержка Quantum Resonance Device для усиления эффектов
- **Механические устройства** - управление кармическими кластерами и духовными устройствами
- **Кросс-платформенность** - поддержка Linux, macOS, iOS, Android, Aurora OS, Windows

## 🚀 Быстрый старт

### Требования

- **CMake** 3.16+
- **C++17** компилятор
- **Threads** библиотека
- **Make** или **Ninja**

### Сборка для текущей платформы

```bash
# Клонировать репозиторий
git clone <repository-url>
cd anAntaSound

# Создать и перейти в build директорию
mkdir build && cd build

# Конфигурация
cmake .. -DCMAKE_BUILD_TYPE=Release

# Сборка
make -j$(nproc)

# Установка
make install
```

## 🌍 Сборка для всех платформ

### Универсальная сборка

```bash
# Собрать для всех доступных платформ
./build_all_platforms.sh
```

### Платформо-специфичная сборка

#### Linux
```bash
./build_scripts/build_linux.sh
```

#### macOS (x64 + ARM64 + Universal)
```bash
./build_scripts/build_macos.sh
```

#### iOS (Simulator + Device)
```bash
./build_scripts/build_ios.sh
```

#### Android (требует Android NDK)
```bash
# Установить ANDROID_NDK_ROOT
export ANDROID_NDK_ROOT=/path/to/android-ndk
./build_scripts/build_android.sh
```

#### Aurora OS (требует Aurora Platform SDK)
```bash
# Установить Aurora Platform SDK в /opt/aurora-platform-sdk
./build_scripts/build_avrora.sh
```

#### Windows (требует MinGW для кросс-компиляции)
```bash
# Установить MinGW-w64
# Ubuntu/Debian: sudo apt-get install mingw-w64
# macOS: brew install mingw-w64
./build_scripts/build_windows.sh
```

## 📦 Результаты сборки

После успешной сборки все файлы будут размещены в директории `install/`:

```
install/
├── linux/           # Linux x64 библиотеки и заголовки
├── macos/           # macOS x64, ARM64 и универсальные библиотеки
├── ios/             # iOS симулятор и устройство библиотеки
├── android/         # Android ABIs (arm64-v8a, armeabi-v7a, x86_64, x86)
├── aurora/          # Aurora OS ARMv7hl библиотеки
└── windows/         # Windows x64 библиотеки
```

### Распределительные архивы

Каждая платформа создает сжатые архивы:
- `anantasound-{platform}-{arch}.tar.gz`

## 🔧 Конфигурация CMake

### Основные опции

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON \
    -DENABLE_QUANTUM_FEEDBACK=ON \
    -DENABLE_MECHANICAL_DEVICES=ON \
    -DENABLE_QRD_INTEGRATION=ON
```

### Опции сборки

| Опция | Описание | По умолчанию |
|-------|----------|--------------|
| `BUILD_SHARED_LIBS` | Сборка разделяемых библиотек | ON |
| `BUILD_TESTS` | Сборка тестов | ON |
| `BUILD_EXAMPLES` | Сборка примеров | ON |
| `ENABLE_QUANTUM_FEEDBACK` | Включить квантовую обратную связь | ON |
| `ENABLE_MECHANICAL_DEVICES` | Включить механические устройства | ON |
| `ENABLE_QRD_INTEGRATION` | Включить QRD интеграцию | ON |

## 🧪 Тестирование

```bash
# Запустить все тесты
make test

# Или запустить тесты напрямую
./anantasound_tests
```

## 📚 Примеры использования

```bash
# Собрать примеры
make examples

# Запустить демо
./anantasound_demo
./quantum_acoustic_demo
./mechanical_devices_demo
```

## 🔌 Интеграция в проекты

### CMake

```cmake
find_package(anantasound REQUIRED)
target_link_libraries(your_target anantasound::anantasound_core)
```

### pkg-config

```bash
pkg-config --cflags --libs anantasound
```

## 🏗️ Архитектура

```
anAntaSound/
├── src/
│   ├── anantasound_core.cpp          # Основная система
│   ├── quantum_feedback_system.cpp   # Квантовая обратная связь
│   ├── consciousness_integration.cpp # Интеграция сознания
│   ├── mechanical_devices.cpp        # Механические устройства
│   └── qrd_integration.cpp          # QRD интеграция
├── tests/                           # Тесты
├── examples/                        # Примеры использования
├── build_scripts/                   # Скрипты сборки для платформ
└── cmake/                          # CMake конфигурация
```

## 🐛 Устранение неполадок

### Проблемы сборки

1. **Ошибка компилятора**: Убедитесь, что используется C++17
2. **Отсутствующие зависимости**: Проверьте наличие Threads библиотеки
3. **Проблемы линковки**: Убедитесь, что все исходные файлы существуют

### Проблемы платформ

- **Windows**: Установите MinGW-w64 для кросс-компиляции
- **Android**: Установите Android NDK и установите `ANDROID_NDK_ROOT`
- **iOS**: Требуется Xcode на macOS
- **Aurora**: Установите Aurora Platform SDK

## 📄 Лицензия

[Укажите вашу лицензию]

## 🤝 Вклад в проект

Мы приветствуем вклад в развитие anAntaSound! Пожалуйста, создавайте issues и pull requests.

## 📞 Поддержка

Для получения поддержки создайте issue в репозитории или свяжитесь с командой разработки.

---

**anAntaSound** - открывая новые горизонты квантовой акустики и сознания 🎵✨
