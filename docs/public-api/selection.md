# Selection controls

Selection controls let users choose one or more options from a set. QtMaterial3Widgets provides Material-style checkbox, radio button, and switch widgets.

## Public headers

```cpp
#include <qtmaterial/widgets/selection/qtmaterialcheckbox.h>
#include <qtmaterial/widgets/selection/qtmaterialradiobutton.h>
#include <qtmaterial/widgets/selection/qtmaterialswitch.h>
#include <qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h>
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
## Radio button
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

Selection controls synchronize their text labels into accessible names when the application
has not supplied an explicit name. Switch descriptions expose the current On/Off state, and
Segmented Button keeps its accessible group summary synchronized with programmatic segment,
selection, enabled-state and multi-selection changes.

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

## Segmented Button

`QtMaterialSegmentedButton` supports single selection, optional multi-selection, disabled
segments, Left/Right/Home/End navigation, Space/Return/Enter activation and accessible summaries
for the group and individual segments. Rendering covers selected/unselected, enabled/disabled,
focus and RTL states through the normal component test and visual infrastructure.


## Keyboard, RTL and HiDPI contract

Checkbox, Radio Button and Switch retain `Qt::StrongFocus` and ignore activation while
disabled. Space activates the native selection-control contract. Switch directional keys map
to the visual direction: Right means on in LTR and off in RTL; Left means off in LTR and on
in RTL.

Segmented Button supports Left/Right/Home/End navigation, skips disabled segments and maps
horizontal navigation to visual direction under RTL. The selection-family maturity suite also
checks LTR/RTL size-hint stability and DPR 2.0 rendering smoke for Checkbox, Radio Button,
Switch and Segmented Button.


## Consumer component cards

### Checkbox

**Screenshot.** Gallery route `/selection/checkbox`.

**When to use.** Use a checkbox for independent boolean choices, multi-selection, or a mixed/partially selected value.

**API.** `<qtmaterial/widgets/selection/qtmaterialcheckbox.h>`, type `QtMaterialCheckbox`. Standard Qt checked/tristate APIs remain authoritative.

**States.** Unchecked, checked, partially checked, focused and disabled.

**Keyboard.** Tab focuses and Space toggles through the supported check-state model.

**Accessibility.** Provide visible text or an explicit accessible name. Checked/partially-checked and disabled state must remain exposed.

**RTL.** Indicator/label composition follows layout direction.

**Example.**
```cpp
auto *choice = new QtMaterial::QtMaterialCheckbox(parent);
choice->setText(QStringLiteral("Enable notifications"));
choice->setChecked(true);
```

### Radio Button

**Screenshot.** Gallery route `/selection/radio-button`.

**When to use.** Use radio buttons when exactly one choice from a visible set should be selected.

**API.** `<qtmaterial/widgets/selection/qtmaterialradiobutton.h>`, type `QtMaterialRadioButton`. Use Qt button grouping when explicit grouping is required.

**States.** Unchecked, checked, focused and disabled.

**Keyboard.** Native Qt radio/group navigation remains authoritative; keyboard focus and selection are preserved by the Material styling layer.

**Accessibility.** The visible option text should clearly identify the choice; selection and enabled state are exposed.

**RTL.** Indicator/label ordering follows layout direction.

**Example.**
```cpp
auto *metric = new QtMaterial::QtMaterialRadioButton(parent);
metric->setText(QStringLiteral("Metric"));
```

### Switch

**Screenshot.** Gallery route `/selection/switch`.

**When to use.** Use a switch for an on/off setting that takes effect immediately; use a checkbox for values collected as part of a larger form.

**API.** `<qtmaterial/widgets/selection/qtmaterialswitch.h>`, type `QtMaterialSwitch`; use the standard checked state.

**States.** Off/on, focus and disabled.

**Keyboard.** Tab focuses and Space toggles the switch.

**Accessibility.** Give the setting a clear label. The accessibility description synchronizes the current On/Off state.

**RTL.** Track/thumb and label composition respect layout direction.

**Example.**
```cpp
auto *darkMode = new QtMaterial::QtMaterialSwitch(parent);
darkMode->setText(QStringLiteral("Dark mode"));
darkMode->setChecked(false);
```

### Segmented Button

**Screenshot.** Gallery route `/selection/segmented-button`.

**When to use.** Use `QtMaterialSegmentedButton` for a compact set of closely related options, including single- or supported multi-selection workflows.

**API.** `<qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h>`, type `QtMaterialSegmentedButton`.

**States.** Segment selection, disabled segments/group state and focus are synchronized with the component model.

**Keyboard.** Keyboard navigation follows the focused segmented-button contract while retaining native Qt focus behavior where applicable.

**Accessibility.** The group summary remains synchronized with segment, selection, enabled-state and multi-selection changes.

**RTL.** Segment ordering/directional behavior follows the widget layout direction.

**Example.** Use the Selection Gallery segmented-button example to inspect single/multiple selection and disabled states.
