# Component expansion

Version 0.8 makes the library useful for broader desktop application workflows without
introducing a second interaction or theming stack. New components reuse Qt's Model/View,
input, focus and accessibility primitives and the common 0.6/0.7 infrastructure.

## Chip family

`QtMaterialChip` is the canonical implementation for Assist, Filter, Input and
Suggestion variants. Filter chips are checkable, Input chips can expose a removable
trailing affordance, and all variants share the common interaction-state, ripple,
focus and reduced-motion behavior. Filter chips toggle through the native Space/Return button
contract. A removable chip emits `removeRequested()` from its trailing affordance and also from
Delete or Backspace while focused; disabled chips suppress keyboard removal.

## Search and choice

`QtMaterialSearchBar` owns the query input and clear/search actions.
`QtMaterialSearchView` composes a search bar, `QSortFilterProxyModel` and
`QListView`; applications retain ownership of their source model.
`QtMaterialComboBox` remains a `QComboBox`, preserving the normal Qt model,
delegate and editable-combo contracts. Its default rendering is Material-aware: the
closed control paints the themed container, outline, focus state and trailing chevron,
while the popup uses a themed item delegate with distinct selected and hover state
layers. Selection remains visually stronger than hover, and the chevron reflects the
open/closed popup state.

The combo participates in `ThemeContext` inheritance and resolves its visual state
through the internal autocomplete-spec resolution boundary. Replacing the native item
delegate remains supported and intentionally replaces the built-in Material popup-item
rendering.

### Native Qt extension points

The following composed-widget accessors are intentionally retained as stable
Qt-native extension points for 1.x: `QtMaterialSearchBar::lineEdit()`,
`QtMaterialSearchView::searchBar()`, `QtMaterialSearchView::resultView()`,
`QtMaterialDateRangePicker::startPicker()` / `endPicker()`, and
`QtMaterialTimePicker::timeField()`. The returned child objects remain owned
by their parent QtMaterial widget; callers may configure documented Qt behavior
but must not delete or reparent them.

Text-field and Autocomplete `lineEdit()` accessors are retained for the same
reason: validators, input methods, completers, selection, and other native Qt
editor integration remain valid application extension points.

## Slider family

`QtMaterialSlider` keeps the complete `QSlider` API and adds a value-label policy.
`QtMaterialRangeSlider` provides lower/upper values, mouse dragging, keyboard
adjustment, horizontal/vertical orientation and RTL-aware horizontal mapping.

The focused `tst_slider_maturity` suite certifies accessible value summaries,
two-handle keyboard traversal for Range Slider, Home/End bounds, LTR/RTL directional
behavior and desktop rendering at 100%, 125%, 150%, 175% and 200% scale equivalents. The visual-regression harness additionally emits
`input_slider_matrix_*` candidates covering horizontal, vertical, disabled and RTL
states in the controlled light/dark/high-contrast theme set.

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

## Input maturity evidence

The non-slider Input family shares a dedicated maturity contract in
`tst_input_maturity`. It covers accessible naming of composed native controls,
representative keyboard behavior, RTL propagation and DPR 2.0 rendering smoke for
TextField, ComboBox, Autocomplete, Search Bar/View, Date Field/Picker/Range Picker and
Time Field/Picker. Focused component tests remain responsible for deeper validation,
popup and parsing behavior.

## Release requirements

Every component introduced into the 0.8 release scope has a public header, production
source, registered test target, documentation, registry entry and at least usable
maturity. Qt 5.14.2 remains the minimum supported Qt version.
