@echo off

:: This basically just copies the LLVM_Wrapper .lib to the rust debug build dir.
:: The definition of what is copied, and where relative to the prefix given below, is specified in CMakeLists.txt
cmake --install build --prefix %CD%\..\1-kaleidoscope-rust\target\debug --config Debug

:: zlib1 is needed but cmake doesn't pay attention to it apparently.
:: Note that if I've lost this file (again), it's from vcpkg. There should be another copy locally in Projects,
:: otherwise, it can be found on the vcpkg website.
copy %CD%\build\Debug\zlib1.dll %CD%\..\1-kaleidoscope-rust\target\debug\zlib1.dll