# Theming guide

This guide is for application developers consuming `qt-material3-widget`.

Use this page when you need to:

- generate a theme from a seed color
- switch light and dark mode at runtime
- serialize a theme to JSON
- restore a theme from JSON or from a file
- understand which type to use for each theming workflow

For exact member signatures, continue to the generated [C++ API reference](../api/index.md).

## Mental model

The theming layer is split into four roles:

- `QtMaterial::ThemeOptions` describes **how a theme should be built**
- `QtMaterial::ThemeBuilder` constructs a **resolved `Theme`** (and, in branches that include the seed-scheme extension, direct `ColorScheme` values from a seed)
- `QtMaterial::ThemeSerializer` converts a resolved `Theme` to and from JSON
- `QtMaterial::ThemeManager` owns the **active runtime theme** used by the application

In practice:

- use `ThemeBuilder` for deterministic construction
- use `ThemeSerializer` for persistence and transport
- use `ThemeManager` for app-wide runtime state

## Include style

Prefer installed public headers:

```cpp
#include <qtmaterial/theme/qtmaterialtheme.h>
#include <qtmaterial/theme/qtmaterialthemebuilder.h>
#include <qtmaterial/theme/qtmaterialthememanager.h>
#include <qtmaterial/theme/qtmaterialthemeserializer.h>
```

## 1. Build a theme from options

Use `ThemeOptions` when you want an explicit, reproducible build configuration.

```cpp
#include <QApplication>
#include <QColor>

#include <qtmaterial/theme/qtmaterialthemebuilder.h>
#include <qtmaterial/theme/qtmaterialthememanager.h>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QtMaterial::ThemeOptions options;
    options.sourceColor = QColor("#6750A4");
    options.mode = QtMaterial::ThemeMode::Light;
    options.contrast = QtMaterial::ContrastMode::DefaultContrast;
    options.expressive = false;
    options.useMaterialColorUtilities = true;

    const QtMaterial::Theme theme = QtMaterial::ThemeBuilder{}.build(options);
    QtMaterial::ThemeManager::instance().setTheme(theme);

    return app.exec();
}
```

Use this path when your app stores user-facing theme settings such as mode, seed color, contrast, or expressive mode.

## 2. Build directly from a seed color

When the application only needs a light or dark theme from a seed color, the convenience seed APIs are the shortest path.

```cpp
#include <QColor>

#include <qtmaterial/theme/qtmaterialthemebuilder.h>
#include <qtmaterial/theme/qtmaterialthememanager.h>

QtMaterial::ThemeBuilder builder;

const QtMaterial::Theme lightTheme = builder.buildLightFromSeed(QColor("#6750A4"));
QtMaterial::ThemeManager::instance().setTheme(lightTheme);

const QtMaterial::Theme darkTheme = builder.buildDarkFromSeed(QColor("#6750A4"));
QtMaterial::ThemeManager::instance().setTheme(darkTheme);
```

This is a good fit for theme pickers, demos, onboarding flows, and apps that do not need to expose every theme option explicitly.

## 3. Generate a `ColorScheme` directly from a seed

If your branch includes the seed-scheme extension, `ThemeBuilder` can also produce a `ColorScheme` directly without first moving a full `Theme` through the manager.

```cpp
#include <QColor>

#include <qtmaterial/theme/qtmaterialthemebuilder.h>

QtMaterial::ThemeBuilder builder;

const QtMaterial::ColorScheme lightScheme =
    builder.buildLightSchemeFromSeed(QColor("#6750A4"));

const QtMaterial::ColorScheme darkScheme =
    builder.buildDarkSchemeFromSeed(QColor("#6750A4"));
```

Use direct scheme generation when you need resolved Material color roles for custom painting, previews, screenshots, or offline processing.

If your branch does not expose these methods yet, build a full theme and read the scheme from `theme.colorScheme` instead.

## 4. Apply a seed color at runtime

`ThemeManager` is the best entry point when the app has a single active theme and the user changes the seed at runtime.

```cpp
#include <QColor>

#include <qtmaterial/theme/qtmaterialthememanager.h>

QtMaterial::ThemeManager& manager = QtMaterial::ThemeManager::instance();

// Rebuild using the current mode from manager.options()
manager.applySeedColor(QColor("#00639B"));

// Or force a specific mode
manager.applySeedColor(QColor("#00639B"), QtMaterial::ThemeMode::Dark);
```

This is the recommended consumer-facing API for a settings screen or a live theme playground.

## 5. Export the active theme to JSON

Use `ThemeManager` when you want to export the current runtime theme directly.

```cpp
#include <QByteArray>
#include <QJsonDocument>

#include <qtmaterial/theme/qtmaterialthememanager.h>

const QByteArray json = QtMaterial::ThemeManager::instance().exportThemeJson(
    QJsonDocument::Indented);
```

Write to disk directly:

```cpp
QString error;
const bool ok = QtMaterial::ThemeManager::instance().exportThemeToFile(
    "theme.json", &error, QJsonDocument::Indented);

if (!ok) {
    qWarning() << "Failed to export theme:" << error;
}
```

Use the manager export functions when you want to persist the exact active runtime theme, not just the source options.

## 6. Import a theme from JSON

To restore a previously exported theme, import JSON through `ThemeManager`.

```cpp
#include <QByteArray>
#include <QDebug>

#include <qtmaterial/theme/qtmaterialthememanager.h>

const QByteArray json = R"json(
{
  "formatVersion": 1
}
)json";

QString error;
const bool ok = QtMaterial::ThemeManager::instance().importThemeJson(json, &error);

if (!ok) {
    qWarning() << "Failed to import theme:" << error;
}
```

Or from a file:

```cpp
QString error;
const bool ok = QtMaterial::ThemeManager::instance().importThemeFromFile(
    "theme.json", &error);

if (!ok) {
    qWarning() << "Failed to import theme file:" << error;
}
```

After a successful import, consumers should treat the imported `Theme` as the new active runtime theme.

## 7. Use `ThemeSerializer` directly

Use `ThemeSerializer` when you need serialization outside the singleton manager, for example in tests, tooling, batch conversion, or custom storage flows.

### Serialize a resolved theme

```cpp
#include <QJsonDocument>

#include <qtmaterial/theme/qtmaterialthemebuilder.h>
#include <qtmaterial/theme/qtmaterialthemeserializer.h>

QtMaterial::ThemeBuilder builder;
const QtMaterial::Theme theme = builder.buildLightFromSeed(QColor("#6750A4"));

const QByteArray json = QtMaterial::ThemeSerializer::toJson(
    theme, QJsonDocument::Indented);
```

### Deserialize a resolved theme

```cpp
#include <QByteArray>
#include <QDebug>

#include <qtmaterial/theme/qtmaterialthemeserializer.h>

QString error;
bool ok = false;

const QtMaterial::Theme theme =
    QtMaterial::ThemeSerializer::fromJson(json, &ok, &error);

if (!ok) {
    qWarning() << "Failed to parse theme JSON:" << error;
}
```

### File-based serialization

```cpp
#include <QDebug>

#include <qtmaterial/theme/qtmaterialthemeserializer.h>

QString error;

const bool writeOk = QtMaterial::ThemeSerializer::writeToFile(
    theme, "theme.json", &error);

if (!writeOk) {
    qWarning() << "Failed to write theme:" << error;
}

QtMaterial::Theme restoredTheme;
const bool readOk = QtMaterial::ThemeSerializer::readFromFile(
    "theme.json", &restoredTheme, &error);

if (!readOk) {
    qWarning() << "Failed to read theme:" << error;
}
```

## 8. Typical consumer workflows

### Settings screen with theme persistence

A common application flow is:

1. user chooses a seed color and mode
2. app calls `ThemeManager::applySeedColor(...)`
3. app exports the active theme to JSON on save
4. app imports that JSON at the next launch

This keeps the runtime path simple while preserving the full resolved theme.

### Theme preview tool

For previews and screenshots:

1. use `ThemeBuilder` to generate several candidate themes from different seeds
2. optionally extract only `ColorScheme` for custom preview widgets
3. promote the selected theme into `ThemeManager` only when the user confirms

### Tooling or tests

For tests and offline utilities:

1. construct a `Theme` with `ThemeBuilder`
2. serialize it with `ThemeSerializer`
3. deserialize it back
4. compare the token groups that matter to your workflow

## 9. Recommended ownership rules

Prefer these rules in application code:

- UI code that needs the active theme should read from `ThemeManager`
- code that builds themes should use `ThemeBuilder`
- code that saves or loads themes should use `ThemeSerializer` or the corresponding `ThemeManager` helpers
- avoid ad hoc JSON formats for theme persistence when the library already provides one

This keeps theme construction, runtime state, and persistence clearly separated.

## 10. Error-handling guidance

When using file or JSON import APIs:

- surface `errorString` in logs or UI
- treat imported themes as untrusted input
- keep a fallback path, such as rebuilding from `ThemeOptions` or from a default seed color

Example fallback:

```cpp
QString error;
if (!QtMaterial::ThemeManager::instance().importThemeFromFile("theme.json", &error)) {
    qWarning() << "Import failed, falling back to default theme:" << error;
    QtMaterial::ThemeManager::instance().applySeedColor(QColor("#6750A4"));
}
```

## 11. Relationship between options, theme, and JSON

It is useful to keep the type boundaries explicit:

- `ThemeOptions` are compact build inputs
- `Theme` is the fully resolved output
- exported JSON should represent the resolved theme, not only the original options

That distinction matters because a resolved theme can carry concrete token values that are not recoverable from a reduced options-only payload.

## 12. See also

- [Public API guide](index.md)
- [C++ API reference](../api/index.md)
- `examples/theme-playground` for an interactive consumer example
- Material 3 references in [the documentation root](../index.md)

## Suggested next documentation improvements

The next most useful follow-ups would be:

- add a short JSON schema note for theme export
- add a migration note for apps moving from option-only persistence to full theme export
- add a widget-author page explaining how widgets should consume resolved theme roles instead of inventing local theme keys
