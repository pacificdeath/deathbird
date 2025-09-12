param (
    [switch]$Debug,
    [switch]$Gdb,
    [switch]$AtlasGen
)

function Log {
    param (
        [string]$Text,
        [string]$Color = "Green"
    )

    Write-Host "DEATHBIRD-COMPUTER-PROGRAM " -NoNewLine -ForegroundColor Magenta
    Write-Host "`"$Text`"" -ForegroundColor $Color
}

function CatchErrors {
    if ($LASTEXITCODE -ne 0) {
        Log "Failed horribly ($LASTEXITCODE)" "Red"
        exit $LASTEXITCODE
    }
}

$ProgramName = "Deathbird"
$OutputExe = "./build/deathbird.exe"
$InputC = "./src/main.c"

$GccArgs = @()
if ($Debug -or $Gdb) {
    $GccArgs += @(
        "-g",
        "-DDEBUG",
        "-Wall",
        "-O0"
    )
} else {
    $GccArgs += @(
        "-O2"
    )
}

$GccArgs += @(
    "-o", $OutputExe,
    $InputC,
    "-std=c99",
    "-I./raylib/include/",
    "-L./raylib/lib/",
    "-lraylib",
    "-lopengl32",
    "-lgdi32",
    "-lwinmm"
)

if ($AtlasGen) {
    Log "Compiling sprite atlas generator"
    $AtlasGccArgs = $GccArgs + @("-DATLAS_GEN")
    & gcc @AtlasGccArgs
    CatchErrors
    Log "Generating sprite atlas"
    & $OutputExe
    CatchErrors
}

Log "Compiling $ProgramName"
& gcc @GccArgs
CatchErrors

if ($Gdb) {
    Log "Debugging $ProgramName"
    & gdb $OutputExe
    CatchErrors
    exit
}

Log "Executing $ProgramName"
& $OutputExe
CatchErrors

