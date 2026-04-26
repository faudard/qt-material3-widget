$ErrorActionPreference = "Stop"

$RepoRoot = Split-Path -Parent $PSScriptRoot
Set-Location $RepoRoot

if (-not $env:CMAKE_BUILD_PARALLEL_LEVEL) {
    $env:CMAKE_BUILD_PARALLEL_LEVEL = "2"
}

$Generator = if ($env:QTM3_GENERATOR) { $env:QTM3_GENERATOR } else { "Ninja" }

function Invoke-ThemingBuild {
    param(
        [string]$Name,
        [string]$UseMcu
    )

    $BuildDir = "build/$Name"

    Write-Host "==> Configuring $Name (QTMATERIAL3_USE_MCU=$UseMcu)"
    cmake -S . -B $BuildDir -G $Generator `
        -DCMAKE_BUILD_TYPE=Debug `
        -DQTMATERIAL3_BUILD_TESTS=ON `
        -DQTMATERIAL3_BUILD_EXAMPLES=ON `
        -DQTMATERIAL3_BUILD_BENCHMARKS=OFF `
        -DQTMATERIAL3_BUILD_VISUAL_REGRESSION_TESTS=ON `
        -DQTMATERIAL3_USE_MCU=$UseMcu

    Write-Host "==> Building $Name"
    cmake --build $BuildDir --config Debug

    Write-Host "==> Running focused theming tests for $Name"
    ctest --test-dir $BuildDir --build-config Debug --output-on-failure -R "theme|serializer|seed|backend|componenttoken|runtime|systemtheme|accessibility|keyboard|visual|public"
}

Invoke-ThemingBuild -Name "theming-fallback" -UseMcu "OFF"
Invoke-ThemingBuild -Name "theming-mcu-requested" -UseMcu "ON"

Write-Host "==> Theming subsystem validation completed."
