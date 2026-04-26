# Visual golden snapshots

This directory stores PNG visual regression goldens for the Theme Studio and themed component grid.

The visual harness has two levels:

1. **Token-board goldens** are deterministic painter-generated snapshots of resolved theme tokens. These are suitable for normal regression gates.
2. **Component-grid goldens** render actual widgets. They are opt-in because native font, rasterization, and platform style differences can produce small pixel differences across operating systems.

Generate or update goldens:

```bash
QTMATERIAL3_UPDATE_VISUAL_GOLDENS=1 \
ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Run strict pixel comparison:

```bash
QTMATERIAL3_VISUAL_STRICT=1 \
ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Optional tolerance:

```bash
QTMATERIAL3_VISUAL_STRICT=1 QTMATERIAL3_VISUAL_MAX_DIFF=25 \
ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Artifacts and diffs are written to `visual-artifacts` next to the test binary unless `QTMATERIAL3_VISUAL_ARTIFACTS_DIR` is set.
