#!/bin/bash
set -e

if [ $# -ne 1 ]; then
    echo "Usage: $0 {cpu|cuda|opencl}"
    echo ""
    echo "  cpu    - Build CPU-only version (no GPU required)"
    echo "  cuda   - Build CUDA version (requires NVIDIA GPU + CUDA Toolkit)"
    echo "  opencl - Build OpenCL version (requires OpenCL SDK)"
    exit 1
fi

VERSION="$1"
BUILD_DIR="build/$VERSION"
EXE_NAME="blur_$VERSION"
SOURCE_DIR="$(cd "$(dirname "$0")/$VERSION" && pwd)"

echo "====== Configure ($VERSION) ======"
mkdir -p "$BUILD_DIR"
pushd "$BUILD_DIR" > /dev/null
cmake "$SOURCE_DIR" -DCMAKE_BUILD_TYPE=Release

echo ""
echo "====== Build ($VERSION) ======"
cmake --build . --config Release
popd > /dev/null

echo ""
echo "====== Run ($VERSION) ======"
"$BUILD_DIR/$EXE_NAME" -i frieren-winter-1.bmp -u frieren-winter-1-blur-$VERSION.bmp -r 128
