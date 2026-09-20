# Theme Studio

`examples/theme-studio` is the supported 0.6.x developer-facing theme authoring tool.

## Authoring surface

- seed color picker
- Light, Dark, and Follow System preference
- Standard, Medium, and High contrast
- TonalSpot and Expressive variants
- Auto / Prefer MCU / Force MCU / Force Fallback backend policy
- component and semantic color previews
- Light/Dark comparison
- editable resolved Theme JSON v1
- strict validation with parser diagnostics
- apply edited JSON directly to the runtime theme
- JSON import/export
- legacy qt-material XML import/export
- presets plus explicit Apply/Reset dirty-state workflow

Theme Studio imports JSON with `ThemeReadMode::Strict`. A document that violates the frozen Theme JSON v1 contract is rejected before it can replace the active theme.

## Follow System

When Follow System is selected, the controller stores `ThemePreference::FollowSystem` and resolves the concrete `ThemeMode` through `SystemTheme`. The built `Theme` therefore remains strictly Light or Dark.

## Backend selection

`ForceFallback` is useful for deterministic snapshots. MCU policies are available when Material Color Utilities is compiled into the build; the runtime backend status remains observable through `ThemeBuilder`.

See [System theme integration](system-theme.md), [Theme JSON schema](theme-json-schema.md), and [component overrides](component-overrides.md).
