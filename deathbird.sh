#!/usr/bin/bash
set -e  # exit on any error
shopt -s expand_aliases

DEBUG=false
ATLAS_GEN=false
NO_PLAY=false

for arg in "$@"; do
    case $arg in
        --debug) DEBUG=true ;;
        --atlasgen) ATLAS_GEN=true ;;
        --noplay) NO_PLAY=true ;;
    esac
done

log() {
    local text="$1"
    local color="${2:-32}"  # default Green
    local ts
    ts=$(date '+%H:%M:%S')

    echo -e "\e[34m[$ts]\e[0m \e[35mDEATHBIRD-COMPUTER-PROGRAM\e[0m [\e[${color}m$text\e[0m]"
}

catch_errors() {
    local code=$?
    if [ $code -ne 0 ]; then
        log "Failed horribly ($code)" 31
        exit $code
    fi
}

PROGRAM_NAME="Deathbird"
OUTPUT_EXE="./build/deathbird.exe"
INPUT_C="./src/main.c"

CLANG_ARGS=()
if $DEBUG; then
    CLANG_ARGS+=("-g" "-fsanitize=address" "-DDEBUG" "-Wall" "-O0")
else
    CLANG_ARGS+=("-O2")
fi

CLANG_ARGS+=(
    "-o" "$OUTPUT_EXE"
    "$INPUT_C"
    "-std=c99"
    "-I./raylib/include/"
    "-L./raylib/lib/"
    "-l:libraylib-linux.a"
    "-lGL"
    "-lX11"
    "-lm"
    "-Wall"
    "-Wextra"
    "-Wconversion"
    "-Wfloat-equal"
    "-Wshadow"
    "-Werror"
    "-DPLATFORM_LINUX"
)

if $ATLAS_GEN; then
    log "Compiling sprite atlas generator"
    ATLAS_CLANG_ARGS=("${CLANG_ARGS[@]}" "-DATLAS_GEN")
    clang "${ATLAS_CLANG_ARGS[@]}"
    catch_errors
    log "Generating sprite atlas"
    "$OUTPUT_EXE"
    catch_errors
fi

log "Compiling $PROGRAM_NAME"
clang "${CLANG_ARGS[@]}"
catch_errors

if $DEBUG; then
    log "Debugging $PROGRAM_NAME"
    lldb "$OUTPUT_EXE"
    catch_errors
    exit
fi

if ! $NO_PLAY; then
    log "Executing $PROGRAM_NAME"
    "$OUTPUT_EXE"
    catch_errors
fi
