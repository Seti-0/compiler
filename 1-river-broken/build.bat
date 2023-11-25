@echo off
setlocal

set FOLDER=Debug
set "ARG="

if "%1" == "release" (
    set FOLDER=Release
    set "ARG=--config Release"
)

set "PATH=C:\frep\llvm\build\%FOLDER%\bin;%PATH%"

cmake -S. -B./build
cmake --build ./build %ARG%