#!/bin/bash
set -e

echo "====== Build CUDABlurDemo ======"
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cd ..

echo ""
echo "====== Run blur ======"
if [ -f build/blur_demo ]; then
    EXE="build/blur_demo"
elif [ -f build/Release/blur_demo ]; then
    EXE="build/Release/blur_demo"
else
    echo "Executable not found"
    exit 1
fi

"$EXE" -i frieren-winter-1.bmp -u frieren-winter-1-blur.bmp -r 128
