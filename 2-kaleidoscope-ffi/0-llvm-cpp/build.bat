@echo off
cmake -S. -B./build | exit /b
cmake --build ./build

:: For release builds, use cmake --build ./build --config Release
:: Before doing so, make sure the release mode 'llvm-config' is on the PATH, not the debug mode one.
:: Also, delete the folder and run cmake -S. -B./build first, to get rid of links to the debug mode stuff.
