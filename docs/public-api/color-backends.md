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

`QTMATERIAL3_USE_MCU` is the build-time request to use a Material Color Utilities adapter.

This development slice separates two concepts:

- `QTMATERIAL3_USE_MCU`: the user requested the MCU backend in CMake;
- `QTMATERIAL3_HAS_MCU`: the library was actually compiled with an MCU adapter.

The current tree does not vendor or link an MCU adapter yet, so `QTMATERIAL3_HAS_MCU` remains false and the deterministic fallback backend is selected. This is intentional: it makes the fallback contract explicit and prevents a CMake option from silently implying an unavailable implementation.

When a real MCU adapter is added later, `QTMATERIAL3_HAS_MCU` should become true only when the adapter is linked and the seed-generation path calls that adapter.

## Runtime resolution

`ThemeBuilder::colorBackendStatus(options)` reports:

- requested backend;
- effective backend;
- whether MCU was requested;
- whether MCU was compiled in;
- whether fallback was used;
- a diagnostic string.

If `ThemeOptions::useMaterialColorUtilities` is true but `QTMATERIAL3_HAS_MCU` is false, the builder uses the fallback backend and reports that fallback was used.

## Contrast mapping for MCU

The public contrast modes map to Material Color Utilities contrast levels as follows:

| `ContrastMode` | MCU contrast level |
|---|---:|
| `Standard` | `0.0` |
| `Medium` | `0.5` |
| `High` | `1.0` |

The fallback backend uses its own deterministic post-generation adjustment pass but keeps the same public contrast-mode semantics.
