# Navigation

The navigation surface exposes:

- `QtMaterial::QtMaterialTabs`
- `QtMaterial::QtMaterialNavigationRail`
- `QtMaterialMenu`

Component maturity is tracked in `docs/components/component-registry.json`.

## Tabs

`QtMaterialTabs` is a themed `QTabWidget` integration with typed routes, stable per-tab metadata,
lazy page-content creation, badges, overflow handling, and synchronization with navigation
controllers.

### Spec flow

The widget keeps authored and resolved state separate:

- `spec()` and `authoredSpec()` expose the consumer-authored `TabsSpec`.
- theme resolution is an internal implementation boundary.

Rendering uses the internal resolved spec; `resolvedSpec()` is intentionally not part
of the installed 0.9 API candidate.

### Routes and synchronization

Each tab can expose:

- a stable tab ID;
- a test/automation ID;
- a normalized `QtMaterialRoute`;
- a lazy-loading factory;
- optional badge content.

`bindTo(QStackedWidget*)` synchronizes a stack directly. Multiple
`QtMaterialNavigationController` instances can also be bound bidirectionally.

### Keyboard contract

The tab bar uses `Qt::StrongFocus` and supports:

- `Home` / `End`;
- horizontal and vertical arrow navigation;
- disabled-tab skipping;
- optional wrap navigation;
- Ctrl+Tab / Ctrl+Shift+Tab style forward/backward navigation.

Horizontal arrows follow visual direction. In a right-to-left layout, Right moves toward the
visually right tab and Left moves toward the visually left tab rather than assuming LTR index
direction.

### Accessibility and automation

The internal tab bar has the stable object name `qtmaterial_tabs_bar` and accessible name
`Tabs`. Native tab labels remain available to the Qt accessibility layer.

Page widgets expose dynamic automation metadata:

- `materialTabId`
- `materialTabTestId`
- `materialTabRoute`
- `materialTabIndex`

### Maturity evidence

`tst_tabs` and the focused route/theme/lifecycle suites cover:

- insertion/removal metadata stability;
- authored-spec round-tripping and internal theme-resolution behavior;
- stacked-widget and controller synchronization;
- lazy loading;
- routes and URL navigation;
- LTR and RTL keyboard behavior;
- accessibility surface;
- DPR 2.0 render smoke.

The gallery navigation page includes a Tabs example with routes, IDs, and a badge.

`QtMaterialTabs` is tracked as **usable**. Reviewed visual-reference coverage and a broader
full state matrix remain before `complete` maturity.

## Navigation Rail

`QtMaterialNavigationRail` is tracked as **usable**. Destinations are ordered and addressable by
index; disabled destinations are skipped. Up/Down follow the vertical destination order while
Left/Right follow visual direction and therefore mirror in RTL layouts. Home/End jump to the
first/last enabled destination, and Space/Return/Enter activate the current destination.

The rail keeps the default accessible name `Navigation rail`, exposes a synchronized container
summary, and provides per-destination accessible text including position, selected state and
disabled state. Focused tests cover disabled-item skipping, activation, RTL directional keyboard
behavior and DPR 2.0 rendering.

## Menu

`QtMaterialMenu` supports action items and separators, disabled-item skipping, checkable and
exclusive items, shortcut labels, mouse activation, directional/Home/End keyboard navigation,
Space/Return/Enter activation, Escape dismissal, type-ahead selection and accessible summary
text.

The row painter mirrors check columns, icons, labels and shortcut text in RTL while preserving
logical item order and geometry. `tst_menu` certifies disabled/separator skipping, activation,
checkable state, Escape dismissal, accessibility summaries, type-ahead navigation, RTL layout
stability and DPR 2.0 rendering.

The advanced Navigation Gallery contains a dedicated Menu showcase with shortcut labels,
checkable state and a disabled action so the same interaction and RTL behavior can be inspected
without relying on another application example.
