param (
    [switch]$Debug,
    [switch]$AtlasGen
)

function Log {
    param (
        [string]$Text,
        [string]$Color = "Green"
    )

    Write-Host "##########################" -NoNewLine -ForegroundColor Magenta
    Write-Host " [$(Get-Date -Format 'HH:mm:ss')]" -ForegroundColor Blue
    Write-Host "DEATHBIRD-COMPUTER-PROGRAM " -NoNewLine -ForegroundColor Magenta
    Write-Host "[$Text]" -ForegroundColor $Color
    Write-Host "##########################" -ForegroundColor Magenta
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
if ($Debug) {
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

if ($Debug) {
    Log "Debugging $ProgramName"
    & gdb $OutputExe --command "gdb-init.txt"
    CatchErrors
    exit
}

Log "Executing $ProgramName"
& $OutputExe
CatchErrors

