# System theme integration

`SystemTheme` resolves platform appearance into the concrete `ThemeMode` consumed by `ThemeBuilder`.

## Policy contract

- `ThemePreference::Light` always resolves to `ThemeMode::Light`.
- `ThemePreference::Dark` always resolves to `ThemeMode::Dark`.
- `ThemePreference::FollowSystem` resolves the platform mode and never enters `Theme` as a third mode.
- high contrast maps to `ContrastMode::High`; otherwise the system bridge uses `ContrastMode::Standard`.

`SystemTheme::resolveMode()` and `SystemTheme::resolveContrast()` are pure helpers so the policy can be tested independently of the CI host appearance.

## Platform support

- Qt 6.5+ uses `QStyleHints::colorScheme()` when available.
- Qt 5.14.2 and older Qt 6 builds fall back to application palette luminance.
- Qt 6.10+ observes `QAccessibilityHints::contrastPreference()` when available.
- platform typography can be applied through `applyPlatformFontToTheme()`.
- reduced-motion platform discovery remains an explicit future policy hook; the 0.7 interaction/effects release owns the reduced-motion contract.

Applications may enable automatic propagation with `setAutoApplyToThemeManager(true)` or use the bridge only as a resolver.
