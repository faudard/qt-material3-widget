# Performance and caching

The theming subsystem should be deterministic and fast enough to use during application startup, live theme editing, and runtime light/dark transitions.

This page documents the performance contracts introduced by the performance slice.

## Measured paths

The benchmark suite covers these hot paths:

- seed theme construction
- light/dark seed matrix construction
- JSON serialization
- JSON deserialization
- component spec generation
- `ThemeManager` signal fanout
- shadow cache key generation
- shadow cache insert/find behavior

Run the full baseline locally:

```bash
scripts/run-performance-baseline.sh
```

On Windows:

```powershell
scripts/run-performance-baseline.ps1
```

## Build flags

```bash
cmake -S . -B build-perf \
  -DQTMATERIAL3_BUILD_TESTS=ON \
  -DQTMATERIAL3_BUILD_BENCHMARKS=ON \
  -DQTMATERIAL3_BUILD_EXAMPLES=OFF
cmake --build build-perf
```

## Performance guardrails

The test `tst_theme_performance_contracts` reports elapsed time for core hot loops. By default the budgets are reported only, because CI machines vary substantially.

Set this variable to turn them into hard assertions:

```bash
QTMATERIAL3_ENFORCE_PERF_BUDGETS=1 ctest --test-dir build-perf -R tst_theme_performance_contracts --output-on-failure
```

Supported budget overrides:

```bash
QTMATERIAL3_THEME_BUILD_BUDGET_MS=3000
QTMATERIAL3_JSON_ROUNDTRIP_BUDGET_MS=3000
QTMATERIAL3_SPEC_FACTORY_BUDGET_MS=3000
```

The default budget file is stored at:

```text
docs/performance/theme-performance-budgets.json
```

## Caching policy

Use caching only where it improves repeated runtime operations without hiding correctness bugs.

Recommended cache boundaries:

1. **ThemeBuilder base theme**
   - Keep immutable base token defaults cached.
   - Never cache mutable resolved themes by reference.

2. **SpecFactory**
   - Prefer pure value generation for simple specs.
   - Add cache only after measured evidence shows repeated generation cost.
   - Cache keys must include theme revision or a stable theme fingerprint plus density/component variant.

3. **Shadow rendering**
   - Cache rendered pixmaps through `QPixmapCache`.
   - Keys must include geometry, radius, blur, offset, color, and device-pixel-ratio if DPR-aware rendering is added.

4. **ThemeManager signal fanout**
   - Avoid emitting when the effective resolved theme did not change.
   - Keep notification count deterministic: one apply cycle per real theme change.

5. **JSON**
   - Keep export deterministic.
   - Omit volatile metadata such as timestamps from golden and performance snapshots unless explicitly requested.

## Regression workflow

1. Run `scripts/run-performance-baseline.sh` before a theming change.
2. Apply the change.
3. Run the script again.
4. Compare `build-perf/performance-results`.
5. Tighten or update budgets only when the change is intentional and documented.

## Release expectations

Before a release:

- benchmark results should be attached to the release PR,
- strict performance budgets may be enabled on at least one stable CI runner,
- visual regression and JSON golden updates must be separated from unrelated performance work,
- any cache-key change must include a regression test.
