# Native Qt adaptation

Qt Material 3 keeps first-class Material widgets such as `QtMaterialTextButton`,
`QtMaterialFilledButton` and `QtMaterialOutlinedButton`. Existing Qt Widgets
applications do not have to replace every `QPushButton`, `QCheckBox`, `QRadioButton`, `QSlider` or `QComboBox` immediately, however.

The `Widgets` module provides an opt-in adapter for existing buttons:

```cpp
#include <qtmaterial/widgets/native/qtmaterialbuttonadapter.h>

QtMaterial::QtMaterialButtonAdapter::apply(
    ui->saveButton,
    QtMaterial::ButtonVariant::Filled);
```

The object remains a `QPushButton`. Existing connections, checkable/default
behavior, object names, menus and Designer-authored ownership stay intact.

## Change the variant at runtime

```cpp
QtMaterial::QtMaterialButtonAdapter::setVariant(
    ui->saveButton,
    QtMaterial::ButtonVariant::Outlined);

QtMaterial::QtMaterialButtonAdapter::setDensity(
    ui->saveButton,
    QtMaterial::Density::Compact);
```

The supported button variants are `Text`, `Filled`, `FilledTonal`,
`Outlined` and `Elevated`. They resolve through the same
`ButtonSpecResolver` used by the first-class Material button family.

## Dynamic properties and Qt Designer

The adapter uses namespaced dynamic properties so authored/native forms can be
migrated without changing the widget class:

```cpp
ui->saveButton->setProperty("qtm3MaterialVariant", "filled");
ui->saveButton->setProperty("qtm3MaterialDensity", "default");
QtMaterial::QtMaterialButtonAdapter::apply(ui->saveButton);
```

After a button has been adapted, changing `qtm3MaterialVariant` or
`qtm3MaterialDensity` repaints and recomputes its geometry automatically.

## Adapt a widget tree

```cpp
QtMaterial::QtMaterialButtonAdapter::applyToDescendants(
    this,
    QtMaterial::ButtonVariant::Text);
```

Opt a specific button out before adapting a form:

```cpp
QtMaterial::QtMaterialButtonAdapter::setOptOut(
    ui->legacySpecialButton, true);
```

## Remove adaptation

```cpp
QtMaterial::QtMaterialButtonAdapter::remove(ui->saveButton);
```

The adapter restores the style that was installed on the button before
adaptation.

## Scope

This path is designed for progressive migration. It provides Material token,
shape, density, hover/focus/press state-layer and label rendering while keeping
native `QPushButton` semantics.

It deliberately does **not** pretend that a native `QPushButton` has become a
`QtMaterialTextButton`. Ripple controllers, Expressive shape morphing and the
full Material-specific interaction pipeline remain features of the first-class
Material widget classes. Applications can therefore mix both approaches and
migrate incrementally.


## Native selection controls

Existing native selection controls can opt into Material rendering without
changing their Qt type:

```cpp
#include <qtmaterial/widgets/native/qtmaterialselectionadapter.h>

QtMaterial::QtMaterialSelectionAdapter::apply(
    ui->rememberCheckBox,
    QtMaterial::Density::Default);

QtMaterial::QtMaterialSelectionAdapter::apply(
    ui->automaticRadioButton);
```

The adapter preserves native Qt behavior:

- `QCheckBox::tristate` and `Qt::PartiallyChecked`;
- `QRadioButton::autoExclusive` grouping;
- existing `toggled()`, `clicked()` and state-change connections;
- widget ownership, object names and Designer-authored forms;
- enabled/disabled, hover, press, focus and RTL behavior.

Both controls resolve the canonical `SelectionSpecResolver` through the
effective `ThemeContext`. Density remains live through the shared
`qtm3MaterialDensity` property.

A whole form can be migrated incrementally:

```cpp
QtMaterial::QtMaterialSelectionAdapter::applyToDescendants(this);
```

Use `qtm3MaterialOptOut=true` (or `setOptOut()`) for exceptional controls.
`remove()` restores the previously installed native style.


## Native slider

A native `QSlider` can keep its complete Qt contract while opting into
Material track/handle/state rendering:

```cpp
#include <qtmaterial/widgets/native/qtmaterialslideradapter.h>

QtMaterial::QtMaterialSliderAdapter::apply(
    ui->volumeSlider,
    QtMaterial::Density::Comfortable);
```

The adapter preserves the existing range, value, orientation, tick position,
single/page steps, inverted appearance/controls, signals and keyboard/mouse
behavior. It implements Material geometry through the native
`QStyle::CC_Slider` sub-control contract, so the rendered handle and Qt's drag
hit target use the same rectangle.

Horizontal, vertical, RTL and inverted sliders are supported. Native tick marks
remain delegated to the platform style while the groove, active track, state
layer, handle and focus ring use the resolved Material `SliderSpec`.

As with the other native adapters, `qtm3MaterialDensity` is live,
`qtm3MaterialOptOut` skips exceptional controls, `applyToDescendants()`
supports form-wide migration, and `remove()` restores the previous style.

The existing first-class `QtMaterialSlider` visual baseline is intentionally
left unchanged in this adapter PR. Moving that established component onto the
new canonical SliderSpec requires its own reviewed visual-baseline change.


## Native combo box

Existing `QComboBox` instances can use the same resolved input-field language
without replacing their model/view stack:

```cpp
#include <qtmaterial/widgets/native/qtmaterialcomboboxadapter.h>

QtMaterial::QtMaterialComboBoxAdapter::apply(
    ui->countryCombo,
    QtMaterial::Density::Default);
```

The adapter preserves the original `QAbstractItemModel`, popup view,
application-provided item delegate, editable `QLineEdit`, current index/text
and selection signals. It changes only the closed control's
`QStyle::CC_ComboBox` rendering and geometry.

The container, state layer, outline/focus ring, current-item label and arrow use
the existing `AutocompleteSpecResolver`. `SC_ComboBoxArrow` and
`SC_ComboBoxEditField` are RTL-aware, so Qt's native hit testing and editable
line-edit placement stay aligned with the Material rendering.

The popup remains application/Qt-owned deliberately: the adapter does not swap
models, views or delegates. Applications that want the full Material popup
surface and item treatment should use the first-class `QtMaterialComboBox`.
