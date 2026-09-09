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

The repository provides dedicated build, documentation and repository-quality
workflows. Static repository health is authoritative in strict mode:

```bash
python tools/repo_health.py --strict
```

The Quality workflow is itself audited and fails if that explicit `--strict`
argument is removed. The component-maturity workflow separately executes
`check_component_registry.py --check-generated --strict`, requiring all 32
components to have explicit metadata, evaluated maturity axes, evidence, gaps
and synchronized generated status files.

The build-oriented workflows include:

- `CI`: regular library builds and tests on Linux, Windows, and macOS;
- `Examples`: example application build validation;
- `Install Consumer`: package install/export and downstream consumer validation.

## Visual validation

The Material visual-contract metadata is a required repository-health check:

```bash
python tools/check_material_visual_contract.py --root .
```

It validates the exhaustive Button fixture, deterministic raster policy,
isolated target inventory and source structure of both the private actual-only
runner and read-only standard-library comparator without Qt. Phase 016.C1 is
source-complete but runtime-unverified; the contract remains `contract-only`,
all reference sets remain `pending`, and real pixel/SSIM scoring is not active.
See [Visual comparator](../conformance/visual-comparator.md).

The registry-wide Material report is also generated and checked in repository
health:

```bash
python tools/generate_material_conformance_report.py --root . --check
python tools/check_material_conformance_harness.py --root .
```

Its current root status is `incomplete`, not failed: 5 Button variants have
partial static evidence, visual execution is pending and 27 registry components
do not yet have an active Material model. The harness itself passes only when
those limitations remain explicit and the four source gates pass. See
[Material conformance report](../conformance/material-conformance-report.md).

The older Theme Studio/component-grid self-regression comparison remains
opt-in. It is not Material conformance and cannot satisfy the Material visual
contract. The default CI path may generate its artifacts and run screenshot
smoke tests without failing because those legacy goldens are absent or
platform-rendered pixels differ.

Use legacy strict comparison only in a controlled environment:

```bash
QTMATERIAL3_VISUAL_STRICT=1 ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Use update mode only when intentionally refreshing visual goldens:

```bash
QTMATERIAL3_UPDATE_VISUAL_GOLDENS=1 ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```
