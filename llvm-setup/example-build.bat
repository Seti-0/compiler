@echo off
setlocal

if "%1" == "release" (
    set "FOLDER=Release"
    set "ARG=--config Release"
) else (
    set "FOLDER=Debug"
    set "ARG="
)
set "PATH=C:\frep\llvm\build\%FOLDER%\bin;%PATH%"

cmake -S. -B./build
cmake --build ./build %ARG%