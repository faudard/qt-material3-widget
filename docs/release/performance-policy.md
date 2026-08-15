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

The shared cache helper is `QtMaterial::QtMaterialPaintCache`.

Benchmark targets should cover:

- theme switch fanout
- spec resolution
- shadow cache hit/miss
- button paint
- text field paint
- progress paint
- list/table delegate paint
