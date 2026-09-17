# Install SDL3 dependencies from source on Windows (MinGW/MSYS2).
# Run once before building the game. Requires git and cmake in PATH.
#
# Usage: pwsh -ExecutionPolicy Bypass -File install_dependencies_windows.ps1

param(
    [string]$InstallPrefix = "$PSScriptRoot\deps"
)

$ErrorActionPreference = "Stop"

Write-Host "---------------------------------"
Write-Host "---                           ---"
Write-Host "---  Installing dependencies  ---"
Write-Host "---                           ---"
Write-Host "---------------------------------"

$BuildDir = Join-Path $env:TEMP "d2tm-build-deps"
if (Test-Path $BuildDir) { Remove-Item -Recurse -Force $BuildDir }
New-Item -ItemType Directory -Path $BuildDir | Out-Null

$CMakeFlags = @(
    "-DCMAKE_BUILD_TYPE=Release",
    "-DCMAKE_INSTALL_PREFIX=$InstallPrefix",
    "-G", "MinGW Makefiles",
    "-DCMAKE_PREFIX_PATH=$InstallPrefix"
)

function Build-Project {
    param(
        [string]$Name,
        [string]$Url,
        [string]$Tag,
        [string[]]$ExtraFlags = @()
    )

    Write-Host ""
    Write-Host "=== $Name ===" -ForegroundColor Cyan
    $SrcDir = Join-Path $BuildDir $Name
    $BldDir = Join-Path $SrcDir "build"

    git clone --depth 1 --branch $Tag $Url $SrcDir
    if ($LASTEXITCODE -ne 0) { throw "git clone failed for $Name" }

    $allFlags = $CMakeFlags + $ExtraFlags
    cmake -S $SrcDir -B $BldDir @allFlags
    if ($LASTEXITCODE -ne 0) { throw "cmake configure failed for $Name" }

    cmake --build $BldDir --target install -- -j4
    if ($LASTEXITCODE -ne 0) { throw "cmake build failed for $Name" }
}

# SDL3
Build-Project "SDL3" "https://github.com/libsdl-org/SDL.git" "release-3.4.10" @(
    "-DSDL_TEST_LIBRARY=OFF"
)

# SDL3_image
Build-Project "SDL3_image" "https://github.com/libsdl-org/SDL_image.git" "release-3.4.4" @(
    "-DSDLIMAGE_SAMPLES=OFF"
)

# SDL3_ttf
Build-Project "SDL3_ttf" "https://github.com/libsdl-org/SDL_ttf.git" "release-3.2.2" @(
    "-DSDLTTF_SAMPLES=OFF"
)

# SDL3_mixer with built-in Timidity MIDI synthesizer (no soundfont files needed)
Build-Project "SDL3_mixer" "https://github.com/libsdl-org/SDL_mixer.git" "release-3.2.4" @(
    "-DSDLMIXER_SAMPLES=OFF",
    "-DSDLMIXER_MIDI_TIMIDITY=ON",
    "-DSDLMIXER_MIDI_FLUIDSYNTH=OFF"
)

# Copy built DLLs into resources/dll so create_release.bat picks them up
$DllSource = Join-Path $InstallPrefix "bin"
$DllDest   = Join-Path $PSScriptRoot "resources\dll"
Write-Host ""
Write-Host "Copying DLLs to $DllDest ..." -ForegroundColor Yellow
foreach ($dll in @("SDL3.dll", "SDL3_image.dll", "SDL3_ttf.dll", "SDL3_mixer.dll")) {
    $src = Join-Path $DllSource $dll
    if (Test-Path $src) {
        Copy-Item $src $DllDest -Force
        Write-Host "  Copied $dll"
    } else {
        Write-Warning "  $dll not found in $DllSource"
    }
}

Write-Host ""
Write-Host "Done. Now run rebuildwin.bat to build the game." -ForegroundColor Green
