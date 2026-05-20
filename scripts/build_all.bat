@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
if %ERRORLEVEL% neq 0 (
    echo vcvarsall failed
    pause
    exit /b %ERRORLEVEL%
)

cd /d "%~dp0.."
if not exist build mkdir build
cd build

echo ====== CMake Config ======
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ====== CMake Build ======
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ====== Running ======
Release\blur_demo.exe

pause
