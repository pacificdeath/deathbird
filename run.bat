@echo off
setlocal enabledelayedexpansion

set D="-g -DDEBUG"
set V="-DVERBOSE"
set PROGRAM=""
set DEBUG=""
set VERBOSE=""
set GDB=0

for %%x in (%*) do (
    if "%%x"=="-help" (
        goto :help
    ) else if "%%x"=="g" (
        set GDB=1
        set DEBUG=%D%
        echo GDB is enabled
    ) else if "%%x"=="d" (
        set DEBUG=%D%
        echo DEBUG is enabled
    ) else if "%%x"=="v" (
        set VERBOSE=%V%
        echo VERBOSE is enabled
    ) else (
        set PROGRAM="%%x"
    )
)

set DEBUG=%DEBUG:"=%
set VERBOSE=%VERBOSE:"=%

gcc ^
    !DEBUG! ^
    !VERBOSE! ^
    -o ./build/deathbird.exe ^
    ./src/main.c ^
    -O0 ^
    -Wall ^
    -std=c99 ^
    -I./raylib-5.0_win64_mingw-w64/include/ ^
    -L./raylib-5.0_win64_mingw-w64/lib/ ^
    -lraylib ^
    -lopengl32 ^
    -lgdi32 ^
    -lwinmm

if not %errorlevel% equ 0 (
    echo compilation of deathbird.exe failed
    goto :end
)

if %errorlevel% equ 0 (
    if !GDB!==1 (
        gdb .\build\deathbird.exe
    ) else (
        .\build\deathbird.exe !PROGRAM!
    )
)
goto :end

:help
    echo %0 ^[Options^] ^<Program File^>
    echo Options:
    echo    -help           this thing
    echo    -debug          enable debug
    echo    -gdb            run gdb after compiliation
goto :end

:end