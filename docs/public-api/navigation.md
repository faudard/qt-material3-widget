# Navigation

The 0.5.0 navigation surface currently exposes two release-scoped widgets:

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

### 0.5.0 maturity evidence

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

Tabs is therefore release-scoped as **usable** in 0.5.0. Reviewed visual-reference coverage and
a broader full state matrix remain before `complete` maturity.

## Navigation Rail

`QtMaterialNavigationRail` is release-scoped as **usable**. Its focused release contract covers
destination selection, disabled destinations, keyboard navigation, resolved specs, and
accessibility summaries.

See [Navigation Rail release readiness](navigationrail-release-readiness.md) for the detailed
contract.
