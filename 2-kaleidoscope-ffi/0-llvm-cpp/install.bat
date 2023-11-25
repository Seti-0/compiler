@echo off

:: This basically just copies the LLVM_Wrapper .lib to the rust debug build dir.
:: The definition of what is copied, and where relative to the prefix given below, is specified in CMakeLists.txt
cmake --install build --prefix %CD%\..\1-kaleidoscope-rust\target\debug --config Debug
