#!/bin/bash
set -e

# Build script for Linux/macOS
# Usage: build.sh {cpu | cuda | opencl}

if [ $# -ne 1 ]; then
    echo "Usage: $0 {cpu|cuda|opencl}"
    echo ""
    echo "  cpu    - Build CPU-only version (no GPU required)"
    echo "  cuda   - Build CUDA version (requires NVIDIA GPU + CUDA Toolkit)"
    echo "  opencl - Build OpenCL version (requires OpenCL SDK)"
    exit 1
fi

VERSION="$1"
BUILD_DIR="build"
EXE_NAME="blur_$VERSION"
ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "====== Configure ======"
cmake -B "$BUILD_DIR" "$ROOT_DIR" -DCMAKE_BUILD_TYPE=Release

echo ""
echo "====== Build ($VERSION) ======"
cmake --build "$BUILD_DIR" --config Release --target "$EXE_NAME"

echo ""
echo "====== Run ($VERSION) ======"
"$BUILD_DIR/bin/$EXE_NAME" -i frieren-winter-1.bmp -u frieren-winter-1-blur-$VERSION.bmp -r 128
