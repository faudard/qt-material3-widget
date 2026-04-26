# Runtime theme stability

The runtime theming contract is intentionally stricter than a simple setter. `ThemeManager` owns the active resolved theme, prevents duplicate notifications, and exposes a monotonically increasing revision counter for consumers that need to cache derived state.

## Notification contract

A theme application emits exactly one apply cycle when the resolved theme changes:

1. `themeChanged(theme)` for source-compatible existing consumers.
2. `themeChangedWithReason(theme, reason)` for consumers that need to distinguish seed, import, rebuild, or direct-set operations.
3. `themeRepolishRequested(theme, reason)` for widget layers that need to invalidate style, layout, or cached rendering state.

Calling `setThemeOptions`, `setTheme`, `applySeedColor`, or `rebuildTheme` with inputs that resolve to the active theme does not emit and does not advance the revision.

## Thread affinity

`ThemeManager` is a `QObject` singleton. Mutating operations must run on the manager owner thread. Cross-thread calls return `false` and log a diagnostic instead of mutating theme state from an unsafe thread.

## Re-entrancy

Theme application is guarded. A nested attempt to apply another theme from inside a theme-change signal handler is ignored and logged. This keeps widget polish/repolish cascades deterministic.

## Import failure behavior

Import failures are non-mutating. Invalid JSON, strict-schema validation failure, or file-read failure returns `false`, preserves the previous theme, preserves the revision, and emits no theme-change signals.

## Observer lifecycle

`ThemeObserver` still connects automatically to `ThemeManager::themeChanged`, but now supports:

- `setThemeObservationEnabled(false)` to disconnect safely.
- `setThemeObservationEnabled(true)` to reconnect using `Qt::UniqueConnection`.
- `syncTheme()` for derived classes that want an immediate initial callback after construction.

The base constructor does not call the pure virtual `onThemeChanged`; derived classes should call `syncTheme()` when they are ready to consume the current theme.
