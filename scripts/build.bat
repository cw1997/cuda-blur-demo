@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d "%~dp0.."
if not exist build mkdir build
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
cmake --build . --config Release
cd ..
