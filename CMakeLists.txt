cmake_minimum_required(VERSION 3.16)
project(anAntaSound VERSION 2.1.0 LANGUAGES CXX)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опции сборки
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(ENABLE_QUANTUM_FEEDBACK "Enable quantum feedback system" ON)
option(ENABLE_MECHANICAL_DEVICES "Enable mechanical devices" ON)
option(ENABLE_QRD_INTEGRATION "Enable QRD integration" ON)

# Настройка компилятора
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        # Use generic x86-64 for cross-compilation compatibility
        if(CMAKE_CROSSCOMPILING)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
        endif()
    endif()
endif()

# Поиск зависимостей
find_package(Threads REQUIRED)

# Поиск FFmpeg библиотек для видео поддержки
find_package(PkgConfig REQUIRED)

# Попробуем найти FFmpeg через pkg-config
pkg_check_modules(FFMPEG QUIET 
    libavformat 
    libavcodec 
    libavutil 
    libswscale 
    libswresample
)

# Если не найдено через pkg-config, используем ручной поиск
if(NOT FFMPEG_FOUND)
    message(STATUS "FFmpeg not found via pkg-config, trying manual search...")
    
    # Поиск FFmpeg в стандартных местах
    find_path(FFMPEG_INCLUDE_DIR libavformat/avformat.h
        PATHS
        /opt/homebrew/include
        /usr/local/include
        /usr/include
    )
    
    find_library(AVFORMAT_LIBRARY avformat
        PATHS
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
    )
    
    find_library(AVCODEC_LIBRARY avcodec
        PATHS
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
    )
    
    find_library(AVUTIL_LIBRARY avutil
        PATHS
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
    )
    
    find_library(SWSCALE_LIBRARY swscale
        PATHS
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
    )
    
    find_library(SWRESAMPLE_LIBRARY swresample
        PATHS
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
    )
    
    if(AVFORMAT_LIBRARY AND AVCODEC_LIBRARY AND AVUTIL_LIBRARY)
        set(FFMPEG_FOUND TRUE)
        set(FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIR})
        set(FFMPEG_LIBRARIES 
            ${AVFORMAT_LIBRARY}
            ${AVCODEC_LIBRARY}
            ${AVUTIL_LIBRARY}
            ${SWSCALE_LIBRARY}
            ${SWRESAMPLE_LIBRARY}
        )
        message(STATUS "FFmpeg found manually: ${FFMPEG_LIBRARIES}")
    else()
        message(WARNING "FFmpeg libraries not found. Video functionality will be disabled.")
        set(FFMPEG_FOUND FALSE)
    endif()
endif()

# Основная библиотека
set(ANANTASOUND_SOURCES
    src/anantasound_core.cpp
    src/audio_analyzer.cpp
    src/adaptive_audio_processor.cpp
    src/breathing_analyzer.cpp
    src/quantum_feedback_system.cpp
    src/consciousness_integration.cpp
    src/mechanical_devices.cpp
    src/qrd_integration.cpp
)

# Добавляем видео плеер только если FFmpeg найден
if(FFMPEG_FOUND)
    list(APPEND ANANTASOUND_SOURCES src/video_player.cpp)
    add_definitions(-DANANTASOUND_ENABLE_VIDEO)
    message(STATUS "Video functionality enabled")
else()
    message(STATUS "Video functionality disabled - FFmpeg not found")
endif()

add_library(anantasound_core ${ANANTASOUND_SOURCES})

# Настройка свойств библиотеки
set_target_properties(anantasound_core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "src/anantasound_core.hpp;src/audio_analyzer.hpp;src/adaptive_audio_processor.hpp;src/breathing_analyzer.hpp;src/quantum_feedback_system.hpp;src/mechanical_devices.hpp;src/consciousness_integration.hpp;src/qrd_integration.hpp;src/video_player.hpp"
)

# Подключение зависимостей
target_link_libraries(anantasound_core
    PUBLIC Threads::Threads
    # PRIVATE ${FFMPEG_LIBRARIES}  # Временно отключено
)

# Включение директорий FFmpeg (временно отключено)
# target_include_directories(anantasound_core
#     PRIVATE ${FFMPEG_INCLUDE_DIRS}
# )

# Включение директорий
target_include_directories(anantasound_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

# Компиляционные определения
target_compile_definitions(anantasound_core
    PRIVATE
        ANANTASOUND_VERSION="${PROJECT_VERSION}"
        $<$<BOOL:${ENABLE_QUANTUM_FEEDBACK}>:ENABLE_QUANTUM_FEEDBACK>
        $<$<BOOL:${ENABLE_MECHANICAL_DEVICES}>:ENABLE_MECHANICAL_DEVICES>
        $<$<BOOL:${ENABLE_QRD_INTEGRATION}>:ENABLE_QRD_INTEGRATION>
)

# Конфигурационный файл CMake
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/anantasoundConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/anantasoundConfig.cmake"
    INSTALL_DESTINATION lib/cmake/anantasound
)

write_basic_package_version_file(
    anantasoundConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Демонстрационные приложения
if(BUILD_EXAMPLES)
    add_executable(anantasound_demo
        examples/anantasound_demo.cpp
    )
    target_link_libraries(anantasound_demo PRIVATE anantasound_core)
    
    add_executable(quantum_acoustic_demo
        examples/quantum_acoustic_demo.cpp
    )
    target_link_libraries(quantum_acoustic_demo PRIVATE anantasound_core)
    
    if(ENABLE_MECHANICAL_DEVICES)
        add_executable(mechanical_devices_demo
            examples/mechanical_devices_demo.cpp
        )
        target_link_libraries(mechanical_devices_demo PRIVATE anantasound_core)
    endif()
    
    add_executable(samples_demo
        examples/samples_demo.cpp
    )
    target_link_libraries(samples_demo PRIVATE anantasound_core)
    
    add_executable(samples_demo_audio
        examples/samples_demo_audio.cpp
    )
    target_link_libraries(samples_demo_audio PRIVATE anantasound_core)
    
    add_executable(samples_demo_dual
        examples/samples_demo_dual.cpp
    )
    target_link_libraries(samples_demo_dual PRIVATE anantasound_core)
    
    add_executable(advanced_audio_demo
        examples/advanced_audio_demo.cpp
    )
    target_link_libraries(advanced_audio_demo PRIVATE anantasound_core)
    
    add_executable(adaptive_audio_demo
        examples/adaptive_audio_demo.cpp
    )
    target_link_libraries(adaptive_audio_demo PRIVATE anantasound_core)
    
    add_executable(breathing_analysis_demo
        examples/breathing_analysis_demo.cpp
    )
    target_link_libraries(breathing_analysis_demo PRIVATE anantasound_core)
    
    add_executable(comprehensive_demo
        examples/comprehensive_demo.cpp
    )
    target_link_libraries(comprehensive_demo PRIVATE anantasound_core)
    
    add_executable(flac_utility
        tools/flac_utility.cpp
    )
    target_link_libraries(flac_utility PRIVATE anantasound_core)
    
    # Виниловый видео плеер демо (отключен из-за проблем с FFmpeg)
    # add_executable(vinyl_video_demo
    #     examples/vinyl_video_demo.cpp
    # )
    # target_link_libraries(vinyl_video_demo PRIVATE anantasound_core)
    # target_include_directories(vinyl_video_demo PRIVATE ${FFMPEG_INCLUDE_DIRS})
    # target_link_libraries(vinyl_video_demo PRIVATE ${FFMPEG_LIBRARIES})
    
    # Простое виниловое демо
    add_executable(simple_vinyl_demo
        examples/simple_vinyl_demo.cpp
    )
    target_link_libraries(simple_vinyl_demo PRIVATE anantasound_core)
    
    # Подключаем аудио и видео библиотеки для macOS
    target_include_directories(samples_demo_audio PRIVATE 
        "/opt/homebrew/include"
        "/opt/homebrew/opt/libsndfile/include"
        "/opt/homebrew/opt/portaudio/include"
        "/opt/homebrew/opt/ffmpeg/include"
    )
    
    # target_include_directories(vinyl_video_demo PRIVATE 
    #     "/opt/homebrew/include"
    #     "/opt/homebrew/opt/ffmpeg/include"
    # )
    
    target_include_directories(samples_demo_dual PRIVATE 
        "/opt/homebrew/include"
        "/opt/homebrew/opt/libsndfile/include"
        "/opt/homebrew/opt/portaudio/include"
    )
    
    target_include_directories(advanced_audio_demo PRIVATE 
        "/opt/homebrew/include"
        "/opt/homebrew/opt/libsndfile/include"
        "/opt/homebrew/opt/portaudio/include"
    )
    
    target_link_libraries(samples_demo_audio PRIVATE 
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework AudioUnit"
        "-framework CoreFoundation"
        "-L/opt/homebrew/lib"
        "-lsndfile"
        "-lportaudio"
        "-lavformat"
        "-lavcodec"
        "-lavutil"
        "-lswscale"
        "-lswresample"
    )
    
    # target_link_libraries(vinyl_video_demo PRIVATE 
    #     "-framework CoreAudio"
    #     "-framework AudioToolbox"
    #     "-framework AudioUnit"
    #     "-framework CoreFoundation"
    #     "-L/opt/homebrew/lib"
    #     "-lsndfile"
    #     "-lportaudio"
    #     "-lavformat"
    #     "-lavcodec"
    #     "-lavutil"
    #     "-lswscale"
    #     "-lswresample"
    # )
    
    target_link_libraries(samples_demo_dual PRIVATE 
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework AudioUnit"
        "-framework CoreFoundation"
        "-L/opt/homebrew/lib"
        "-lsndfile"
        "-lportaudio"
    )
    
    target_link_libraries(advanced_audio_demo PRIVATE 
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework AudioUnit"
        "-framework CoreFoundation"
        "-L/opt/homebrew/lib"
        "-lsndfile"
        "-lportaudio"
    )
endif()

# Тесты
if(BUILD_TESTS)
    enable_testing()
    
    add_executable(anantasound_tests
        tests/test_main.cpp
        tests/test_anantasound_core.cpp
        tests/test_quantum_feedback.cpp
        tests/test_consciousness.cpp
        tests/test_mechanical_devices.cpp
    )
    target_link_libraries(anantasound_tests PRIVATE anantasound_core)
    
    add_test(NAME anantasound_tests COMMAND anantasound_tests)
endif()

# Установка
install(TARGETS anantasound_core
    EXPORT anantasoundTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/anantasound
)

# Установка заголовочных файлов
install(DIRECTORY src/
    DESTINATION include/anantasound
    FILES_MATCHING PATTERN "*.hpp"
)

# Экспорт целей
install(EXPORT anantasoundTargets
    FILE anantasoundTargets.cmake
    NAMESPACE anantasound::
    DESTINATION lib/cmake/anantasound
)

# Конфигурационные файлы
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/anantasoundConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/anantasoundConfigVersion.cmake"
    DESTINATION lib/cmake/anantasound
)

# Создание директории cmake если её нет
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Создание базового конфигурационного файла
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/anantasoundConfig.cmake.in" 
"@PACKAGE_INIT@
include(\"\${CMAKE_CURRENT_LIST_DIR}/anantasoundTargets.cmake\")
check_required_components(anantasound)
")
