$ErrorActionPreference = "Stop"

$RepoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $RepoRoot

if (-not $env:CMAKE_BUILD_PARALLEL_LEVEL) {
    $env:CMAKE_BUILD_PARALLEL_LEVEL = "2"
}

$Generator = if ($env:QTM3_GENERATOR) { $env:QTM3_GENERATOR } else { "Ninja" }

$BuildDir = "build/install-export"
$InstallDir = Join-Path $RepoRoot "build/install-root"
$ConsumerDir = Join-Path $RepoRoot "build/install-consumer"

cmake -S . -B $BuildDir -G $Generator `
    -DCMAKE_BUILD_TYPE=Release `
    -DQTMATERIAL3_BUILD_TESTS=OFF `
    -DQTMATERIAL3_BUILD_EXAMPLES=OFF `
    -DQTMATERIAL3_BUILD_BENCHMARKS=OFF `
    -DQTMATERIAL3_USE_MCU=OFF

cmake --build $BuildDir --config Release
cmake --install $BuildDir --config Release --prefix $InstallDir

cmake -S examples/install-consumer -B $ConsumerDir -G $Generator `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_PREFIX_PATH=$InstallDir

cmake --build $ConsumerDir --config Release

Write-Host "==> Install/export consumer validation completed."
