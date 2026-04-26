# Theming examples

This page maps the public theming workflows to runnable examples. The examples intentionally use the public theme API first and Qt widgets second, so they double as API smoke tests.

## Build

From the repository root:

```bash
cmake -S . -B build \
  -DQTMATERIAL3_BUILD_EXAMPLES=ON \
  -DQTMATERIAL3_BUILD_TESTS=ON
cmake --build build
```

To build the theming examples only when the parent examples tree is enabled:

```bash
cmake -S . -B build \
  -DQTMATERIAL3_BUILD_EXAMPLES=ON \
  -DQTMATERIAL3_BUILD_THEMING_EXAMPLES=ON
cmake --build build --target qtmaterial3_theming_seed_workflow
```

## Examples

### `examples/theming-seed-workflow`

Interactive seed-color workflow.

Covers:

- `ThemeOptions::sourceColor`
- light/dark generation
- contrast mode selection
- `ThemeManager::setThemeOptions`
- resolved JSON inspection

Run:

```bash
./build/examples/theming-seed-workflow/qtmaterial3_theming_seed_workflow
```

### `examples/theming-runtime-switch`

Small runtime-switching example that uses `ThemeManager` as the single runtime theme source.

Covers:

- runtime seed changes
- light/dark switching
- revision tracking
- `themeChangedWithReason`

Run:

```bash
./build/examples/theming-runtime-switch/qtmaterial3_theming_runtime_switch
```

### `examples/theming-json-workflow`

Console example for exporting a resolved theme and reading it back in strict mode.

Covers:

- schema v2 export
- strict read mode
- persisted resolved tokens
- deterministic JSON snapshots

Run:

```bash
./build/examples/theming-json-workflow/qtmaterial3_theming_json_workflow theme-v2.generated.json
```

### `examples/theming-component-overrides`

Console example for component-local override serialization.

Covers:

- family overrides such as `button`
- component overrides such as `button.filled`
- density/icon/custom override payloads
- JSON persistence of overrides

Run:

```bash
./build/examples/theming-component-overrides/qtmaterial3_theming_component_overrides
```

### `examples/theming-backend-report`

Console example for checking the effective backend when MCU is requested.

Covers:

- `QTMATERIAL3_USE_MCU`
- `QTMATERIAL3_HAS_MCU`
- fallback diagnostics
- backend selection status

Run:

```bash
./build/examples/theming-backend-report/qtmaterial3_theming_backend_report
```

## Release check

Before calling theming stable, all examples should build in both configurations:

```bash
cmake -S . -B build-fallback \
  -DQTMATERIAL3_BUILD_EXAMPLES=ON \
  -DQTMATERIAL3_BUILD_THEMING_EXAMPLES=ON \
  -DQTMATERIAL3_USE_MCU=OFF
cmake --build build-fallback

cmake -S . -B build-mcu \
  -DQTMATERIAL3_BUILD_EXAMPLES=ON \
  -DQTMATERIAL3_BUILD_THEMING_EXAMPLES=ON \
  -DQTMATERIAL3_USE_MCU=ON \
  -DQTMATERIAL3_MCU_ROOT=third_party/material-color-utilities
cmake --build build-mcu
```

If MCU sources are absent, the MCU build is still expected to compile and report the deterministic fallback backend.
