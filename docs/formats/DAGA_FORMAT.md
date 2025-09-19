# 🎵 DAGA AUDIO FORMAT - Техническая документация

## 🎯 О формате .daga

**DAGA** (Dome Audio with Advanced Spatial Attributes) - это продвинутый формат аудио для freedome_sphere, специально разработанный для работы с пространственным звуком и системой anAntaSound в купольном отображении.

## ✨ Особенности формата

### 🔊 Пространственный звук:
- **3D позиционирование** - Точное размещение звука в пространстве
- **Купольная реверберация** - Специальные эффекты для купольного пространства
- **Многоканальный звук** - Поддержка surround и spatial audio
- **anAntaSound интеграция** - Полная совместимость с системой

### 🎨 Продвинутые возможности:
- **Акустическое моделирование** - Учет свойств материалов купола
- **Импульсные отклики** - Специальные реверберационные эффекты
- **Синхронизация** - Точная синхронизация с визуальным контентом
- **Оптимизация** - Специальная оптимизация для мобильных устройств

## 📊 Структура файла .daga

```json
{
  "header": {
    "format": "daga",
    "version": "1.0.0",
    "created": "2024-01-01T00:00:00.000Z",
    "author": "Freedome Sphere",
    "compatibility": {
      "freedome_sphere": ">=1.0.0",
      "anantasound": ">=1.0.0",
      "mbharata_client": ">=1.0.0"
    }
  },
  "audio": {
    "id": "daga_1234567890_abc123def",
    "name": "Audio Name",
    "description": "Audio description",
    "tags": ["ambient", "spatial"],
    "created": "2024-01-01T00:00:00.000Z",
    "modified": "2024-01-01T00:00:00.000Z",
    "version": "1.0.0"
  },
  "technical": {
    "sampleRate": 44100,
    "bitDepth": 16,
    "channels": 2,
    "duration": 120.5,
    "format": "daga",
    "compression": "lossless",
    "spatialAudio": true
  },
  "spatial": {
    "enabled": true,
    "positioning": {
      "x": 0,
      "y": 0,
      "z": 0
    },
    "orientation": {
      "x": 0,
      "y": 0,
      "z": 1
    },
    "distanceModel": "exponential",
    "rolloffFactor": 1,
    "maxDistance": 100,
    "refDistance": 1
  },
  "anantasound": {
    "enabled": true,
    "domeReverb": true,
    "spatialEffects": true,
    "settings": {
      "roomSize": 0.5,
      "damping": 0.5,
      "wet": 0.3,
      "dry": 0.7,
      "preDelay": 0.03,
      "decayTime": 1.5
    }
  },
  "data": {
    "main": {
      "format": "daga_compressed",
      "data": "base64_encoded_audio_data",
      "size": 1024000
    },
    "spatial": {
      "left": "left_channel_data",
      "right": "right_channel_data",
      "center": "center_channel_data",
      "lfe": "lfe_channel_data",
      "surround": "surround_channels_data"
    },
    "dome": {
      "optimized": true,
      "projectionType": "spherical",
      "domeRadius": 10,
      "acousticProperties": {
        "absorption": 0.1,
        "reflection": 0.3,
        "diffusion": 0.5
      }
    }
  },
  "effects": {
    "reverb": {
      "enabled": true,
      "type": "dome_reverb",
      "parameters": {
        "roomSize": 0.5,
        "damping": 0.5,
        "wet": 0.3
      }
    },
    "spatial": {
      "enabled": true,
      "type": "3d_positioning",
      "parameters": {
        "distance": 1.0,
        "elevation": 0.0,
        "azimuth": 0.0
      }
    }
  },
  "sync": {
    "timeline": [],
    "markers": [],
    "tempo": 120,
    "timeSignature": "4/4"
  },
  "statistics": {
    "fileSize": 1024000,
    "duration": 120.5,
    "quality": "high",
    "spatialComplexity": "medium",
    "domeOptimized": true,
    "lastModified": "2024-01-01T00:00:00.000Z"
  }
}
```

## 🔧 Технические характеристики

### Аудио параметры:
- **Частота дискретизации**: 44.1 kHz (стандарт), 48 kHz (высокое качество)
- **Разрядность**: 16-bit (стандарт), 24-bit (высокое качество), 32-bit (профессиональное)
- **Каналы**: Моно, Стерео, Surround 5.1, 7.1
- **Сжатие**: Lossless (без потерь качества)

### Пространственные параметры:
- **Позиционирование**: 3D координаты (x, y, z)
- **Ориентация**: Направление звука
- **Дистанционная модель**: Exponential, Linear, Inverse
- **Rolloff Factor**: Коэффициент затухания
- **Max Distance**: Максимальная дистанция слышимости

### anAntaSound параметры:
- **Dome Reverb**: Купольная реверберация
- **Room Size**: Размер помещения (0.0 - 1.0)
- **Damping**: Демпфирование (0.0 - 1.0)
- **Wet/Dry**: Соотношение обработанного/исходного сигнала
- **Pre Delay**: Предзадержка реверберации
- **Decay Time**: Время затухания

## 🏗️ Акустическое моделирование

### Типы материалов купола:
- **Standard** - Стандартный материал
  - Поглощение: 0.1
  - Отражение: 0.3
  - Диффузия: 0.5

- **Acoustic** - Акустический материал
  - Поглощение: 0.3
  - Отражение: 0.2
  - Диффузия: 0.7

- **Reflective** - Отражающий материал
  - Поглощение: 0.05
  - Отражение: 0.6
  - Диффузия: 0.3

- **Absorbent** - Поглощающий материал
  - Поглощение: 0.5
  - Отражение: 0.1
  - Диффузия: 0.8

### Расчет акустических свойств:
```javascript
// Поглощение
absorption = materialFactor * (1 - (radius / 50))

// Отражение
reflection = materialFactor * (radius / 50)

// Диффузия
diffusion = materialFactor * (1 - absorption - reflection)
```

## 🎵 Импульсные отклики

### Создание купольного импульсного отклика:
```javascript
function createDomeImpulseResponse(domeSettings, sampleRate) {
    const radius = domeSettings.radius;
    const material = domeSettings.material;
    const duration = 2.0; // 2 секунды
    const length = sampleRate * duration;
    
    const impulse = new ArrayBuffer(length * 4);
    const view = new Float32Array(impulse);
    
    for (let i = 0; i < length; i++) {
        const time = i / sampleRate;
        const decay = Math.exp(-time / (duration * 0.5));
        const noise = (Math.random() * 2 - 1) * 0.1;
        const materialFactor = getMaterialFactor(material);
        
        view[i] = noise * decay * materialFactor;
    }
    
    return impulse;
}
```

## 🔄 Конвертация форматов

### Поддерживаемые входные форматы:
- **WAV** - Несжатый аудио
- **MP3** - Сжатый аудио
- **OGG** - Открытый формат
- **FLAC** - Сжатие без потерь
- **AAC** - Продвинутое сжатие

### Процесс конвертации:
1. **Загрузка** исходного аудио
2. **Анализ** технических характеристик
3. **Обработка** для пространственного звука
4. **Создание** импульсного отклика
5. **Сохранение** в формате .daga

## 📱 Оптимизация для мобильных

### Настройки сжатия:
- **Качество**: High, Medium, Low
- **Битрейт**: 128 kbps, 192 kbps, 256 kbps
- **Формат**: OGG Vorbis (оптимальный для мобильных)
- **Размер буфера**: Адаптивный

### Производительность:
- **CPU нагрузка**: Минимизирована
- **Память**: Оптимизированное использование
- **Батарея**: Энергоэффективная обработка
- **Задержка**: Минимальная латентность

## 🎯 Использование в freedome_sphere

### Загрузка .daga файла:
```javascript
// Через IPC
const result = await ipcRenderer.invoke('load-daga-file', filePath);

if (result.success) {
    const dagaData = result.data;
    // Обработка загруженных данных
}
```

### Сохранение в .daga формат:
```javascript
// Через IPC
const result = await ipcRenderer.invoke('save-daga-file', audioData, outputPath);

if (result.success) {
    console.log('Daga file saved:', result.path);
}
```

### Интеграция с anAntaSound:
```javascript
// Создание аудио источника из .daga
const audioSource = {
    id: dagaData.audio.id,
    name: dagaData.audio.name,
    position: dagaData.spatial.positioning,
    volume: 0.8,
    spatialAudio: true,
    anantasound: dagaData.anantasound
};

await anAntaSoundManager.addAudioSource(audioSource);
```

## 🐛 Отладка и валидация

### Валидация .daga файла:
```javascript
const validation = dagaFormat.validateDagaFile(dagaData);

if (!validation.valid) {
    console.error('Validation errors:', validation.errors);
}

if (validation.warnings.length > 0) {
    console.warn('Validation warnings:', validation.warnings);
}
```

### Проверка совместимости:
- **Версия формата** - Совместимость версий
- **Заголовок** - Корректность структуры
- **Аудио данные** - Наличие и валидность
- **Пространственные настройки** - Корректность параметров
- **anAntaSound настройки** - Совместимость с системой

## 📄 Лицензия

NativeMindNONC - Все права защищены.

---

*DAGA Audio Format - Продвинутый формат для пространственного звука в купольном отображении*