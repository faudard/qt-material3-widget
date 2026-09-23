# Theme Target Architecture

The theme layer is split into three physical CMake targets. There is no compatibility umbrella target.

```text
Foundation -> ThemeModel <- Specs
                  ^
                  |
                ThemeIO

ThemeRuntime -> ThemeModel
```

## Targets

### `QtMaterial3::ThemeModel`

Owns value/model/compiler concerns:

- `Theme`, `ThemeOptions`, and `ColorScheme`;
- typography, shape, elevation, motion, state-layer, density, icon and accessibility tokens;
- component token overrides;
- theme validation;
- seed/theme builder and color backend;
- optional Material Color Utilities integration.

It has no ThemeManager, QObject observation or serialization responsibility.

### `QtMaterial3::ThemeIO`

Owns JSON theme serialization/deserialization, textual codecs and XML import/export.

It depends on `ThemeModel`.

### `QtMaterial3::ThemeRuntime`

Owns `ThemeContext`, `ThemeContextHost`, `ThemeManager`, `ThemeObserver` and
`SystemTheme`/platform observation.

It depends on `ThemeModel` and does not depend on `ThemeIO`.

## Consumer rule

Consumers link only the layers they use. A widget-only application normally gets the
theme model/runtime dependencies transitively through `QtMaterial3::Widgets`; applications
that directly construct themes or use runtime context APIs may link `ThemeModel` or
`ThemeRuntime` explicitly. Serialization users link `ThemeIO`.

## Critical boundary

`QtMaterial3::Specs` links `QtMaterial3::ThemeModel`, never `ThemeIO` or
`ThemeRuntime`.

That boundary is enforced through the unified Theme gate:

```bash
python3 tools/check_theme.py --root . --scope targets
```

Run all Theme contracts with `--scope all`.
