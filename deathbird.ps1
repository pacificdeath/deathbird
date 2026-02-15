param (
    [switch]$Debug,
    [switch]$AtlasGen,
    [switch]$Gcc
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

$Args = @()
if ($Debug) {
    $Args += @(
        "-g",
        "-DDEBUG",
        "-Wall",
        "-O0"
    )
} else {
    $Args += @(
        "-O2"
    )
}

$Args += @(
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
    $AtlasArgs = $Args + @("-DATLAS_GEN")
    & gcc @AtlasArgs
    CatchErrors
    Log "Generating sprite atlas"
    & $OutputExe
    CatchErrors
}

Log "Compiling $ProgramName"

$stopwatch = [System.Diagnostics.Stopwatch]::StartNew()

if ($Gcc) {
    & gcc @Args
} else {
    & clang @Args
}

$stopwatch.Stop()
Write-Host "Elapsed time: $($stopwatch.Elapsed.TotalSeconds) seconds"

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

