@echo off
setlocal

if "%1" == "release" (
    .\build\Debug\river.exe
) else (
    .\build\Release\river.exe
)