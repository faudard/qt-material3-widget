# Component expansion

Version 0.8 makes the library useful for broader desktop application workflows without
introducing a second interaction or theming stack. New components reuse Qt's Model/View,
input, focus and accessibility primitives and the common 0.6/0.7 infrastructure.

## Chip family

`QtMaterialChip` is the canonical implementation for Assist, Filter, Input and
Suggestion variants. Filter chips are checkable, Input chips can expose a removable
trailing affordance, and all variants share the common interaction-state, ripple,
focus and reduced-motion behavior.

## Search and choice

`QtMaterialSearchBar` owns the query input and clear/search actions.
`QtMaterialSearchView` composes a search bar, `QSortFilterProxyModel` and
`QListView`; applications retain ownership of their source model.
`QtMaterialComboBox` remains a `QComboBox`, preserving the normal Qt model,
delegate and editable-combo contracts.

## Slider family

`QtMaterialSlider` keeps the complete `QSlider` API and adds a value-label policy.
`QtMaterialRangeSlider` provides lower/upper values, mouse dragging, keyboard
adjustment, horizontal/vertical orientation and RTL-aware horizontal mapping.

## Date and time

`QtMaterialTimeField` is a keyboard-friendly `QTimeEdit`.
`QtMaterialTimePicker` provides a modal picker using the same field.
`QtMaterialDateRangePicker` composes two existing `QtMaterialDatePicker` instances
and guarantees `startDate <= endDate`.

The pre-existing DatePicker remains the canonical calendar implementation; the old
inputs compatibility header is not a second implementation.

## Menu and desktop data

The existing Material Menu contract includes disabled items, separators, shortcuts,
checkable state and keyboard navigation. Table, GridList and Carousel remain based on
Qt Model/View and are part of the 0.8 desktop-heavy maturity pass. Table sorting,
selection, dense rows and resizable columns remain native Qt behaviors.

## Release requirements

Every component introduced into the 0.8 release scope has a public header, production
source, registered test target, documentation, registry entry and at least usable
maturity. Qt 5.14.2 remains the minimum supported Qt version.
