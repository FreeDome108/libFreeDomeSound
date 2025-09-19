#!/bin/bash

# FreeDome Sound Linux Build Script
set -e

echo "=== Building FreeDome Sound for Linux ==="

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build/linux"
INSTALL_DIR="${PROJECT_ROOT}/install/linux"

# Clean previous build
echo "Cleaning previous build..."
rm -rf "${BUILD_DIR}"
rm -rf "${INSTALL_DIR}"

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure with CMake
echo "Configuring with CMake..."
cmake "${PROJECT_ROOT}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
    -DBUILD_SHARED_LIBS=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON \
    -DENABLE_QUANTUM_FEEDBACK=ON \
    -DENABLE_MECHANICAL_DEVICES=ON \
    -DENABLE_QRD_INTEGRATION=ON

# Build
echo "Building..."
make -j$(nproc)

# Run tests
echo "Running tests..."
make test

# Install
echo "Installing..."
make install

echo "✓ Linux build completed successfully!"
echo "Installation directory: ${INSTALL_DIR}"

# Create distribution archive
cd "${PROJECT_ROOT}/install"
tar -czf "anantasound-linux-x64.tar.gz" "linux"

echo "✓ Distribution archive created: anantasound-linux-x64.tar.gz"
