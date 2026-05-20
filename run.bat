@echo off
chcp 65001 >nul

echo ====== Build CUDABlurDemo ======
if not exist build mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed
    pause
    exit /b %ERRORLEVEL%
)
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed
    pause
    exit /b %ERRORLEVEL%
)
cd ..

echo.
echo ====== Run blur ======
build\Release\blur_demo.exe -i frieren-winter-1.bmp -u frieren-winter-1-blur.bmp -r 128

pause
