#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repo_root"

: "${CMAKE_BUILD_PARALLEL_LEVEL:=2}"
: "${QTM3_GENERATOR:=Ninja}"

if [[ "${OSTYPE:-}" == linux* ]]; then
  export QT_QPA_PLATFORM="${QT_QPA_PLATFORM:-offscreen}"
fi

build_dir="build/install-export"
install_dir="$repo_root/build/install-root"
consumer_dir="$repo_root/build/install-consumer"

cmake -S . -B "$build_dir" -G "$QTM3_GENERATOR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DQTMATERIAL3_BUILD_TESTS=OFF \
  -DQTMATERIAL3_BUILD_EXAMPLES=OFF \
  -DQTMATERIAL3_BUILD_BENCHMARKS=OFF \
  -DQTMATERIAL3_USE_MCU=OFF

cmake --build "$build_dir" --config Release
cmake --install "$build_dir" --config Release --prefix "$install_dir"

if [[ ! -f examples/install-consumer/CMakeLists.txt ]]; then
  echo "error: examples/install-consumer/CMakeLists.txt is missing" >&2
  exit 1
fi

cmake -S examples/install-consumer -B "$consumer_dir" -G "$QTM3_GENERATOR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$install_dir"

cmake --build "$consumer_dir" --config Release

echo "==> Install/export consumer validation completed."
