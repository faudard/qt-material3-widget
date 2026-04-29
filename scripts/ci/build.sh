#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build}"
BUILD_CONFIG="${BUILD_CONFIG:-Release}"
BUILD_TARGET="${BUILD_TARGET:-}"

echo "Build directory : ${BUILD_DIR}"
echo "Build config    : ${BUILD_CONFIG}"

if [ ! -d "${BUILD_DIR}" ]; then
  echo "ERROR: build directory '${BUILD_DIR}' does not exist."
  echo "Run configure step first, for example:"
  echo "  cmake -S . -B ${BUILD_DIR} -G Ninja -DCMAKE_BUILD_TYPE=${BUILD_CONFIG}"
  exit 1
fi

if [ -n "${BUILD_TARGET}" ]; then
  echo "Building target : ${BUILD_TARGET}"
  cmake --build "${BUILD_DIR}" --config "${BUILD_CONFIG}" --target "${BUILD_TARGET}"
else
  cmake --build "${BUILD_DIR}" --config "${BUILD_CONFIG}"
fi