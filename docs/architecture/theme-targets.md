# Theme Target Architecture

Spec 006 makes the theme architecture a physical CMake boundary.

```text
Foundation
    |
    v
ThemeModel  <---------------- Specs
    |
    +----------> ThemeIO
    |               |
    +----------> ThemeRuntime
                    ^
                    |
              temporary ThemeIO edge
```

The intended final direction after Specs 007-009 is:

```text
Foundation -> ThemeModel -> Specs
                  ^
                  |
             ThemeIO

ThemeRuntime -> ThemeModel
```

## Targets

### `QtMaterial3::ThemeModel`

Owns value/model/compiler concerns:
- `Theme`, `ThemeOptions`, `ColorScheme`;
- typography, shape, elevation, motion, state-layer, density, icon and accessibility tokens;
- component token overrides;
- theme validation;
- seed/theme builder and color backend;
- optional Material Color Utilities integration.

It has no ThemeManager, QObject observation or serialization responsibility.

### `QtMaterial3::ThemeIO`

Owns:
- JSON theme serialization/deserialization;
- XML import/export adapter;
- textual/schema migration work in later specs.

Depends on `ThemeModel`.

### `QtMaterial3::ThemeRuntime`

Owns:
- ThemeContext / ThemeContextHost;
- ThemeManager;
- ThemeObserver;
- SystemTheme/platform observation.

In Spec 006 it still links ThemeIO because the pre-existing ThemeManager API owns
JSON import/export and ThemeContext fingerprints via ThemeSerializer. Specs 008/009
must remove this transitional dependency.

### `QtMaterial3::Theme`

Compatibility/meta CMake target only.

It is an `INTERFACE` target aggregating the three physical theme layers so existing
downstream CMake consumers do not break during the pre-1.0 migration.

New architecture-sensitive code should link the narrowest layer possible.

## Critical boundary

`QtMaterial3::Specs` links `QtMaterial3::ThemeModel`, never Theme, ThemeIO or ThemeRuntime.

That boundary is enforced by `tools/check_theme_target_decomposition.py`.
