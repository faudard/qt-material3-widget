# Theming example

The repository keeps one focused public theming workflow example instead of
several overlapping executables.

## Build

```bash
cmake -S . -B build \
  -DQTMATERIAL3_BUILD_EXAMPLES=ON \
  -DQTMATERIAL3_BUILD_THEMING_EXAMPLES=ON
cmake --build build --target qtmaterial3_theming_workflows
```

## `examples/theming-workflows`

The example exercises the main supported theming flows in one place:

- seed-based theme construction;
- fallback/MCU backend diagnostics;
- component-local token overrides;
- deterministic JSON serialization;
- strict JSON round-trip validation;
- runtime application through `ThemeManager`;
- revision changes when switching light/dark options.

Run it as a console program. An optional first argument writes the generated
theme snapshot to that path.

```bash
./build/examples/theming-workflows/qtmaterial3_theming_workflows theme.generated.json
```

When MCU sources are absent, an MCU-requested build is still expected to
compile and report the deterministic fallback backend.
