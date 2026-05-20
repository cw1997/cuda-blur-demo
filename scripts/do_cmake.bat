@echo on
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
if errorlevel 1 exit /b 1
cd /d D:\cuda\cuda_blur_demo\build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1
cmake --build . --config Release
if errorlevel 1 exit /b 1
Release\blur_demo.exe
pause
