#!/usr/bin/bash
set -e  # exit on any error
shopt -s expand_aliases

# ----------------------------
# Parameters
# ----------------------------
DEBUG=false
ATLAS_GEN=false

for arg in "$@"; do
    case $arg in
        --debug) DEBUG=true ;;
        --atlasgen) ATLAS_GEN=true ;;
    esac
done

# ----------------------------
# Logging function
# ----------------------------
log() {
    local text="$1"
    local color="${2:-32}"  # default Green
    local ts
    ts=$(date '+%H:%M:%S')

    # Colors: 31=Red, 32=Green, 34=Blue, 35=Magenta
    echo -e "\e[35m##########################\e[0m"
    echo -e "\e[34m[$ts]\e[0m \e[35mDEATHBIRD-COMPUTER-PROGRAM\e[0m [\e[${color}m$text\e[0m]"
    echo -e "\e[35m##########################\e[0m"
}

# ----------------------------
# Error handling
# ----------------------------
catch_errors() {
    local code=$?
    if [ $code -ne 0 ]; then
        log "Failed horribly ($code)" 31
        exit $code
    fi
}

# ----------------------------
# Paths and program names
# ----------------------------
PROGRAM_NAME="Deathbird"
OUTPUT_EXE="./build/deathbird.exe"
INPUT_C="./src/main.c"

CLANG_ARGS=()
if $DEBUG; then
    CLANG_ARGS+=("-g" "-DDEBUG" "-Wall" "-O0")
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
    "-DPLATFORM_LINUX"
)

# ----------------------------
# Atlas generation
# ----------------------------
if $ATLAS_GEN; then
    log "Compiling sprite atlas generator"
    ATLAS_CLANG_ARGS=("${CLANG_ARGS[@]}" "-DATLAS_GEN")
    clang "${ATLAS_CLANG_ARGS[@]}"
    catch_errors
    log "Generating sprite atlas"
    "$OUTPUT_EXE"
    catch_errors
fi

# ----------------------------
# Compile main program
# ----------------------------
log "Compiling $PROGRAM_NAME"
clang "${CLANG_ARGS[@]}"
catch_errors

# ----------------------------
# Debug mode
# ----------------------------
if $DEBUG; then
    log "Debugging $PROGRAM_NAME"
    gdb "$OUTPUT_EXE" -x gdb-init.txt
    catch_errors
    exit
fi

# ----------------------------
# Execute program
# ----------------------------
log "Executing $PROGRAM_NAME"
"$OUTPUT_EXE"
catch_errors
