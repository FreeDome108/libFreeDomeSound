#!/bin/bash

# FreeDome Sound Universal Build Script
# Supports: Linux, macOS, iOS, Android, Aurora OS, Windows

set -e

echo "=== FreeDome Sound Universal Build Script ==="
echo "Building for multiple platforms..."

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
INSTALL_DIR="${PROJECT_ROOT}/install"

# Clean previous builds
echo "Cleaning previous builds..."
rm -rf "${BUILD_DIR}"
rm -rf "${INSTALL_DIR}"

# Detect host platform
detect_host_platform() {
    case "$(uname -s)" in
        Linux*)     echo "Linux";;
        Darwin*)    echo "macOS";;
        CYGWIN*|MINGW*|MSYS*) echo "Windows";;
        *)          echo "Unknown";;
    esac
}

HOST_PLATFORM=$(detect_host_platform)
echo "Host platform: ${HOST_PLATFORM}"

# Build for Linux
echo "=== Building for Linux ==="
if command -v gcc >/dev/null 2>&1; then
    echo "Building Linux x64..."
    ./build_scripts/build_linux.sh
    echo "✓ Linux x64 build completed"
else
    echo "⚠ GCC not found, skipping Linux build"
fi

# Build for macOS
echo "=== Building for macOS ==="
if [[ "${HOST_PLATFORM}" == "macOS" ]]; then
    echo "Building macOS..."
    ./build_scripts/build_macos.sh
    echo "✓ macOS build completed"
else
    echo "⚠ Not on macOS, skipping macOS build"
fi

# Build for iOS
echo "=== Building for iOS ==="
if [[ "${HOST_PLATFORM}" == "macOS" ]] && command -v xcodebuild >/dev/null 2>&1; then
    echo "Building iOS..."
    ./build_scripts/build_ios.sh
    echo "✓ iOS builds completed"
else
    echo "⚠ iOS build requires macOS with Xcode, skipping"
fi

# Build for Android
echo "=== Building for Android ==="
if [[ -n "${ANDROID_NDK_ROOT}" ]] || [[ -n "${ANDROID_NDK}" ]]; then
    echo "Building Android..."
    ./build_scripts/build_android.sh
    echo "✓ Android builds completed"
else
    echo "⚠ ANDROID_NDK_ROOT not set, skipping Android build"
fi

# Build for Aurora OS
echo "=== Building for Aurora OS ==="
if [[ -d "/opt/aurora-platform-sdk" ]]; then
    echo "Building Aurora OS..."
    ./build_scripts/build_avrora.sh
    echo "✓ Aurora OS build completed"
else
    echo "⚠ Aurora Platform SDK not found, skipping Aurora build"
fi

# Build for Windows
echo "=== Building for Windows ==="
if command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then
    echo "Building Windows..."
    ./build_scripts/build_windows.sh
    echo "✓ Windows build completed"
elif [[ "${HOST_PLATFORM}" == "Windows" ]]; then
    echo "Building Windows natively..."
    ./build_scripts/build_windows.sh
    echo "✓ Windows build completed"
else
    echo "⚠ MinGW cross-compiler not found, skipping Windows build"
fi

echo ""
echo "=== Build Summary ==="
echo "Built platforms:"
for dir in "${INSTALL_DIR}"/*; do
    if [[ -d "$dir" ]]; then
        platform=$(basename "$dir")
        lib_count=$(find "$dir" -name "*.so" -o -name "*.dylib" -o -name "*.dll" -o -name "*.a" | wc -l)
        echo "  ✓ ${platform} (${lib_count} libraries)"
    fi
done

echo ""
echo "Installation directory: ${INSTALL_DIR}"
echo "Build completed successfully!"

# Create distribution archives
echo "Creating distribution archives..."
cd "${INSTALL_DIR}"
for dir in */; do
    if [[ -d "$dir" ]]; then
        platform=$(basename "$dir")
        echo "Creating archive for ${platform}..."
        tar -czf "freedomesound-${platform}.tar.gz" "$dir"
    fi
done

echo "✓ Distribution archives created"
echo "=== Build process completed ==="
