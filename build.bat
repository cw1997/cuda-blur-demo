@echo off
chcp 65001 >nul

:: Build script for Windows (MSVC + Visual Studio generator)
:: Usage: build.bat {cpu | cuda | opencl}

if "%1"=="" (
    echo Usage: build.bat {cpu^| cuda^| opencl}
    echo.
    echo   cpu    - Build CPU-only version (no GPU required)
    echo   cuda   - Build CUDA version (requires NVIDIA GPU + CUDA Toolkit)
    echo   opencl - Build OpenCL version (requires OpenCL SDK)
    exit /b 1
)

set VERSION=%1
set BUILD_DIR=build
set EXE_NAME=blur_%VERSION%
set ROOT_DIR=%~dp0

echo ====== Configure ======
cmake -B "%BUILD_DIR%" "%ROOT_DIR%" -G "Visual Studio 18 2026" -A x64 -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ====== Build (%VERSION%) ======
cmake --build "%BUILD_DIR%" --config Release --target %EXE_NAME%
if %ERRORLEVEL% neq 0 (
    echo Build failed
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ====== Run (%VERSION%) ======
"%BUILD_DIR%\bin\Release\%EXE_NAME%.exe" -i frieren-winter-1.bmp -u frieren-winter-1-blur-%VERSION%.bmp -r 16

pause
