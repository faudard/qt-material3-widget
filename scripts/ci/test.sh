#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build}"
BUILD_CONFIG="${BUILD_CONFIG:-Release}"
CTEST_OUTPUT_ON_FAILURE="${CTEST_OUTPUT_ON_FAILURE:-1}"
CTEST_PARALLEL_LEVEL="${CTEST_PARALLEL_LEVEL:-2}"

echo "Test directory  : ${BUILD_DIR}"
echo "Build config    : ${BUILD_CONFIG}"
echo "CTest parallel  : ${CTEST_PARALLEL_LEVEL}"

if [ ! -d "${BUILD_DIR}" ]; then
  echo "ERROR: build directory '${BUILD_DIR}' does not exist."
  echo "Run configure and build steps first."
  exit 1
fi

export QT_QPA_PLATFORM="${QT_QPA_PLATFORM:-offscreen}"

CTEST_ARGS=(
  --test-dir "${BUILD_DIR}"
  --build-config "${BUILD_CONFIG}"
  --parallel "${CTEST_PARALLEL_LEVEL}"
)

if [ "${CTEST_OUTPUT_ON_FAILURE}" = "1" ]; then
  CTEST_ARGS+=(--output-on-failure)
fi

if command -v ctest >/dev/null 2>&1; then
  :
else
  echo "ERROR: ctest not found in PATH."
  exit 1
fi

if [ "$(uname -s)" = "Linux" ] && command -v xvfb-run >/dev/null 2>&1; then
  echo "Running tests with xvfb-run..."
  xvfb-run -a ctest "${CTEST_ARGS[@]}"
else
  echo "Running tests without xvfb-run..."
  ctest "${CTEST_ARGS[@]}"
fi