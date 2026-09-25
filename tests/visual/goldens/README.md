# Visual golden snapshots

This directory contains QtMaterial3 self-regression snapshots.

Two categories are maintained:

1. deterministic token-board goldens;
2. component-grid artifacts rendered with real Qt widgets.

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
