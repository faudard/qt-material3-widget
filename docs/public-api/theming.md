# Theming guide

This guide is for application developers consuming `qt-material3-widget`.

Use this page when you need to:

- generate a theme from a seed color;
- switch light and dark mode at runtime;
- serialize a theme to JSON;
- restore a theme from JSON or from a file;
- understand which type to use for each theming workflow.

For exact member signatures, continue to the generated [C++ API reference](../api/index.md).

## Mental model

The theming layer is split into four roles:

- `QtMaterial::ThemeOptions` describes the build inputs for a theme;
- `QtMaterial::ThemeBuilder` constructs a resolved `QtMaterial::Theme`;
- `QtMaterial::ThemeSerializer` converts a resolved theme to and from JSON;
- `QtMaterial::ThemeManager` owns the active runtime theme used by the application.

In practice:

- use `ThemeBuilder` for deterministic construction;
- use `ThemeSerializer` for persistence and transport;
- use `ThemeManager` for application-wide runtime state.

`ThemeOptions::mode` is the resolved mode consumed by `ThemeBuilder`. It is always either `ThemeMode::Light` or `ThemeMode::Dark`.

`ThemeOptions::preference` is the user-facing preference. It may be `ThemePreference::FollowSystem`, but that preference must be resolved to a concrete `ThemeMode` before the final theme is built.

## Include style

Prefer installed public headers:

```cpp
#include <qtmaterial/theme/qtmaterialthemebuilder.h>
#include <qtmaterial/theme/qtmaterialthememanager.h>
#include <qtmaterial/theme/qtmaterialthemeserializer.h>
#include <qtmaterial/theme/qtmaterialthemeoptions.h>
```

## Build a theme from options

Use `ThemeOptions` when you want an explicit, reproducible build configuration.

```cpp
#include <QApplication>
#include <QColor>

#include <qtmaterial/theme/qtmaterialthemebuilder.h>
#include <qtmaterial/theme/qtmaterialthememanager.h>
#include <qtmaterial/theme/qtmaterialthemeoptions.h>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QtMaterial::ThemeOptions options;
    options.sourceColor = QColor("#6750A4");
    options.mode = QtMaterial::ThemeMode::Light;
    options.preference = QtMaterial::ThemePreference::Light;
    options.contrast = QtMaterial::ContrastMode::Standard;
    options.variant = QtMaterial::ThemeVariant::TonalSpot;
    options.backendPolicy = QtMaterial::ColorBackendPolicy::Auto;

    const QtMaterial::Theme theme = QtMaterial::ThemeBuilder{}.build(options);
    QtMaterial::ThemeManager::instance().setTheme(theme);

    return app.exec();
}
```

Use this path when your application stores user-facing theme settings such as seed color, light/dark preference, contrast, variant, or backend policy.

## Theme variants

`ThemeVariant` controls the dynamic color variant used when a seed color generates a scheme.

```cpp
options.variant = QtMaterial::ThemeVariant::TonalSpot;
options.variant = QtMaterial::ThemeVariant::Expressive;
```

Use `TonalSpot` as the stable default. Use `Expressive` when the application intentionally wants a stronger Material dynamic-color personality.

## Color backend policy

`ColorBackendPolicy` controls how the color-generation backend is selected.

```cpp
options.backendPolicy = QtMaterial::ColorBackendPolicy::Auto;
options.backendPolicy = QtMaterial::ColorBackendPolicy::PreferMaterialColorUtilities;
options.backendPolicy = QtMaterial::ColorBackendPolicy::ForceMaterialColorUtilities;
options.backendPolicy = QtMaterial::ColorBackendPolicy::ForceFallback;
```

Recommended defaults:

- `Auto` for most applications;
- `PreferMaterialColorUtilities` when you want the Material Color Utilities backend when available, but can accept the fallback backend;
- `ForceMaterialColorUtilities` for tooling or tests that must fail if the backend is unavailable;
- `ForceFallback` for deterministic fallback snapshots or for builds that intentionally avoid Material Color Utilities.

## Build directly from a seed color

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

This is a good fit for theme pickers, demos, onboarding flows, and applications that do not need to expose every theme option explicitly.

## Generate a `ColorScheme` directly from a seed

`ThemeBuilder` can also produce a `ColorScheme` directly without first moving a full `Theme` through the manager.

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

## Apply a seed color at runtime

`ThemeManager` is the best entry point when the application has a single active theme and the user changes the seed color at runtime.

```cpp
#include <QColor>

#include <qtmaterial/theme/qtmaterialthememanager.h>

QtMaterial::ThemeManager& manager = QtMaterial::ThemeManager::instance();

// Rebuild using the current mode from manager.options().
manager.applySeedColor(QColor("#00639B"));

// Or force a specific resolved mode.
manager.applySeedColor(QColor("#00639B"), QtMaterial::ThemeMode::Dark);
```

This is the recommended consumer-facing API for a settings screen or live theme playground.

## Export the active theme to JSON

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
#include <QDebug>

QString error;
const bool ok = QtMaterial::ThemeManager::instance().exportThemeToFile(
    "theme.json",
    &error,
    QJsonDocument::Indented);

if (!ok) {
    qWarning() << "Failed to export theme:" << error;
}
```

Use the manager export functions when you want to persist the exact active runtime theme, not only the source options.

## Import a theme from JSON

To restore a previously exported theme, import JSON through `ThemeManager`.

```cpp
#include <QByteArray>
#include <QDebug>

#include <qtmaterial/theme/qtmaterialthememanager.h>

const QByteArray json = R"json(
{
  "formatVersion": 1,
  "source": {
    "seedColor": "#6750A4FF",
    "mode": "Light",
    "preference": "Light",
    "contrast": "Standard",
    "variant": "TonalSpot",
    "colorBackendPolicy": "Auto"
  },
  "resolved": {
    "colorScheme": {},
    "typographyScale": {},
    "shapeScale": {},
    "elevationScale": {},
    "motionTokens": {},
    "stateLayer": {
      "color": "#000000FF",
      "hoverOpacity": 0.08,
      "focusOpacity": 0.12,
      "pressOpacity": 0.12,
      "dragOpacity": 0.16
    },
    "accessibility": {
      "highContrast": false,
      "reducedMotion": false,
      "minimumTextContrastRatio": 4.5,
      "minimumUiContrastRatio": 3.0,
      "focusRing": {
        "width": 2,
        "offset": 2,
        "radiusAdjustment": 0,
        "color": "#6750A4FF",
        "opacity": 1.0
      }
    },
    "interactions": {
      "keyboardFocusVisible": true,
      "strongFocusIndicators": false,
      "hoverFeedbackEnabled": true,
      "pressFeedbackEnabled": true,
      "dragFeedbackEnabled": true
    },
    "density": {},
    "iconSizes": {},
    "componentOverrides": {}
  },
  "metadata": {
    "generatorVersion": "qt-material3-widget",
    "libraryVersion": "0.1.0",
    "qtVersion": "6.x"
  }
}
)json";

QString error;
const bool ok = QtMaterial::ThemeManager::instance().importThemeJson(
    json,
    &error,
    QtMaterial::ThemeReadMode::Strict);

if (!ok) {
    qWarning() << "Failed to import theme JSON:" << error;
}
```

Or from a file:

```cpp
QString error;
const bool ok = QtMaterial::ThemeManager::instance().importThemeFromFile(
    "theme.json",
    &error,
    QtMaterial::ThemeReadMode::Lenient);

if (!ok) {
    qWarning() << "Failed to import theme file:" << error;
}
```

After a successful import, consumers should treat the imported `Theme` as the new active runtime theme.

## Use `ThemeSerializer` directly

Use `ThemeSerializer` when you need serialization outside the singleton manager, for example in tests, tooling, batch conversion, or custom storage flows.

### Serialize a resolved theme

```cpp
#include <QByteArray>
#include <QColor>
#include <QJsonDocument>

#include <qtmaterial/theme/qtmaterialthemebuilder.h>
#include <qtmaterial/theme/qtmaterialthemeserializer.h>

QtMaterial::ThemeBuilder builder;
const QtMaterial::Theme theme = builder.buildLightFromSeed(QColor("#6750A4"));

const QByteArray json = QtMaterial::ThemeSerializer::toJson(
    theme,
    QJsonDocument::Indented);
```

### Deserialize a resolved theme

```cpp
#include <QDebug>

#include <qtmaterial/theme/qtmaterialthemeserializer.h>

QString error;
bool ok = false;

const QtMaterial::Theme theme = QtMaterial::ThemeSerializer::fromJson(
    json,
    QtMaterial::ThemeReadMode::Strict,
    &ok,
    &error);

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
    theme,
    "theme.json",
    &error);

if (!writeOk) {
    qWarning() << "Failed to write theme:" << error;
}

QtMaterial::Theme restoredTheme;
const bool readOk = QtMaterial::ThemeSerializer::readFromFile(
    "theme.json",
    &restoredTheme,
    QtMaterial::ThemeReadMode::Strict,
    &error);

if (!readOk) {
    qWarning() << "Failed to read theme:" << error;
}
```

## Typical consumer workflows

### Settings screen with theme persistence

A common application flow is:

1. the user chooses a seed color, light/dark preference, contrast, and variant;
2. the application resolves `ThemePreference::FollowSystem` to `ThemeMode::Light` or `ThemeMode::Dark`;
3. the application builds or rebuilds the theme through `ThemeBuilder` or `ThemeManager`;
4. the application exports the active theme to JSON on save;
5. the application imports that JSON at the next launch.

This keeps the runtime path simple while preserving the full resolved theme.

### Theme preview tool

For previews and screenshots:

1. use `ThemeBuilder` to generate candidate themes from different seeds;
2. optionally extract only `ColorScheme` for custom preview widgets;
3. promote the selected theme into `ThemeManager` only when the user confirms.

### Tooling or tests

For tests and offline utilities:

1. construct a `Theme` with `ThemeBuilder`;
2. serialize it with `ThemeSerializer`;
3. deserialize it back;
4. compare the token groups that matter to your workflow.

## Recommended ownership rules

Prefer these rules in application code:

- UI code that needs the active theme should read from `ThemeManager`;
- code that builds themes should use `ThemeBuilder`;
- code that saves or loads themes should use `ThemeSerializer` or the corresponding `ThemeManager` helpers;
- avoid ad hoc JSON formats for theme persistence when the library already provides one.

This keeps theme construction, runtime state, and persistence clearly separated.

## Error-handling guidance

When using file or JSON import APIs:

- surface `errorString` in logs or UI;
- treat imported themes as untrusted input;
- prefer `ThemeReadMode::Strict` for user-provided files or tests;
- use `ThemeReadMode::Lenient` only when forward-compatible extra data should be tolerated;
- keep a fallback path, such as rebuilding from `ThemeOptions` or from a default seed color.

Example fallback:

```cpp
#include <QColor>
#include <QDebug>

QString error;
if (!QtMaterial::ThemeManager::instance().importThemeFromFile("theme.json", &error)) {
    qWarning() << "Import failed, falling back to default theme:" << error;
    QtMaterial::ThemeManager::instance().applySeedColor(QColor("#6750A4"));
}
```

## Relationship between options, theme, and JSON

Keep the type boundaries explicit:

- `ThemeOptions` are compact build inputs;
- `Theme` is the fully resolved output;
- exported JSON represents the resolved theme plus the source inputs and metadata.

That distinction matters because a resolved theme carries concrete token values that are not recoverable from a reduced options-only payload.

## JSON schema contract

The repository ships one current pre-release theme schema:

```text
docs/schema/theme.schema.json
```

The schema documents the current `ThemeSerializer::kCurrentFormatVersion`, which is `1` for the first official theme JSON contract.

There are no public legacy `v1`/`v2` migration promises yet. Until the first official release, breaking changes to this pre-release schema may still happen. After release, schema compatibility rules should be documented in the release notes.

## See also

- [Theme JSON schema](theme-json-schema.md)
- [Public API guide](index.md)
- [C++ API reference](../api/index.md)
- `examples/theme-playground` for an interactive consumer example
- Material 3 references in [the documentation root](../index.md)
