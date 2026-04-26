# Theme Studio and system appearance integration

`examples/theme-playground` is now treated as a developer-facing Theme Studio rather
than a minimal smoke-test window.

## Theme Studio features

- seed color picker
- light, dark, and follow-system mode controls
- contrast mode selector
- typography preview
- shape preview
- elevation preview
- state-layer preview
- component preview grid
- resolved JSON export/import
- copy resolved JSON
- copy C++ snippet
- copy CSS-like token report
- screenshot export

## System theme integration

Use `QtMaterial::SystemTheme` when an application wants Material themes to follow
platform appearance changes:

```cpp
QtMaterial::SystemTheme::instance().setPreference(
    QtMaterial::ThemeModePreference::FollowSystem);
QtMaterial::SystemTheme::instance().setUsePlatformFont(true);
QtMaterial::SystemTheme::instance().setAutoApplyToThemeManager(true);
```

`SystemTheme` resolves effective light/dark mode from `QStyleHints::colorScheme`
when available and falls back to palette luminance otherwise. On Qt 6.10 and newer,
it reads `QAccessibilityHints::contrastPreference` and maps high contrast to
`ContrastMode::High`.

The bridge is optional. Existing applications can continue to use `ThemeManager`
directly and ignore the system-theme helper.
