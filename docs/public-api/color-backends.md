# Theme color backends

The theme layer has an explicit backend decision boundary so seed-color generation can be tested and documented independently of widget rendering.

## Backends

### Deterministic fallback backend

The fallback backend is always compiled. It is the portability baseline for the library and is used whenever Material Color Utilities is unavailable or not requested.

Fallback behavior is intentionally deterministic:

- the same `ThemeOptions` must produce the same resolved theme;
- light and dark generation must resolve the same required color-role set;
- contrast modes must produce stable role adjustments;
- regression snapshots for known seed colors must remain stable unless a deliberate token/schema change is made.

### Material Color Utilities backend

`QTMATERIAL3_USE_MCU` is the build-time request to use the Material Color Utilities adapter.

`QTMATERIAL3_HAS_MCU` means the library was actually compiled with a valid MCU C++ checkout and the adapter is linked into `qtmaterial3_theme`.

By default the build looks for MCU here:

```text
third_party/material-color-utilities
```

A different checkout can be provided with:

```bash
cmake -S . -B build \
  -DQTMATERIAL3_USE_MCU=ON \
  -DQTMATERIAL3_MCU_ROOT=/absolute/path/to/material-color-utilities
```

When MCU is found, the theme target compiles the upstream C++ sources, defines `QTMATERIAL3_HAS_MCU=1`, and `ThemeOptions::useMaterialColorUtilities=true` routes seed generation through the MCU adapter.

When MCU is requested but not found, `QTMATERIAL3_HAS_MCU=0`; the builder reports fallback through `ThemeBuilder::colorBackendStatus(options)` and uses the deterministic fallback backend.

## Runtime resolution

`ThemeBuilder::colorBackendStatus(options)` reports:

- requested backend;
- effective backend;
- whether MCU was requested;
- whether MCU was compiled in;
- whether fallback was used;
- a diagnostic string.

If `ThemeOptions::useMaterialColorUtilities` is true but `QTMATERIAL3_HAS_MCU` is false, the builder uses the fallback backend and reports that fallback was used.

## MCU scheme mapping

The adapter maps non-expressive themes to MCU `SchemeTonalSpot` and expressive themes to MCU `SchemeExpressive`.

The public contrast modes map to Material Color Utilities contrast levels as follows:

| `ContrastMode` | MCU contrast level |
|---|---:|
| `Standard` | `0.0` |
| `Medium` | `0.5` |
| `High` | `1.0` |

The fallback backend uses its own deterministic post-generation adjustment pass but keeps the same public contrast-mode semantics.

## Golden snapshots

MCU snapshots live in:

```text
tests/theme/goldens/mcu
```

Generate or refresh them from an MCU-enabled build with:

```bash
QTMATERIAL3_UPDATE_GOLDENS=1 ctest --test-dir build --output-on-failure -R tst_theme_mcu_goldens
```

Golden changes should be reviewed as API-affecting changes because they represent resolved theme output.
