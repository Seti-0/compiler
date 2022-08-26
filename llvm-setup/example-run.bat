@echo off
setlocal

if "%1" == "release" (
    .\build\Release\river.exe
) else (
    .\build\Debug\river.exe
)