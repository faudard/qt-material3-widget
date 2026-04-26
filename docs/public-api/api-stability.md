# API stability policy

The theming subsystem uses a staged stability policy until the first stable release.

## Frozen after the theming stabilization work

The following names are treated as stable within the current minor release line:

- public token group names;
- `ThemeReadMode` values;
- JSON schema `formatVersion` field;
- JSON schema v2 top-level object names;
- color backend status names;
- `ThemeManager` runtime notification semantics;
- component-local override key names documented in the public API.

## Allowed additive changes

The following changes are allowed without a schema-breaking migration:

- adding optional token fields;
- adding optional metadata fields;
- adding new component override groups;
- adding new examples;
- adding new non-required JSON fields;
- adding new tests or diagnostics.

## Migration-required changes

The following require a migration note and, when relevant, a schema version bump:

- renaming public token fields;
- removing public token fields;
- changing strict JSON validation behavior;
- changing golden seed outputs;
- changing backend-selection semantics;
- changing `ThemeManager` signal emission order;
- changing default high-contrast or accessibility thresholds.

## Backend stability

`QTMATERIAL3_USE_MCU` means "request the MCU backend." It does not guarantee that MCU is available in the build tree. The actual runtime/build result is represented by the compiled backend status.

A build that requests MCU but cannot compile the adapter must still produce deterministic fallback colors and expose that status through the backend diagnostic API.
