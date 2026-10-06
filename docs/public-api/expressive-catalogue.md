# Material 3 Expressive catalogue and data

QtMaterial3 extends the opt-in Expressive foundation with first-class Qt Widgets
for the Material 3 Expressive catalogue. Existing 1.x widgets keep their stable
appearance unless their Expressive property is enabled.

## Split Button

`QtMaterialSplitButton` separates a primary action from its related secondary
action while preserving two independent keyboard focus targets.

```cpp
QtMaterial::QtMaterialSplitButton split("Create");
connect(&split, &QtMaterial::QtMaterialSplitButton::primaryTriggered,
        this, &Editor::createDocument);
connect(&split, &QtMaterial::QtMaterialSplitButton::secondaryTriggered,
        this, &Editor::showCreateMenu);
```

The primary and trailing buttons are available through `primaryButton()` and
`trailingButton()` for icons, accessibility labels and application-specific
menu wiring. Expressive size changes are propagated to both segments.

## Button Groups

`QtMaterialButtonGroup` groups `QtMaterialTextButton` derivatives with one
selection model, connected spacing and deterministic keyboard traversal.

```cpp
QtMaterial::QtMaterialButtonGroup group;
group.addButton("Day");
group.addButton("Week");
group.addButton("Month");
group.setCurrentIndex(1);
```

Exclusive groups synchronize each child button's checked state. Left/Right,
Home and End move focus; horizontal traversal mirrors in RTL. Set
`exclusive(false)` for an action group.

## Floating Toolbar

`QtMaterialFloatingToolbar` hosts icon buttons or arbitrary widgets in a
floating Material surface. It supports horizontal or vertical orientation and a
collapsed mode that keeps only the leading action visible.

```cpp
auto *toolbar = new QtMaterial::QtMaterialFloatingToolbar;
toolbar->addAction(editIcon, tr("Edit"));
toolbar->addAction(shareIcon, tr("Share"));
toolbar->setExpanded(false);
```

Arrow-key traversal follows orientation and mirrors horizontal direction in RTL.

## Loading Indicator

`QtMaterialLoadingIndicator` is the Expressive indeterminate loading primitive.
Its silhouette changes while rotating, uses the theme Primary role and stops
animation under reduced motion. `active(false)` produces an idle, non-animating
state.

## Expressive lists and segmented lists

`QtMaterialListItem` now exposes opt-in Expressive rendering and an
`ExpressiveSegmentPosition`. Expressive rows use larger one/two-line geometry
and state-driven rounded containers.

`QtMaterialSegmentedList` assigns `Single`, `First`, `Middle` and `Last`
positions automatically as items are inserted or removed:

```cpp
QtMaterial::QtMaterialSegmentedList list;
list.addItem("Personal");
list.addItem("Work");
list.addItem("Shared");
```

Normal `QtMaterialList` and `QtMaterialListItem` defaults are unchanged.

## Expressive menu

`QtMaterialMenu::setExpressive(true)` applies larger Expressive menu item
geometry, padding and container shape on top of the currently resolved theme
spec. Explicit specs remain the base contract and can be toggled between stable
and Expressive presentation without losing their authored values.

## Morphing chips

`QtMaterialChip::setExpressive(true)` makes the chip container round at rest and
morph toward a compact Expressive shape while pressed or selected. The
transition is immediate when reduced motion is enabled and otherwise uses a
short bounded animation. Assist, Filter, Input and Suggestion behavior remains
unchanged.

## Compatibility

All additions are source-additive for the 1.x line. Qt 5.14.2 remains the
minimum Qt 5 target, and the implementations avoid Qt 6-only APIs.
