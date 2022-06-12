@echo off
cmake -S. -B./build | exit /b
cmake --build ./build