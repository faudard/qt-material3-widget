# Public API guide

This section explains the supported public surface of `qt-material3-widget`.

Use these pages for package-level guidance and family-level orientation. For exact member signatures, continue to the generated [C++ API reference](../api/index.md).

## Main API areas

### Theming

- theme construction via `QtMaterial::ThemeBuilder`
- runtime theme access and updates via `QtMaterial::ThemeManager`
- persistence and validation via `QtMaterial::ThemeSerializer`
- system appearance resolution via `QtMaterial::SystemTheme`
- typed component-local overrides via `ComponentTokenOverrides`
- token containers such as `ColorScheme`, `TypographyScale`, `ShapeScale`, `ElevationScale`, `MotionTokens`, and `StateLayer`

### Core widget infrastructure

- reusable interaction and density behavior through base classes such as `QtMaterialControl` and `QtMaterialAbstractButton`
- theme-change propagation into widgets
- focus, hover, press, and accessibility synchronization hooks

### Widget families

- buttons
- compact controls
- inputs
- navigation
- progress indicators
- selection controls
- surfaces
- data widgets

## Include style

Prefer includes from the installed public include tree:

```cpp
#include <qtmaterial/theme/qtmaterialthemebuilder.h>
#include <qtmaterial/theme/qtmaterialthememanager.h>
#include <qtmaterial/widgets/buttons/qtmaterialfilledbutton.h>
```

## Pages

```{toctree}
:maxdepth: 1

theming
theme-json-schema
component-overrides
system-theme
theme-studio
interaction-effects
component-expansion
buttons
compact-controls
data-widgets
inputs
navigation
progress-indicators
selection
surfaces
```
