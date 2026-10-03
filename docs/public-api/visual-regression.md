# Visual regression testing

QtMaterial3 keeps one visual self-regression harness for resolved theme tokens
and representative component rendering.

## Coverage

The harness exercises:

- light and dark themes;
- standard and high contrast;
- seed-color changes;
- focus/accessibility token changes;
- representative component rendering;
- Selection family state matrices across default, selected, disabled and RTL states;
- Input field state matrices across empty, value, disabled and RTL states;
- composite Input matrices for Search View, Date Picker and Date Range Picker;
- Navigation matrices for Tabs, Navigation Rail, Menu, Breadcrumb and Command Palette;
- Slider/Range Slider matrices covering horizontal, vertical, disabled and RTL states;
- Desktop-data matrices for Table, Tree View, Pagination and Split View;
- Theme Studio screenshot output.

Token-board snapshots use deterministic painting and are the safest regression
baseline. Component-grid snapshots use real Qt widgets and therefore remain
opt-in for strict pixel comparison across platforms.

## Run

```bash
ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Strict comparison:

```bash
QTMATERIAL3_VISUAL_STRICT=1 \
ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Intentional golden update:

```bash
QTMATERIAL3_UPDATE_VISUAL_GOLDENS=1 \
ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Optional tolerance:

```bash
QTMATERIAL3_VISUAL_STRICT=1 QTMATERIAL3_VISUAL_MAX_DIFF=25 \
ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Artifacts are written to `visual-artifacts` next to the test executable unless
`QTMATERIAL3_VISUAL_ARTIFACTS_DIR` overrides the directory.

Golden changes should be reviewed when theme generation, component specs,
shape/elevation/typography defaults, or accessibility/focus behavior changes.

## Candidate family goldens

Selection, Input, Navigation and Desktop Productivity state matrices use a two-stage baseline workflow.

A matrix without a checked-in PNG is a **candidate golden**. Smoke rendering still runs on
every test job, while the Ubuntu/Fusion CI job regenerates the candidate PNGs and publishes
them in the `family-visual-candidate-goldens` artifact. Missing candidate baselines do not
break strict release validation.

After visual review, commit the approved PNG under `tests/visual/goldens/`. From that point,
`QTMATERIAL3_VISUAL_STRICT=1` automatically compares it pixel-for-pixel through the same
test case. Once a family baseline is considered release-critical, also add its path to
`base.stable_release.visual_goldens` in `tools/release_rules.json`.

The candidate set currently contains three controlled theme variants for each matrix:
light/standard contrast, dark/standard contrast, and light/high contrast. This intentionally
avoids a full Cartesian explosion while covering the highest-value theme/state combinations.
