# Material 3 Expressive and adaptive desktop

QtMaterial3 exposes Material 3 Expressive as an additive, opt-in layer on top of
the stable 1.x widget API. Existing applications keep their current geometry and
rendering until Expressive is enabled on a button.

## Expressive buttons

All common button styles that use `QtMaterialTextButton` share the same
Expressive contract:

```cpp
QtMaterial::QtMaterialFilledButton action("Create");
action.setExpressive(true);
action.setExpressiveSize(QtMaterial::QtMaterialButtonSize::Large);
action.setExpressiveShape(QtMaterial::QtMaterialButtonShape::Round);
```

Available sizes are ExtraSmall, Small, Medium, Large and ExtraLarge. Their
container/touch metrics are:

| Size | Container | Minimum touch target | Horizontal padding | Icon |
| --- | ---: | ---: | ---: | ---: |
| XS | 32 | 48 | 12 | 20 |
| S | 40 | 48 | 16 | 20 |
| M | 56 | 56 | 24 | 24 |
| L | 96 | 96 | 48 | 32 |
| XL | 136 | 136 | 64 | 40 |

Round and Square are semantic shape choices rather than independent skins.
During press, and while a checkable button is selected, the container morphs
toward the opposite Expressive shape. The renderer uses the cached
`QtMaterialShapeMorph` primitive and the `SpatialFast` motion profile. Reduced
motion resolves directly to the final state.

For the full Expressive experience, use the Expressive motion scheme as well:

```cpp
QtMaterial::ThemeOptions options;
options.motionScheme = QtMaterial::MotionScheme::Expressive;
QtMaterial::Theme theme = QtMaterial::ThemeBuilder().build(options);
```

The Expressive color variant and Expressive motion/widget behavior remain
separate choices so applications can adopt them independently.

## Expressive FAB

`QtMaterialFab` adds a dedicated size scale:

```cpp
QtMaterial::QtMaterialFab fab;
fab.setExpressive(true);
fab.setFabSize(QtMaterial::QtMaterialFabSize::Medium);
fab.setExpressiveShape(QtMaterial::QtMaterialButtonShape::Round);
```

Small, Standard, Medium and Large FABs use 40, 56, 80 and 96 logical-pixel
containers respectively, while preserving at least a 48 logical-pixel touch
target. Press morphing uses the same shared shape/motion pipeline as common
buttons.

## Window size classes

Adaptive decisions are based on the current application window, not the physical
display. `WindowSizeClass::fromLogicalSize()` classifies Qt logical pixels using the
Material adaptive width breakpoints:

| Width | Class |
| --- | --- |
| < 600 | Compact |
| 600-839 | Medium |
| 840-1199 | Expanded |
| 1200-1599 | Large |
| >= 1600 | ExtraLarge |

Height classes are Compact below 480, Medium from 480 through 899, and Expanded
from 900 upward.

This makes resizing, split-screen, tiling and multi-monitor moves deterministic.

## Navigation Suite

`QtMaterialNavigationSuite` owns one destination model and selects its
presentation from the current width class:

- Compact: bottom Navigation Bar.
- Medium and wider: Navigation Rail.

Selection survives presentation changes. Pointer, keyboard, RTL, disabled-item
and accessibility behavior are implemented by the same widget rather than by
recreating navigation controls during resize. The accessibility surface remains
a `QAccessible::List` across Bar/Rail transitions; each destination stays a
`ListItem` with selected, disabled and focused state plus activation actions.
Focus changes notify the item-level accessibility interface, so a responsive
mode switch does not collapse screen-reader focus back to an opaque container.

```cpp
auto* navigation = shell.navigationSuite();
navigation->addDestination("Home", homeIcon);
navigation->addDestination("Search", searchIcon);
navigation->setCurrentIndex(0);
```

## Responsive application shell

`QtMaterialAdaptiveShell` provides a ready-to-use Qt Widgets application shell:

```cpp
auto* shell = new QtMaterial::QtMaterialAdaptiveShell;
shell->setContentWidget(mainPage);
shell->setSupportingWidget(inspector);
```

The shell continuously resolves the current window class, changes Navigation
Suite presentation and lays out an optional supporting pane. The supporting
pane appears from Expanded width when at least 320 logical pixels remain for the
main content. RTL places the rail and supporting pane on the corresponding
trailing/leading sides.

`supportingPaneVisible` exposes the resolved pane state and emits
`supportingPaneVisibleChanged` only when the effective composition changes.
`accessibilitySummary` reports the current width class, resolved density and
supporting-pane visibility, which makes resize/split-screen transitions
observable without inspecting private geometry.

Automatic density is enabled by default. It is a Qt desktop adaptation rather
than a new Material breakpoint contract:

- Compact -> Default density.
- Medium -> Comfortable density.
- Expanded/Large/ExtraLarge -> Compact desktop density.

The shell propagates the resolved density to descendant QtMaterial controls and
buttons. Call `setAutomaticDensity(false)` when an application wants to own
density explicitly; subsequent breakpoint changes still update
`resolvedDensity()` but do not overwrite application-owned child density.

## 1.9 certification

`tst_adaptive_shell` covers all five width classes, Navigation Bar/Rail
selection continuity, disabled-item skipping, per-destination accessible focus,
automatic-density opt-out, supporting-pane visibility and RTL composition.

The pinned visual lane renders 30 `adaptive_desktop_*` cases:
five width classes × LTR/RTL × light/dark/high-contrast. CI renders them twice
on Qt 6.4.0 / Fusion / xcb and rejects renderer, dimension or pixel drift.
Visual review and native NVDA/Orca/VoiceOver results are recorded in
`docs/components/adaptive-desktop-certification-1.9.json`; promotion remains
fail-closed until all evidence passes.

## References

The breakpoint model follows the Android adaptive window size class guidance and
the Navigation Suite behavior follows the Material adaptive navigation pattern.
Expressive button behavior follows the Material 3 Expressive common-button model:
five sizes, round/square shapes and state-driven shape changes.
