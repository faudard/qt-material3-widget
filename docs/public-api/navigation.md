# Navigation

The navigation surface exposes:

- `QtMaterial::QtMaterialTabs`
- `QtMaterial::QtMaterialNavigationBar`
- `QtMaterial::QtMaterialNavigationRail`
- `QtMaterialMenu`

Component maturity is tracked in `docs/components/component-registry.json`.

The [Navigation/Desktop certification guide](navigation-desktop-certification.md) follows
Tabs, Rail, Menu, Breadcrumb, Command Palette and SplitView in that order. It records the
registered keyboard/accessibility contracts and the pending visual/platform review.

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
- desktop render smoke at 100%, 125%, 150%, 175% and 200% scale equivalents.

The gallery navigation page includes a Tabs example with routes, IDs, and a badge.

`QtMaterialTabs` is tracked as **usable**. Reviewed visual-reference coverage and a broader
full state matrix remain before `complete` maturity.

## Navigation Bar

`QtMaterialNavigationBar` implements the Material 3 bottom navigation pattern for a compact
set of primary destinations. Destinations expose label, icon and enabled state; the selected
destination is highlighted with the theme's secondary-container roles while the bar itself uses
the surface-container role. Applications can hide labels for compact presentations without
changing logical destination order.

The keyboard contract uses Left/Right, Home/End and Space/Return/Enter. Horizontal traversal
follows visual direction, so Left/Right mirror under RTL while Home/End continue to address the
first and last enabled logical destinations. Disabled destinations are skipped. The widget keeps
an accessible container name and synchronized selection summary, and the focused 1.7 test covers
selection, disabled-state fallback, keyboard activation, RTL direction and DPR 2.0 rendering.

The initial 1.7 entry is tracked as **usable**. Reviewed deterministic navigation goldens and
platform screen-reader traversal remain before release-scope promotion and `complete` maturity.

## Navigation Rail

`QtMaterialNavigationRail` is tracked as **usable**. Destinations are ordered and addressable by
index; disabled destinations are skipped. Up/Down follow the vertical destination order while
Left/Right follow visual direction and therefore mirror in RTL layouts. Home/End jump to the
first/last enabled destination, and Space/Return/Enter activate the current destination.

The rail keeps the default accessible name `Navigation rail`, exposes a synchronized container
summary, and provides per-destination accessible text including position, selected state and
disabled state. Focused tests cover disabled-item skipping, activation, RTL directional keyboard
behavior and desktop rendering at 100%, 125%, 150%, 175% and 200% scale equivalents.

## Menu

`QtMaterialMenu` supports action items and separators, disabled-item skipping, checkable and
exclusive items, shortcut labels, mouse activation, directional/Home/End keyboard navigation,
Space/Return/Enter activation, Escape dismissal, type-ahead selection and accessible summary
text.

The row painter mirrors check columns, icons, labels and shortcut text in RTL while preserving
logical item order and geometry. `tst_menu` certifies disabled/separator skipping, activation,
checkable state, Escape dismissal, accessibility summaries, type-ahead navigation, RTL layout
stability and desktop rendering at 100%, 125%, 150%, 175% and 200% scale equivalents.

The advanced Navigation Gallery contains a dedicated Menu showcase with shortcut labels,
checkable state and a disabled action so the same interaction and RTL behavior can be inspected
without relying on another application example.

## Breadcrumb overflow

`QtMaterialBreadcrumb::maximumVisibleItems` bounds the number of visible path segments without
changing the logical path. Hidden ranges collapse into an accessible `…` button backed by a
native `QMenu`; choosing a hidden segment updates `currentIndex` and emits the same
`activated(index, text)` signal as a visible segment. The current segment is always retained
when a visibility limit is active, along with the path endpoints where the limit permits.

`responsiveElisionEnabled` is an additive opt-in for width-driven overflow. When enabled,
Breadcrumb estimates the visible path capacity from the current widget width and typography,
then reduces the visible segment count only when the existing layout no longer fits. Resizing
back to a wider surface restores segments automatically. It is disabled by default so existing
1.x applications retain their current non-eliding layout unless they opt in.

## Command Palette desktop states

`QtMaterialCommandPalette` exposes a configurable `emptyStateText` and a public
`ShortcutRole` model role. The result delegate renders shortcut text on the trailing side,
mirroring correctly under RTL, while the empty-state label replaces the result list when the
proxy model has no matches. Applications continue to own the source model; the extra role is
purely presentational and does not alter filtering or activation semantics.
