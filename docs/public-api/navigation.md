# Navigation

The navigation surface exposes:

- `QtMaterial::QtMaterialTabs`
- `QtMaterial::QtMaterialNavigationRail`

Component maturity is tracked in `docs/components/component-registry.json`.

## Tabs

`QtMaterialTabs` is a themed `QTabWidget` integration with typed routes, stable per-tab metadata,
lazy page-content creation, badges, overflow handling, and synchronization with navigation
controllers.

### Spec flow

The widget keeps authored and resolved state separate:

- `authoredSpec()` is the consumer-provided `TabsSpec`.
- `resolvedSpec()` is the runtime spec after theme resolution.

Rendering uses the resolved spec.

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
- authored/resolved spec separation;
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
index; disabled destinations are skipped. Up/Left and Down/Right navigate enabled destinations,
Home/End jump to the boundary, and Space/Return/Enter activate the current destination. The rail
keeps a default accessible name and exposes per-destination accessible summaries.

## Menu

`QtMaterialMenu` supports action items and separators, disabled-item skipping, checkable items,
mouse activation, directional/Home/End keyboard navigation, Space/Return/Enter activation,
Escape dismissal, RTL-aware layout and accessible summary text.
