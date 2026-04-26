param(
    [string]$BuildDir = "build"
)

$ErrorActionPreference = "Stop"

cmake -S . -B $BuildDir `
  -DQTMATERIAL3_BUILD_TESTS=ON `
  -DQTMATERIAL3_BUILD_EXAMPLES=ON `
  -DQTMATERIAL3_BUILD_VISUAL_REGRESSION_TESTS=ON

cmake --build $BuildDir

$env:QTMATERIAL3_UPDATE_VISUAL_GOLDENS = "1"
ctest --test-dir $BuildDir --output-on-failure -R tst_theme_visual_regression
Remove-Item Env:\QTMATERIAL3_UPDATE_VISUAL_GOLDENS -ErrorAction SilentlyContinue
