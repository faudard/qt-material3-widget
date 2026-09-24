# CI and release validation

The project uses one cross-platform CI pipeline plus focused static-quality and
release-readiness gates.

## Local validation

For repository-wide static checks:

```bash
python tools/repo_health.py --strict
python tools/check_release.py --root . --scope all
```

For the theming subsystem:

```bash
python scripts/validate_theming_subsystem.py
```

That validation covers both `QTMATERIAL3_USE_MCU=OFF` and an MCU-requested
configuration. If MCU sources are unavailable, the build must still compile and
report the deterministic fallback backend.

## Installed consumer validation

Use the consumer matrix:

```bash
python scripts/ci/run-consumer-matrix.py --scenario install-shared
```

The matrix is the single consumer test harness for source, `add_subdirectory`,
FetchContent and installed-package scenarios.

## GitHub Actions

- **Quality** runs Python tooling tests, strict repository health and docs.
- **CI** owns the Qt 5/Qt 6 platform matrix, unit tests, consumers, sanitizers,
  examples/benchmarks and package construction.
- **release-readiness** runs the release contract plus release build/install
  validation on tags, schedule or manual dispatch.

## Visual regression

The maintained visual regression harness is
`tst_theme_visual_regression`. It protects token-board and component-grid
rendering against repository regressions. Pixel comparison is opt-in because
native fonts and rasterization vary by platform.

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
