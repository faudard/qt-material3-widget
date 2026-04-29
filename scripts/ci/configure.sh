#!/usr/bin/env bash
set -euo pipefail

SOURCE_DIR="${SOURCE_DIR:-.}"
BUILD_DIR="${BUILD_DIR:-build}"
BUILD_CONFIG="${BUILD_CONFIG:-Release}"
CMAKE_GENERATOR="${CMAKE_GENERATOR:-Ninja}"

QTMATERIAL3_BUILD_TESTS="${QTMATERIAL3_BUILD_TESTS:-ON}"
QTMATERIAL3_BUILD_EXAMPLES="${QTMATERIAL3_BUILD_EXAMPLES:-ON}"
QTMATERIAL3_BUILD_BENCHMARKS="${QTMATERIAL3_BUILD_BENCHMARKS:-OFF}"
QTMATERIAL3_USE_MCU="${QTMATERIAL3_USE_MCU:-OFF}"

echo "Source directory        : ${SOURCE_DIR}"
echo "Build directory         : ${BUILD_DIR}"
echo "Build config            : ${BUILD_CONFIG}"
echo "CMake generator         : ${CMAKE_GENERATOR}"
echo "Build tests             : ${QTMATERIAL3_BUILD_TESTS}"
echo "Build examples          : ${QTMATERIAL3_BUILD_EXAMPLES}"
echo "Build benchmarks        : ${QTMATERIAL3_BUILD_BENCHMARKS}"
echo "Use MCU                 : ${QTMATERIAL3_USE_MCU}"

if ! command -v cmake >/dev/null 2>&1; then
  echo "ERROR: cmake not found in PATH."
  exit 1
fi

if [ "${CMAKE_GENERATOR}" = "Ninja" ] && ! command -v ninja >/dev/null 2>&1; then
  echo "ERROR: ninja not found in PATH, but CMAKE_GENERATOR=Ninja."
  exit 1
fi

cmake -S "${SOURCE_DIR}" -B "${BUILD_DIR}" -G "${CMAKE_GENERATOR}" \
  -DCMAKE_BUILD_TYPE="${BUILD_CONFIG}" \
  -DQTMATERIAL3_BUILD_TESTS="${QTMATERIAL3_BUILD_TESTS}" \
  -DQTMATERIAL3_BUILD_EXAMPLES="${QTMATERIAL3_BUILD_EXAMPLES}" \
  -DQTMATERIAL3_BUILD_BENCHMARKS="${QTMATERIAL3_BUILD_BENCHMARKS}" \
  -DQTMATERIAL3_USE_MCU="${QTMATERIAL3_USE_MCU}"