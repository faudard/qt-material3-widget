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


## Desktop Scale 2.0 gate

The 1.21 gate extends the 1.14 scale evidence to sustained desktop-application workloads.

Required workloads:

- Table and Tree View over 1,000,000-row/item virtual models,
- lazy model growth to 1,000,000 rows through repeated `fetchMore()`,
- application-owned `QSortFilterProxyModel` filtering across 1,000,000 source rows,
- 100,000-command Command Palette provider snapshots and repeated fuzzy refreshes,
- thousands of global theme changes with live Material widgets,
- adaptive shell resize storms crossing every width breakpoint,
- repeated Dialog and Side Sheet construction/open/close/destruction,
- sustained shadow/pixmap cache hit traffic,
- post-warm-up long-run create/destroy cycles with retained-RSS observation.

Desktop Scale 2.0 records per-scenario process CPU, RSS growth, p95 and p99 operation
latency. Pull requests collect report-only evidence. Scheduled/manual Linux runs enforce
the checked-in budgets in `docs/performance/desktop-scale-2-budgets.json`.

A canonical history artifact stores the latest bounded per-commit results from `main`.
Pull requests compare against that history without mutating it. Scheduled/manual runs also
capture a Valgrind Massif heap profile for the long-run lifecycle scenario so allocation
growth can be investigated separately from RSS/tail-latency gates.
