# Performance policy

Release-quality widgets must avoid unnecessary work in paint and layout hot paths.

Rules:

1. Resolve specs before paint.
2. Do not perform ad hoc theme lookups inside tight paint loops.
3. Cache expensive paths, shadows, pixmaps and icon compositions.
4. Make caches device-pixel-ratio aware.
5. Avoid heap allocation in repeated paint operations where practical.
6. Avoid repainting geometry-stable widgets for non-visual state changes.
7. Provide benchmarks for hot paths.

Paint/shadow caches are internal implementation helpers and are benchmarked without being part of the installed API.

Benchmark targets should cover:

- theme switch fanout
- spec resolution
- shadow cache hit/miss
- button paint
- text field paint
- progress paint
- list/table delegate paint

## Scale gate

The 1.14 scale gate keeps performance evidence separate from normal correctness tests.

Required workloads:

- 100,000-row Table and Tree View virtualized model/view paths,
- 1,000 simultaneously live Material widgets,
- global theme propagation across those 1,000 widgets,
- at least 50,000 Command Palette commands with query/filter refresh,
- 1,000 Expressive motion controllers under repeated retargeting,
- repeated widget construction/destruction with retained-RSS observation.

Budgets are expressed as process CPU milliseconds and resident-memory growth (MiB).
Hosted pull-request runs are report-only; scheduled/manual Linux runs enforce the checked-in
budget table. A budget increase requires a rationale in the PR and must not be used to hide
an uninvestigated regression.
