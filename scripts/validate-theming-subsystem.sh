#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repo_root"

: "${CMAKE_BUILD_PARALLEL_LEVEL:=2}"
: "${QTM3_GENERATOR:=Ninja}"

if [[ "${OSTYPE:-}" == linux* ]]; then
  export QT_QPA_PLATFORM="${QT_QPA_PLATFORM:-offscreen}"
fi

run_build() {
  local name="$1"
  local use_mcu="$2"
  local build_dir="build/${name}"

  echo "==> Configuring ${name} (QTMATERIAL3_USE_MCU=${use_mcu})"
  cmake -S . -B "$build_dir" -G "$QTM3_GENERATOR" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DQTMATERIAL3_BUILD_TESTS=ON \
    -DQTMATERIAL3_BUILD_EXAMPLES=ON \
    -DQTMATERIAL3_BUILD_BENCHMARKS=OFF \
    -DQTMATERIAL3_BUILD_VISUAL_REGRESSION_TESTS=ON \
    -DQTMATERIAL3_USE_MCU="$use_mcu"

  echo "==> Building ${name}"
  cmake --build "$build_dir" --config Debug

  echo "==> Running focused theming tests for ${name}"
  local ctest_cmd=(ctest --test-dir "$build_dir" --build-config Debug --output-on-failure -R 'theme|serializer|seed|backend|componenttoken|runtime|systemtheme|accessibility|keyboard|visual|public')
  if command -v xvfb-run >/dev/null 2>&1 && [[ "${OSTYPE:-}" == linux* ]]; then
    xvfb-run -a "${ctest_cmd[@]}"
  else
    "${ctest_cmd[@]}"
  fi
}

run_build theming-fallback OFF
run_build theming-mcu-requested ON

echo "==> Theming subsystem validation completed."
