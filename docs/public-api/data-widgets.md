# Data widgets

The data family contains lightweight Material wrappers that keep standard Qt model/view and
layout extension points intact.

## Divider

`QtMaterialDivider` is decorative by default and supports horizontal/vertical orientation,
leading/trailing insets, configurable thickness and palette/spec-resolved color. A divider that
carries semantic meaning can be made non-decorative and given an accessibility label. Horizontal
leading/trailing insets mirror in RTL layouts.

```cpp
#include <qtmaterial/widgets/data/qtmaterialdivider.h>
```

## List

`QtMaterialList` manages ordered `QtMaterialListItem` rows with no/single/multi-selection,
disabled-item skipping, Up/Down/Left/Right/Home/End navigation, Space/Return/Enter activation and
accessible container/item summaries.

## Grid List

`QtMaterialGridList` provides selectable grid items with title/supporting text, optional icons,
disabled state, no/single/multi-selection, directional/Home/End navigation, keyboard activation,
RTL-aware horizontal movement and deterministic cell sizing.

## Carousel

`QtMaterialCarousel` manages content-card items with current-item tracking, optional wrap-around,
configurable visible-item count, Left/Right/Home/End navigation, Space/Return/Enter activation,
RTL-aware placement and accessible current-item summaries. Rendering is driven by the resolved
`CarouselSpec`.

## Table

`QtMaterialTable` is the Material 3 wrapper around `QTableView`. Its contract covers
row-oriented selection by default, regular/dense density, accessible table/current-cell/row
summaries, plain Return/Enter/Space row activation and a stable `rowActivated(int)` signal.
Modifier-based selection and navigation are deliberately left to `QTableView`, so Shift/Ctrl
range/toggle selection, Home/End and page movement keep native desktop behavior.

The table intentionally keeps Qt's model/view architecture. Applications continue to provide a
`QAbstractItemModel`, optional `QSortFilterProxyModel` chains and custom delegates where
domain-specific rendering or editing is required. `inlineEditingEnabled` selects native
DoubleClick/F2 edit triggers; delegate commit/cancel semantics stay unchanged. Opt-in
`contextMenuEnabled` emits `contextMenuRequested(index, globalPosition)` and leaves creation,
lifetime and actions of the menu to the application.

For desktop workflows, `QtMaterialTable` also exposes explicit policies for column reordering,
cell-vs-row selection and native internal drag/drop. These policies configure the underlying
`QTableView` behavior rather than replacing Qt's model, delegate or MIME/drop contracts.
The 1.12 workspace payload persists column order, width, visibility, sort presentation and the
Material desktop policies. See [Desktop and productivity components](desktop-productivity.md)
for the wider desktop surface.
