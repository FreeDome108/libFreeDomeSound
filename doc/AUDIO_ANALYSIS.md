# anAntaSound Audio Analysis System

## Обзор

Система анализа аудио файлов anAntaSound предоставляет расширенные возможности для работы с различными аудио форматами, включая FLAC, WAV, MP3, AIFF и OGG. Система интегрирована с квантовой акустической обработкой для создания уникального опыта анализа звука.

## Основные возможности

### 🎵 Поддерживаемые форматы
- **FLAC** - Free Lossless Audio Codec (без потерь качества)
- **WAV** - Waveform Audio File Format
- **MP3** - MPEG-1 Audio Layer III
- **AIFF** - Audio Interchange File Format
- **OGG** - Ogg Vorbis

### 🔍 Анализ качества
- Спектральный анализ частот
- Анализ метаданных (ID3, Vorbis tags)
- Валидация FLAC файлов
- Оценка качества по различным параметрам
- Квантовый акустический анализ

### 🛠️ Утилиты
- Конвертация между форматами
- Нормализация аудио
- Изменение частоты дискретизации
- Пакетная обработка файлов
- Генерация отчетов анализа

## Быстрый старт

### 1. Анализ отдельного файла

```cpp
#include "audio_analyzer.hpp"

using namespace AnantaSound;

AudioAnalyzer analyzer;
if (analyzer.loadAudioFile("sample.flac")) {
    const auto& metadata = analyzer.getMetadata();
    const auto& info = analyzer.getAudioInfo();
    const auto& spectral = analyzer.getSpectralData();
    
    std::cout << "Title: " << metadata.title << std::endl;
    std::cout << "Sample Rate: " << info.sample_rate << " Hz" << std::endl;
    std::cout << "Dominant Frequency: " << spectral.dominant_frequency << " Hz" << std::endl;
}
```

### 2. Валидация FLAC файла

```cpp
if (AudioUtils::validateFLACQuality("sample.flac")) {
    std::cout << "FLAC file is valid and high quality!" << std::endl;
}
```

### 3. Получение информации о файле

```cpp
std::string info = AudioUtils::getFileInfo("sample.flac");
std::cout << info << std::endl;
```

## Демонстрационные приложения

### advanced_audio_demo

Полнофункциональное демо приложение для анализа аудио файлов:

```bash
# Сборка
cd build
make advanced_audio_demo

# Запуск
./advanced_audio_demo
```

**Режимы работы:**
1. **Автоматический анализ** - анализ всех файлов в папке samples
2. **Интерактивный режим** - выбор файлов для анализа
3. **Отчет о качестве** - сравнение качества всех файлов

### flac_utility

Командная утилита для работы с FLAC файлами:

```bash
# Сборка
cd build
make flac_utility

# Валидация FLAC файла
./flac_utility validate sample.flac

# Полная информация о файле
./flac_utility info sample.flac

# Полный анализ
./flac_utility analyze sample.flac

# Пакетный анализ папки
./flac_utility batch ./samples

# Конвертация формата
./flac_utility convert input.wav output.flac

# Нормализация
./flac_utility normalize input.flac output.flac -1.0

# Изменение частоты дискретизации
./flac_utility resample input.flac output.flac 48000
```

## Структура данных

### AudioMetadata

```cpp
struct AudioMetadata {
    std::string title;      // Название трека
    std::string artist;     // Исполнитель
    std::string album;      // Альбом
    std::string genre;      // Жанр
    int year;               // Год выпуска
    int track_number;       // Номер трека
    std::string comment;    // Комментарий
    std::string copyright;  // Авторские права
    std::string software;   // Программное обеспечение
};
```

### AudioInfo

```cpp
struct AudioInfo {
    int sample_rate;        // Частота дискретизации (Гц)
    int channels;           // Количество каналов
    int bits_per_sample;    // Битность
    double duration_seconds; // Длительность (секунды)
    int64_t total_samples;  // Общее количество сэмплов
    std::string format;     // Формат файла
    std::string codec;      // Кодек
};
```

### SpectralData

```cpp
struct SpectralData {
    std::vector<std::complex<double>> fft_data;  // FFT данные
    std::vector<double> frequencies;             // Частоты
    std::vector<double> magnitudes;              // Амплитуды
    std::vector<double> phases;                  // Фазы
    double dominant_frequency;                   // Доминирующая частота
    double spectral_centroid;                    // Спектральный центроид
    double spectral_rolloff;                     // Спектральный rolloff
    double spectral_bandwidth;                   // Спектральная ширина
};
```

## Квантовый акустический анализ

Система интегрирована с квантовой акустикой anAntaSound:

```cpp
// Создание квантового звукового поля на основе анализа
double base_frequency = spectral.dominant_frequency;
if (base_frequency < 20.0) base_frequency = 432.0; // Дефолтная частота

QuantumSoundField main_field = core_.createQuantumSoundField(
    base_frequency, position, QuantumSoundState::COHERENT);

// Обработка поля
core_.processSoundField(main_field);

// Создание гармоник
for (int i = 1; i <= 3; ++i) {
    double harmonic_freq = base_frequency * (i + 1);
    QuantumSoundField harmonic_field = core_.createQuantumSoundField(
        harmonic_freq, harmonic_pos, QuantumSoundState::EXCITED);
    core_.processSoundField(harmonic_field);
}
```

## Оценка качества

Система автоматически оценивает качество аудио файлов по шкале 0-100:

### Критерии оценки:

- **Частота дискретизации** (30 баллов)
  - 96+ кГц: 30 баллов
  - 48 кГц: 25 баллов
  - 44.1 кГц: 20 баллов
  - <44.1 кГц: 10 баллов

- **Битность** (25 баллов)
  - 24+ бит: 25 баллов
  - 16 бит: 20 баллов
  - <16 бит: 10 баллов

- **Формат** (25 баллов)
  - FLAC: 25 баллов
  - WAV/AIFF: 20 баллов
  - MP3: 10 баллов
  - Другие: 5 баллов

- **Спектральные характеристики** (20 баллов)
  - Широкая полоса (>5 кГц): 20 баллов
  - Средняя полоса (2-5 кГц): 15 баллов
  - Узкая полоса (<2 кГц): 10 баллов

## Экспорт отчетов

### Индивидуальный отчет анализа

Для каждого файла создается детальный отчет:

```
=== anAntaSound Audio Analysis Report ===
File: sample.flac
Format: FLAC (Free Lossless Audio Codec)

--- Metadata ---
Title: Sample Track
Artist: Unknown Artist
Album: Unknown Album
Genre: Unknown
Year: 0

--- Technical Info ---
Sample Rate: 44100 Hz
Channels: 2
Bits per Sample: 16
Duration: 3.45 seconds
Total Samples: 152145

--- Spectral Analysis ---
Dominant Frequency: 440.0 Hz
Spectral Centroid: 1250.5 Hz
Spectral Bandwidth: 3200.2 Hz
Spectral Rolloff: 0.85
```

### Отчет сравнения качества

Общий отчет для всех проанализированных файлов:

```
=== anAntaSound Quality Comparison Report ===
Generated: 1703123456789

Quality Ranking:
  1. sample.flac - 95.0/100
  2. sample.wav - 85.0/100
  3. sample.mp3 - 65.0/100
```

## Зависимости

### Обязательные библиотеки:
- **libsndfile** - чтение/запись аудио файлов
- **PortAudio** - воспроизведение аудио
- **C++17** - стандарт языка

### Опциональные библиотеки:
- **TagLib** - извлечение метаданных
- **FFTW** - быстрое преобразование Фурье (для полного FFT)

## Установка зависимостей

### macOS (Homebrew)
```bash
brew install libsndfile portaudio taglib fftw
```

### Ubuntu/Debian
```bash
sudo apt-get install libsndfile1-dev portaudio19-dev libtag1-dev libfftw3-dev
```

### CentOS/RHEL
```bash
sudo yum install libsndfile-devel portaudio-devel taglib-devel fftw-devel
```

## Примеры использования

### Анализ качества FLAC файлов

```cpp
// Проверка качества FLAC
std::vector<std::string> flac_files = {"track1.flac", "track2.flac", "track3.flac"};

for (const auto& file : flac_files) {
    AudioAnalyzer analyzer;
    if (analyzer.loadAudioFile(file)) {
        const auto& info = analyzer.getAudioInfo();
        
        if (info.format == "FLAC") {
            std::cout << "Analyzing FLAC quality: " << file << std::endl;
            
            // Специальный анализ для FLAC
            if (AudioUtils::validateFLACQuality(file)) {
                std::cout << "  ✅ High quality FLAC" << std::endl;
                std::cout << "  📊 Sample rate: " << info.sample_rate << " Hz" << std::endl;
                std::cout << "  🎯 Bit depth: " << info.bits_per_sample << " bits" << std::endl;
            }
        }
    }
}
```

### Пакетная обработка

```cpp
// Анализ всех аудио файлов в папке
std::string samples_dir = "./samples";
std::vector<std::pair<std::string, double>> quality_scores;

for (const auto& entry : fs::directory_iterator(samples_dir)) {
    if (entry.is_regular_file()) {
        std::string ext = entry.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".flac" || ext == ".wav" || ext == ".mp3") {
            AudioAnalyzer analyzer;
            if (analyzer.loadAudioFile(entry.path().string())) {
                // Расчет оценки качества
                double score = calculateQualityScore(analyzer);
                quality_scores.push_back({entry.path().filename().string(), score});
            }
        }
    }
}

// Сортировка по качеству
std::sort(quality_scores.begin(), quality_scores.end(),
          [](const auto& a, const auto& b) { return a.second > b.second; });

// Вывод результатов
for (const auto& [filename, score] : quality_scores) {
    std::cout << filename << ": " << score << "/100" << std::endl;
}
```

## Расширение функциональности

### Добавление нового формата

```cpp
bool AudioAnalyzer::detectFormat(const std::string& file_path) {
    std::string extension = fs::path(file_path).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".flac") {
        info_.format = "FLAC";
        info_.codec = "Free Lossless Audio Codec";
    } else if (extension == ".m4a") {  // Новый формат
        info_.format = "M4A";
        info_.codec = "MPEG-4 Audio";
    }
    // ... другие форматы
    
    return true;
}
```

### Кастомный анализ качества

```cpp
double AudioAnalyzer::calculateCustomQualityScore() {
    double score = 0.0;
    
    // Ваша логика оценки качества
    if (info_.sample_rate >= 96000) score += 30;
    if (info_.bits_per_sample >= 24) score += 25;
    if (info_.format == "FLAC") score += 25;
    
    // Дополнительные критерии
    if (spectral_data_.spectral_bandwidth > 8000) score += 20;
    
    return std::min(score, 100.0);
}
```

## Устранение неполадок

### Проблемы с зависимостями

**Ошибка: "libsndfile not found"**
```bash
# macOS
brew install libsndfile

# Ubuntu
sudo apt-get install libsndfile1-dev

# Проверка установки
pkg-config --modversion sndfile
```

**Ошибка: "PortAudio not found"**
```bash
# macOS
brew install portaudio

# Ubuntu
sudo apt-get install portaudio19-dev
```

### Проблемы с компиляцией

**Ошибка C++17**
```bash
# Убедитесь что используется C++17
export CXXFLAGS="-std=c++17"
cmake .. -DCMAKE_CXX_STANDARD=17
```

**Ошибка линковки**
```bash
# Проверьте что все библиотеки найдены
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
```

## Лицензия

[Укажите вашу лицензию]

## Поддержка

Для получения поддержки:
1. Создайте issue в репозитории
2. Опишите проблему подробно
3. Приложите логи ошибок
4. Укажите версию системы и компилятора

---

**anAntaSound Audio Analysis System** - открывая новые горизонты анализа звука с квантовой точностью 🎵⚛️
