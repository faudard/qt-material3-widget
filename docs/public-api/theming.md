# Theming API

The theming API is the foundation of the library. It maps Material 3 concepts—color, typography, shape, elevation, motion, and state feedback—into a Qt-friendly runtime model.

## Main classes

### `QtMaterial::ThemeOptions`

A lightweight options object used to describe how a theme should be built.

Current fields in the public header:

- `sourceColor`
- `mode`
- `contrast`
- `expressive`
- `useMaterialColorUtilities`

Use this type as input to `ThemeBuilder` or as the configuration owned by `ThemeManager`.

### `QtMaterial::Theme`

The resolved theme object. It aggregates the token groups used by widgets:

- `ColorScheme`
- `TypographyScale`
- `ShapeScale`
- `ElevationScale`
- `MotionTokens`
- `StateLayer`

This is the object widgets ultimately consume when they resolve colors, radii, text styles, transitions, or state-layer feedback.

### `QtMaterial::ThemeBuilder`

Responsible for constructing a theme from options or from a seed color.

Public entry points currently exposed by the header:

- `build(const ThemeOptions& options)`
- `buildLightFromSeed(const QColor& seed)`
- `buildDarkFromSeed(const QColor& seed)`

This class is the correct starting point when you want deterministic theme generation.

### `QtMaterial::ThemeManager`

Central runtime access point for the active theme.

Public entry points currently exposed by the header:

- `instance()`
- `theme()`
- `options()`
- `setTheme(...)`
- `setThemeOptions(...)`
- `rebuildTheme()`
- `themeChanged(...)` signal

Use `ThemeManager` when the application needs a single active theme that propagates to widgets.

## Token containers

### `QtMaterial::ColorScheme`

Stores resolved colors by Material color role.

### `QtMaterial::TypographyScale`

Stores `TypographyStyle` values by type role.

### `QtMaterial::ShapeScale`

Stores corner radii by shape role.

### `QtMaterial::ElevationScale`

Stores elevation styling such as blur, offset, and tonal overlay opacity.

### `QtMaterial::MotionTokens`

Stores motion styles such as duration and easing for named motion tokens.

### `QtMaterial::StateLayer`

Stores state-layer color and opacity values for hover, focus, press, and drag feedback.

## Recommended usage pattern

```cpp
#include <qtmaterial/theme/qtmaterialthemebuilder.h>
#include <qtmaterial/theme/qtmaterialthememanager.h>

QtMaterial::ThemeOptions options;
options.sourceColor = QColor("#6750A4");
options.mode = QtMaterial::ThemeMode::Light;
options.contrast = QtMaterial::ContrastMode::DefaultContrast;
options.expressive = false;
options.useMaterialColorUtilities = true;

QtMaterial::Theme theme = QtMaterial::ThemeBuilder{}.build(options);
QtMaterial::ThemeManager::instance().setTheme(theme);
```

## Material 3 references

Use the official Material 3 docs to understand the semantics behind the Qt API:

- color overview: <https://m3.material.io/styles/color/overview>
- color roles: <https://m3.material.io/styles/color/roles>
- typography: <https://m3.material.io/styles/typography>
- type scale: <https://m3.material.io/styles/typography/type-scale-tokens>
- shape: <https://m3.material.io/styles/shape>
- motion: <https://m3.material.io/styles/motion/overview/how-it-works>

## Documentation improvement targets

The next useful improvement is to add Doxygen comments directly in the public headers for:

- each `ThemeOptions` field
- each token container’s role mapping behavior
- the ownership and lifecycle expectations around `ThemeManager`
- the difference between builder-time theme generation and runtime theme mutation
