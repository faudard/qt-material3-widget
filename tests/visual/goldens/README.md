# Visual golden snapshots

This directory contains QtMaterial3 self-regression snapshots.

Three categories are maintained:

1. deterministic token-board goldens;
2. reviewed component-grid goldens rendered with real Qt widgets;
3. reviewable Selection/Input family state-matrix candidates.

Generate or intentionally update goldens with:

```bash
QTMATERIAL3_UPDATE_VISUAL_GOLDENS=1 \
ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Run strict comparison with:

```bash
QTMATERIAL3_VISUAL_STRICT=1 \
ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Native font and rasterization differences can affect component-grid pixels
across platforms, so strict comparison should be used in a controlled
environment.

## Promoting family candidates

CI generates the following candidate families in the pinned Ubuntu/Fusion rendering path:

- `selection_matrix_*.png`
- `input_field_matrix_*.png`
- `input_composite_matrix_*.png`

Download the `family-visual-candidate-goldens` workflow artifact, review the images, then
commit the approved PNGs into this directory. Candidate cases tolerate a missing baseline,
but once the file exists they use the same strict zero-pixel comparison as established
goldens. Add only reviewed, release-critical files to `tools/release_rules.json`.
