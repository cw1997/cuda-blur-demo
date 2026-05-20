@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
cd /d "%~dp0..\build"
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release 2>&1
exit /b %ERRORLEVEL%
