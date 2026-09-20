# Qt Material 3 Theme Studio

`qtmaterial3_theme_studio` is the supported developer-facing theme authoring example for the 0.6.x release.

## Features

- seed color selection
- Light, Dark, and Follow System preference
- Standard, Medium, and High contrast
- TonalSpot and Expressive variants
- Auto, Prefer MCU, Force MCU, and Force Fallback backend policies
- live component and color-role previews
- Light/Dark comparison
- editable Theme JSON v1 view
- strict JSON validation and direct apply
- JSON import/export
- legacy qt-material XML import/export
- built-in presets and dirty/apply/reset workflow

## Build

```bash
cmake -S . -B build -DQTMATERIAL3_BUILD_EXAMPLES=ON -DQTMATERIAL3_BUILD_THEMING_EXAMPLES=ON
cmake --build build --target qtmaterial3_theme_studio
```

The example uses the same public ThemeBuilder, ThemeSerializer, SystemTheme, and ThemeManager contracts available to downstream consumers.
