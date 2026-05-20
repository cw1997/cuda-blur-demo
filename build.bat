@echo off
chcp 65001 >nul

set BUILD_DIR=build
set EXE_NAME=blur_demo
set SOURCE_DIR=%~dp0
if "%SOURCE_DIR:~-1%"=="\" set SOURCE_DIR=%SOURCE_DIR:~0,-1%

echo ====== Configure ======
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
pushd "%BUILD_DIR%"
cmake "%SOURCE_DIR%" -G "Visual Studio 18 2026" -A x64 -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ====== Build ======
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed
    pause
    exit /b %ERRORLEVEL%
)
popd

echo.
echo ====== Run ======
"%BUILD_DIR%\Release\%EXE_NAME%.exe" -i frieren-winter-1.bmp -u frieren-winter-1-blur.bmp -r 16

pause
