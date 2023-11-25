@echo off
setlocal

set FOLDER=Debug
set EXE=river

if "%1" == "release" (
    set FOLDER=Release
    shift
)

if "%1" == "tokens" (
    set "EXE=test-tokens"
    shift
)

:: Apparently "shift" has no effect on %*
:: Is there a neater way of passing arguments down?
.\build\%FOLDER%\%EXE%.exe %1 %2 %3 %4 %5 %6 %7 %8 %9
