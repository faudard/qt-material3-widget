#!/usr/bin/env bash
set -euo pipefail

build_dir="${1:-build-perf}"

cmake -S . -B "$build_dir" \
  -DQTMATERIAL3_BUILD_TESTS=ON \
  -DQTMATERIAL3_BUILD_EXAMPLES=OFF \
  -DQTMATERIAL3_BUILD_BENCHMARKS=ON \
  -DQTMATERIAL3_USE_MCU="${QTMATERIAL3_USE_MCU:-OFF}"

cmake --build "$build_dir"

mkdir -p "$build_dir/performance-results"

ctest --test-dir "$build_dir" --output-on-failure -R 'performance|theme_performance'

for exe in \
  benchmark_theme_pipeline \
  benchmark_theme_manager_fanout \
  benchmark_spec_factory_pipeline \
  benchmark_shadow_cache_pipeline \
  benchmark_theme_switch \
  benchmark_ripple \
  benchmark_widget_creation; do
  path="$(find "$build_dir" -type f -perm -111 -name "$exe" 2>/dev/null | head -n 1 || true)"
  if [[ -n "$path" ]]; then
    echo "==> $exe"
    "$path" -o "$build_dir/performance-results/$exe.txt",txt || true
    cat "$build_dir/performance-results/$exe.txt" || true
  else
    echo "warning: benchmark executable not found: $exe" >&2
  fi
done

echo "Performance results written to $build_dir/performance-results"
