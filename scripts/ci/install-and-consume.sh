#!/usr/bin/env bash
set -euo pipefail

cmake --install build --prefix "$PWD/install"

cmake -S examples/install-consumer \
  -B consumer-build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$PWD/install"

cmake --build consumer-build --config Release