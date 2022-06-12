@echo off
cmake -S. -B./build | exit /b
cmake --build ./build | exit /b
.\build\Debug\Kaleidoscope.exe