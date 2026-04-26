#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${1:-build}"

cmake -S . -B "${BUILD_DIR}" \
  -DQTMATERIAL3_BUILD_TESTS=ON \
  -DQTMATERIAL3_BUILD_EXAMPLES=ON \
  -DQTMATERIAL3_BUILD_VISUAL_REGRESSION_TESTS=ON

cmake --build "${BUILD_DIR}"

QTMATERIAL3_UPDATE_VISUAL_GOLDENS=1 \
ctest --test-dir "${BUILD_DIR}" --output-on-failure -R tst_theme_visual_regression
