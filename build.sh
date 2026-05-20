#!/bin/bash
set -e

BUILD_DIR="build"
EXE_NAME="blur_demo"
SOURCE_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "====== Configure ======"
mkdir -p "$BUILD_DIR"
pushd "$BUILD_DIR" > /dev/null
cmake "$SOURCE_DIR" -DCMAKE_BUILD_TYPE=Release

echo ""
echo "====== Build ======"
cmake --build . --config Release
popd > /dev/null

echo ""
echo "====== Run ======"
"$BUILD_DIR/$EXE_NAME" -i frieren-winter-1.bmp -u frieren-winter-1-blur.bmp -r 128
