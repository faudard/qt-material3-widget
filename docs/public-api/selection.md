# Selection controls

Selection controls let users choose one or more options from a set. QtMaterial3Widgets provides Material-style checkbox, radio button, and switch widgets.

## Public headers

```cpp
#include <qtmaterial/widgets/selection/qtmaterialcheckbox.h>
#include <qtmaterial/widgets/selection/qtmaterialradiobutton.h>
#include <qtmaterial/widgets/selection/qtmaterialswitch.h>
```
## Checkbox

Use QtMaterialCheckbox when the user can enable or disable an independent option, or when multiple options may be selected at the same time.
```cpp
auto* checkbox = new QtMaterial::QtMaterialCheckbox(parent);
checkbox->setText(QStringLiteral("Enable notifications"));
checkbox->setChecked(true);
```
A checkbox may also represent an indeterminate state when the value is mixed or partially selected.
```cpp
checkbox->setTristate(true);
checkbox->setCheckState(Qt::PartiallyChecked);
Radio button
```
Use QtMaterialRadioButton when the user must choose one option from a mutually exclusive group.
```cpp
auto* optionA = new QtMaterial::QtMaterialRadioButton(parent);
optionA->setText(QStringLiteral("Option A"));

auto* optionB = new QtMaterial::QtMaterialRadioButton(parent);
optionB->setText(QStringLiteral("Option B"));
```
Group radio buttons with Qt's standard button grouping APIs when exclusivity is required across several controls.

## Switch

Use QtMaterialSwitch for an immediate on/off setting.
```cpp
auto* sw = new QtMaterial::QtMaterialSwitch(parent);
sw->setText(QStringLiteral("Use dark mode"));
sw->setChecked(false);
```
Prefer switches for settings that take effect immediately. Prefer checkboxes when the value is submitted as part of a larger form.

## Accessibility

Selection controls should expose a clear text label or accessible name.

```cpp
checkbox->setAccessibleName(QStringLiteral("Enable notifications"));
```
For automation and functional tests, assign a stable test id when available:
```cpp
checkbox->setProperty("materialTestId", QStringLiteral("notifications-checkbox"));
```
## Theming

Selection controls resolve colors, state layers, focus indication, shape, density, and disabled state from the active Material theme. They should not hard-code palette values in application code.

## Segmented Button release readiness

See `docs/public-api/segmentedbutton-release-readiness.md`.

