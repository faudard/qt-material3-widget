# Data widgets

Table, grid list, and carousel APIs are introduced in Phase 6.


## QtMaterialDivider

`QtMaterialDivider` provides a lightweight Material 3 separator widget for use in
layouts, forms, drawers, menus, and list compositions.

### Header

```cpp
#include <qtmaterial/widgets/data/qtmaterialdivider.h>

## Table release contract

`QtMaterialTable` is the Material 3 table wrapper around `QTableView`.
The release-ready contract covers:

- row-oriented selection by default;
- dense and regular density;
- accessible table and current-cell summaries;
- row summaries that include column headers when available;
- keyboard activation with Return, Enter, or Space;
- stable `rowActivated(int)` emission for form/navigation flows.

The table intentionally keeps Qt's model/view architecture. Applications should
continue to provide a `QAbstractItemModel`, optional sorting/filtering proxies,
and custom delegates where domain-specific rendering is required.

