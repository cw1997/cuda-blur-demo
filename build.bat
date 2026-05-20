@echo off
chcp 65001 >nul

if "%1"=="" (
    echo Usage: build.bat {cpu^|cuda^|opencl}
    echo.
    echo   cpu    - Build CPU-only version (no GPU required)
    echo   cuda   - Build CUDA version (requires NVIDIA GPU + CUDA Toolkit)
    echo   opencl - Build OpenCL version (requires OpenCL SDK)
    exit /b 1
)

set VERSION=%1
set BUILD_DIR=build\%VERSION%
set EXE_NAME=blur_%VERSION%
set SOURCE_DIR=%~dp0%VERSION%
if "%SOURCE_DIR:~-1%"=="\" set SOURCE_DIR=%SOURCE_DIR:~0,-1%

echo ====== Configure (%VERSION%) ======
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
pushd "%BUILD_DIR%"
cmake "%SOURCE_DIR%" -G "Visual Studio 18 2026" -A x64 -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ====== Build (%VERSION%) ======
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed
    pause
    exit /b %ERRORLEVEL%
)
popd

echo.
echo ====== Run (%VERSION%) ======
"%BUILD_DIR%\Release\%EXE_NAME%.exe" -i frieren-winter-1.bmp -u frieren-winter-1-blur-%VERSION%.bmp -r 16

pause
