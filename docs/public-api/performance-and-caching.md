# Performance and caching

The theming subsystem should be deterministic and fast enough to use during application startup, live theme editing, and runtime light/dark transitions.

This page documents the maintained performance and caching contracts.

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
python3 scripts/run_performance_baseline.py
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
QTMATERIAL3_SPEC_RESOLUTION_BUDGET_MS=3000
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

2. **Spec resolution**
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

1. Run `python3 scripts/run_performance_baseline.py` before a theming change.
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

## 1.14 Performance / Scale

The scale suite extends the theming micro-benchmarks with production-size workloads:

- Table and Tree View backed by 100,000-row/items virtual models,
- creation of 1,000 Material widgets,
- global theme propagation across 1,000 live widgets,
- a 50,000-command Command Palette provider snapshot plus fuzzy query refresh,
- 1,000 Expressive transition controllers with repeated retarget/finish cycles,
- repeated create/destroy cycles to catch retained-memory regressions.

The heavy scale contracts are opt-in so ordinary functional CI remains deterministic:

```bash
python3 scripts/run_performance_baseline.py build-perf --scale
```

To enforce the CPU and RSS budgets:

```bash
python3 scripts/run_performance_baseline.py build-perf --scale --enforce
```

The canonical scale budget table is stored in
`docs/performance/scale-performance-budgets.json`. Every scenario reports wall time,
process CPU time, and resident-memory growth. CPU and RSS are the enforced metrics; wall
time remains diagnostic because hosted-runner scheduling noise can be significant.

Pull requests collect report-only evidence. Scheduled and manually dispatched Linux runs
enable strict budget enforcement on the same build directory after the report pass.


## 1.21 Desktop Scale 2.0

Desktop Scale 2.0 is the opt-in application-scale extension of the 1.14 gate. It keeps
the existing 100k/1k/50k contracts intact and adds heavier scenarios instead of changing
their meaning.

Run the complete 1.14 + 1.21 baseline locally:

```bash
python3 scripts/run_performance_baseline.py build-perf --desktop-scale-2
```

`--desktop-scale-2` implies `--scale`. To enforce the checked-in CPU, RSS, p95 and p99
budgets locally:

```bash
python3 scripts/run_performance_baseline.py build-perf --desktop-scale-2 --enforce
```

The 1.21 suite covers:

- 1,000,000-row Table/Tree navigation,
- lazy `fetchMore()` growth to 1,000,000 rows,
- million-row `QSortFilterProxyModel` filtering,
- 100,000 Command Palette results,
- 2,000 live theme changes,
- 5,000 adaptive resize transitions,
- 1,000 Dialog/Side Sheet lifecycles,
- 5,000 shadow/pixmap cache hits,
- warmed long-run lifecycle cycles for memory-growth detection.

Results are written to
`build-perf/performance-results/desktop-scale-2.json`. The CI history helper merges that
snapshot into a bounded `performance-history.json` ledger and emits a Markdown comparison
against the previous canonical `main` result.

Scheduled/manual Linux runs also execute:

```bash
python3 scripts/profile_desktop_scale_allocations.py \
  build-perf \
  --output-dir build-perf/performance-results/allocation-profile
```

That harness uses Valgrind Massif on the long-run lifecycle scenario. Massif remains
diagnostic evidence rather than a cross-platform hard gate; CPU, RSS, p95 and p99 remain
the deterministic budget contract.
