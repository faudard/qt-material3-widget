# Visual golden snapshots

This directory contains QtMaterial3 self-regression snapshots.

Three categories are maintained:

1. deterministic token-board goldens;
2. reviewed component-grid goldens rendered with real Qt widgets;
3. reviewable family state-matrix and focus candidates.

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
- `navigation_split_matrix_*.png`
- `navigation_focus_{tabs,rail,menu,breadcrumb,palette,split}_*.png`
- `input_slider_matrix_*.png`
- `desktop_data_matrix_*.png`
- `surface_bar_matrix_*.png`
- `surface_overlay_matrix_*.png`
- `layout_matrix_*.png`
- `progress_compact_matrix_*.png`
- `data_extended_matrix_*.png`
- `missing_material3_matrix_*.png`

Download the `family-visual-candidate-goldens` workflow artifact, review the images, then
commit the approved PNGs into this directory. Candidate cases tolerate a missing baseline,
but once the file exists they use the same strict zero-pixel comparison as established
goldens. Add only reviewed, release-critical files to `tools/release_rules.json`.

## Navigation/Desktop evidence

The dedicated `navigation-desktop-visual-evidence` artifact captures 27 navigation and
SplitView cases twice in independent processes on Qt 6.4.0 / Fusion / xcb at scale 1 and
font DPI 96. Each pass contains `goldens/` and `artifacts/manifest.json`; the artifact also
includes strict comparison output and `navigation-repeatability.json`.

`tools/check_navigation_visual_repeatability.py` rejects missing cases, renderer drift,
different commits, changed pixels or dimensions. A repeatable result keeps `visualReview`
at `pending`. Review approved images from the first pass, copy them here, and add the
release-critical paths to the release rules. Focus fixtures use keyboard focus and a
nonblinking caret; repeatability alone does not certify appearance or OS accessibility.

CI redirects candidates with `QTMATERIAL3_VISUAL_GOLDENS_DIR` rather than writing into
this directory. Reviewed references are compared before candidate generation.

## 1.7 Missing Material 3 evidence

The dedicated `missing-material3-visual-evidence` artifact renders
`missing_material3_matrix_{light_standard,dark_standard,light_high}.png` twice in independent
processes on Qt 6.4.0 / Fusion / xcb at scale 1 and font DPI 96.
`tools/check_missing_material3_visual_repeatability.py` rejects renderer drift, missing cases,
different commits, dimensions or pixels. Repeatability is only determinism evidence: the ledger
in `docs/components/material3-catalogue-certification-1.7.json` must still record a human visual
review before these PNGs are copied here and promoted.

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

## 1.5 enterprise certification

Family candidates are generated only by the pinned Qt 6.4.0 / Fusion CI lane. A family
matrix can become a stable release golden only after the generated PNG has been visually
reviewed. Once promoted, its light-standard, dark-standard and light-high variants are
listed in `tools/release_rules.json` and the registry may raise that family's rendering
axis to 4/4.

The 1.5 gate intentionally rejects a `complete` release-scoped component when its mapped
family goldens are absent from the stable release rules.
