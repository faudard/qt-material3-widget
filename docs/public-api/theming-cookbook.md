# Theming cookbook

## Build a theme from a seed color

```cpp
QtMaterial::ThemeOptions options;
options.sourceColor = QColor("#6750A4");
options.mode = QtMaterial::ThemeMode::Light;
options.contrast = QtMaterial::ContrastMode::Standard;

QtMaterial::ThemeBuilder builder;
const QtMaterial::Theme theme = builder.build(options);
```

## Apply a theme at runtime

```cpp
QtMaterial::ThemeOptions options = QtMaterial::ThemeManager::instance().options();
options.sourceColor = QColor("#00639B");
options.mode = QtMaterial::ThemeMode::Dark;

QtMaterial::ThemeManager::instance().setThemeOptions(options);
```

`ThemeManager` owns runtime theme state. Widgets and examples should observe the manager instead of independently rebuilding themes.

## Export a resolved theme

```cpp
const QByteArray json = QtMaterial::ThemeSerializer::toJson(
    QtMaterial::ThemeManager::instance().theme(),
    QJsonDocument::Indented);
```

The exported JSON contains source inputs and resolved tokens. That makes snapshots useful for debugging and design review.

## Read a theme in strict mode

```cpp
bool ok = false;
QString error;
const QtMaterial::Theme theme = QtMaterial::ThemeSerializer::fromJson(
    json,
    QtMaterial::ThemeReadMode::Strict,
    &ok,
    &error);
```

Use strict mode for CI fixtures and user-authored theme packs. Use lenient mode for application preference files when forward compatibility matters more than rejecting unknown keys.

## Add component-local overrides

```cpp
QtMaterial::ComponentTokenOverride filledButton;
filledButton.colors.insert(QtMaterial::ColorRole::Primary, QColor("#004A77"));
filledButton.colors.insert(QtMaterial::ColorRole::OnPrimary, QColor("#FFFFFF"));
filledButton.custom.insert("minHeight", 44);

theme.componentOverrides().setOverride("button.filled", filledButton);
```

Recommended override key order:

1. family key, for example `button`
2. component key, for example `button.filled`
3. legacy alias only when maintaining compatibility with old code

## Check backend selection

```cpp
QtMaterial::ThemeOptions options;
options.sourceColor = QColor("#6750A4");
options.useMaterialColorUtilities = true;

QtMaterial::ThemeBuilder builder;
const QtMaterial::ThemeColorBackendStatus status = builder.colorBackendStatus(options);
```

Applications should not infer MCU availability from the request flag alone. Always check the effective backend if behavior depends on MCU-specific output.

## Recommended app startup flow

1. Build default `ThemeOptions` from application branding.
2. Load persisted JSON using `UpgradeIfPossible`.
3. Fall back to the default generated theme if loading fails.
4. Install the theme through `ThemeManager`.
5. Let widgets observe manager notifications and repolish once per revision.
