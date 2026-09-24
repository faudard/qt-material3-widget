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
