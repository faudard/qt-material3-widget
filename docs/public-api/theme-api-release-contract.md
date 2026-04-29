# Theme API release contract

This document describes the first official theme contract. Earlier experimental
JSON files are intentionally not part of the public compatibility surface.

## Resolved mode vs user preference

`ThemeMode` is the resolved mode stored in a built `Theme`:

```cpp
ThemeMode::Light
ThemeMode::Dark
```

`ThemePreference` is the user-facing setting:

```cpp
ThemePreference::Light
ThemePreference::Dark
ThemePreference::FollowSystem
```

`FollowSystem` is resolved by `SystemTheme` before calling `ThemeBuilder`.

## Generation options

`ThemeOptions` is the canonical input to `ThemeBuilder`:

```cpp
QtMaterial::ThemeOptions options;
options.sourceColor = QColor("#6750A4");
options.preference = QtMaterial::ThemePreference::FollowSystem;
options.mode = QtMaterial::ThemeMode::Light; // resolved mode
options.contrast = QtMaterial::ContrastMode::Standard;
options.variant = QtMaterial::ThemeVariant::TonalSpot;
options.backendPolicy = QtMaterial::ColorBackendPolicy::Auto;
```

`ThemeVariant::Expressive` selects Material Color Utilities variant generation
when the MCU backend is available.

`ColorBackendPolicy::Auto` is the default. It uses Material Color Utilities when
compiled in and otherwise falls back to the deterministic Qt implementation.

## JSON format

The first public JSON format is:

```text
docs/schema/theme.schema.json
```

The root field `formatVersion` is `1`. There is no public v1/v2 migration layer
because no official release has shipped yet.

The `source` object stores:

```json
{
  "seedColor": "#ff6750a4",
  "mode": "Light",
  "preference": "FollowSystem",
  "contrast": "Standard",
  "variant": "TonalSpot",
  "colorBackendPolicy": "Auto"
}
```

## Validation

Use `validateTheme(theme)` before exporting, snapshotting, or accepting a user
provided theme file. Validation checks required Material color roles, key contrast
pairs, and token ranges.
