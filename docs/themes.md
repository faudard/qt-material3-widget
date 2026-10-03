# Themes

The consumer theming API separates construction, runtime ownership and persistence.

## Dynamic colors

Build a complete theme from a seed:

```cpp
QtMaterial::ThemeOptions options;
options.sourceColor = QColor("#6750A4");
options.mode = QtMaterial::ThemeMode::Light;
options.preference = QtMaterial::ThemePreference::Light;
options.contrast = QtMaterial::ContrastMode::Standard;
options.variant = QtMaterial::ThemeVariant::TonalSpot;
options.backendPolicy = QtMaterial::ColorBackendPolicy::Auto;

const auto theme = QtMaterial::ThemeBuilder{}.build(options);
QtMaterial::ThemeManager::instance().setTheme(theme);
```

For the shortest path, use `buildLightFromSeed()` or `buildDarkFromSeed()`.

## Dark mode

Dark mode is a resolved `ThemeMode`. A user preference may instead be `ThemePreference::FollowSystem`; resolve that preference before building the final theme. For OS integration, see [System theme integration](public-api/system-theme.md).

## Change colors at runtime

```cpp
auto &manager = QtMaterial::ThemeManager::instance();
manager.applySeedColor(QColor("#00639B"));
```

Existing Material widgets receive theme-change propagation through the runtime theme infrastructure.

## Dynamic-color backend

`ColorBackendPolicy` supports `Auto`, `PreferMaterialColorUtilities`, `ForceMaterialColorUtilities` and `ForceFallback`. Material Color Utilities is optional; the fallback backend remains available when MCU is not built.

See [Color backends](public-api/color-backends.md) for the detailed contract.

## Component overrides

Use typed component-local overrides instead of widget-specific ad hoc palettes. Overrides live in the resolved theme and can therefore be persisted with the normal theme serialization path.

See [Component overrides](public-api/component-overrides.md) for the supported keys and examples.

## Persistence

Use `ThemeSerializer` for JSON/file persistence and `ThemeManager` for active runtime state. Prefer strict reads for user-provided theme files and keep a fallback theme available.

For the complete API and examples, see the [Theming guide](public-api/theming.md).
