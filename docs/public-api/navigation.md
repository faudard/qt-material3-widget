# Navigation

Phase 6 navigation widgets now include a proposed `QtMaterialTabs` implementation aligned with the repository architecture.

## Why this corrected iteration exists

The earlier experimental Tabs patches were useful to validate feature scope, but they had three structural weaknesses:

- metadata was stored by raw tab index in maps, which is fragile after `insertTab()` / `removeTab()`
- the theme application path mutated the authored spec instead of resolving into a separate runtime spec
- generic QObject binding was flexible, but too weakly typed for a library that is trying to stabilize its public API

This corrected patch fixes those points while preserving the recent features:

- Material 3 themed states
- overflow menu
- lazy loading
- routes and URL navigation
- test IDs for automation
- multi-widget synchronization through navigation controllers

## Public types

### `TabsSpec`

`TabsSpec` remains a simple value type in `qtmaterial3_specs`.

It covers:

- variant
- density
- alignment
- overflow mode
- color roles
- state-layer opacities
- sizing and animation values
- scroll and wrap-navigation behavior

### `QtMaterialRoute`

`QtMaterialRoute` is a small typed wrapper over a normalized route path.

Example:

```cpp
QtMaterialRoute(QStringLiteral("settings/profile"))
```

### `QtMaterialNavigationController`

Instead of binding Tabs directly to arbitrary QObjects, this patch introduces a narrow controller contract:

```cpp
class QtMaterialNavigationController : public QObject {
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
};
```

That makes multi-widget synchronization explicit and testable.

A convenience adapter for `QStackedWidget` is included:

```cpp
QtMaterialStackedWidgetController
```

## `QtMaterialTabs`

### Spec flow

The widget now keeps two specs:

- `authoredSpec()` — exactly what the consumer provided
- `resolvedSpec()` — runtime spec after theme resolution

That matches the repository rule:

> widgets render from resolved specs, not from ad hoc theme lookups

### Metadata model

All per-tab metadata now lives in a descriptor vector synchronized with the real tab order.

Stored per tab:

- `tabId`
- `tabTestId`
- `route`
- lazy-loading `factory`
- `loaded` state
- badge content and visibility

This avoids index/key drift after insertions and removals.

### Automation surface

Automation data is exposed through dynamic properties on each page widget:

- `materialTabId`
- `materialTabTestId`
- `materialTabRoute`
- `materialTabIndex`

The tab bar also gets a stable object name:

```cpp
qtmaterial_tabs_bar
```

This is a better split than overloading accessibility text for test automation.

## Lazy loading

Lazy loading no longer assumes the tab page itself does not exist.

Instead:

- the tab page exists as a normal `QWidget`
- the factory creates the page content on first activation
- content is inserted into the page layout when needed

That integrates cleanly with `QTabWidget`.

## URL navigation

`navigateToUrl()` now resolves both host and path:

```cpp
app://settings/profile
```

becomes:

```text
settings/profile
```

This avoids the common bug where only the trailing path segment is used.

## Integration checklist

The patch archive contains:

- `include/qtmaterial/specs/qtmaterialtabsspec.h`
- `include/qtmaterial/widgets/navigation/qtmaterialnavigationcontroller.h`
- `include/qtmaterial/widgets/navigation/qtmaterialtabs.h`
- `src/specs/qtmaterialtabsspec.cpp`
- `src/widgets/navigation/qtmaterialtabs.cpp`
- `tests/widgets/navigation/tst_tabs.cpp`

To integrate into the repository:

1. add the new spec source/header to `qtmaterial3_specs`
2. add the new widget source/header to `qtmaterial3_widgets`
3. add `tst_tabs` to `tests/CMakeLists.txt`

The provided script patches those files automatically.
