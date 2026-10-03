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
- `navigation_primary_matrix_*.png`
- `navigation_desktop_matrix_*.png`
- `input_slider_matrix_*.png`
- `desktop_data_matrix_*.png`
- `surface_bar_matrix_*.png`
- `progress_compact_matrix_*.png`
- `data_extended_matrix_*.png`

Download the `family-visual-candidate-goldens` workflow artifact, review the images, then
commit the approved PNGs into this directory. Candidate cases tolerate a missing baseline,
but once the file exists they use the same strict zero-pixel comparison as established
goldens. Add only reviewed, release-critical files to `tools/release_rules.json`.

## Desktop Productivity candidates

The same candidate workflow also covers:

- `input_slider_matrix_*.png`
- `desktop_data_matrix_*.png`

These candidates exercise Slider/Range Slider plus Table/Tree/Pagination/Split View across the
controlled light, dark and high-contrast theme set. Promote only reviewed PNGs to stable
release goldens.

## 1.5 enterprise rule

A family matrix is not completion evidence merely because CI can render it. `complete` requires
reviewed PNGs committed here and listed in `tools/release_rules.json`. Candidate-only matrices
therefore cap the rendering axis at 3/4. Families without a deterministic matrix remain below
that gate until one is added. This rule applies equally to Selection, Inputs, Navigation,
Surfaces, Data, Progress and Compact controls.
