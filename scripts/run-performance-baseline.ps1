param(
  [string]$BuildDir = "build-perf"
)

$ErrorActionPreference = "Stop"

$UseMcu = $env:QTMATERIAL3_USE_MCU
if (-not $UseMcu) { $UseMcu = "OFF" }

cmake -S . -B $BuildDir `
  -DQTMATERIAL3_BUILD_TESTS=ON `
  -DQTMATERIAL3_BUILD_EXAMPLES=OFF `
  -DQTMATERIAL3_BUILD_BENCHMARKS=ON `
  -DQTMATERIAL3_USE_MCU=$UseMcu

cmake --build $BuildDir

$ResultsDir = Join-Path $BuildDir "performance-results"
New-Item -ItemType Directory -Force -Path $ResultsDir | Out-Null

ctest --test-dir $BuildDir --output-on-failure -R 'performance|theme_performance'

$Executables = @(
  "benchmark_theme_pipeline",
  "benchmark_theme_manager_fanout",
  "benchmark_spec_factory_pipeline",
  "benchmark_shadow_cache_pipeline",
  "benchmark_theme_switch",
  "benchmark_ripple",
  "benchmark_widget_creation"
)

foreach ($Exe in $Executables) {
  $Candidate = Get-ChildItem -Path $BuildDir -Recurse -File -ErrorAction SilentlyContinue |
    Where-Object { $_.BaseName -eq $Exe } |
    Select-Object -First 1

  if ($Candidate) {
    Write-Host "==> $Exe"
    $OutFile = Join-Path $ResultsDir "$Exe.txt"
    & $Candidate.FullName -o "$OutFile,txt"
    if (Test-Path $OutFile) { Get-Content $OutFile }
  } else {
    Write-Warning "benchmark executable not found: $Exe"
  }
}

Write-Host "Performance results written to $ResultsDir"
