# CI and release validation

The theming subsystem is only considered merge-ready when it passes the same checks used by release validation.

## Local validation

Run the focused theming validation script:

```bash
scripts/validate-theming-subsystem.sh
```

This performs two builds:

1. `QTMATERIAL3_USE_MCU=OFF`
2. `QTMATERIAL3_USE_MCU=ON`

The MCU-requested build must pass even when Material Color Utilities is not vendored. In that case the backend status should report the documented fallback path.

## Install/export validation

Run:

```bash
scripts/validate-install-consumer.sh
```

This verifies that:

- the project builds;
- `cmake --install` exports headers, libraries, and package config files;
- a separate consumer can use `find_package(QtMaterial3Widgets REQUIRED)`;
- the exported imported targets link successfully.

## GitHub Actions

The repository provides three workflows:

- `CI`: regular library builds and tests on Linux, Windows, and macOS;
- `Examples`: example application build validation;
- `Install Consumer`: package install/export and downstream consumer validation.

## Visual regression gates

Visual regression comparison remains opt-in. The default CI path should generate artifacts and run screenshot smoke tests without failing because goldens are absent or platform-rendered pixels differ.

Use strict visual comparison only in a controlled environment:

```bash
QTMATERIAL3_VISUAL_STRICT=1 ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Use update mode only when intentionally refreshing visual goldens:

```bash
QTMATERIAL3_UPDATE_VISUAL_GOLDENS=1 ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```
