@echo off
chcp 65001 >nul

echo ====== 编译 CUDABlurDemo ======
if not exist build mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo CMake 配置失败
    pause
    exit /b %ERRORLEVEL%
)
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo 编译失败
    pause
    exit /b %ERRORLEVEL%
)
cd ..

echo.
echo ====== 运行性能对比 ======
build\Release\blur_demo.exe

pause
