# qt-material3-widget — Future Roadmap and Current Status

Repository: <https://github.com/faudard/qt-material3-widget>

This document combines two prior responses:

1. A complete future roadmap for `faudard/qt-material3-widget`.
2. A status table of the project based on the public repository state reviewed at the time.

---

# Part 1 — Complete Future Roadmap

Below is a **proposed exhaustive future roadmap** for `faudard/qt-material3-widget`. It is based on the current repository state, not an official maintainer commitment.

## 1. Current baseline

The repository is already positioned as a **Material 3 widget toolkit for Qt Widgets**, with an explicit warning that it is still **experimental / design-in-progress**. Its core architectural rule is important: **widgets render from resolved specs, not ad hoc theme lookups**. That rule should stay the backbone of the whole roadmap.

The current module layout is already strong: `qtmaterial3_theme`, `qtmaterial3_legacy`, `qtmaterial3_core`, `qtmaterial3_specs`, `qtmaterial3_effects`, and `qtmaterial3_widgets`. The README also names the current focus areas: packaging/install/export, accessibility and keyboard correctness, performance and memory review, public API stabilization, documentation, and release readiness.

The CMake project is currently versioned as **0.4.0**, targets Qt 5.14+ or Qt 6, uses C++17, exposes CMake options for tests/examples/benchmarks, and already defines install/export rules for the library targets.

The public widget surface already includes buttons, selection controls, surfaces, inputs, lists/dividers, tabs, snackbars, progress indicators, and several spec classes. The roadmap should therefore focus less on “starting the library” and more on **stabilizing, completing, testing, documenting, and releasing it**.

---

## 2. Strategic goal

The future target should be:

> Make `qt-material3-widget` the reference Material 3 implementation for classic Qt Widgets applications: typed, themeable, testable, accessible, installable, documented, and stable enough for real desktop applications.

That means the project should become:

1. **A real C++ library**, not a demo.
2. **A Material 3 design-system implementation**, not only painted widgets.
3. **A stable Qt Widgets API**, not a moving experiment.
4. **A consumable package**, not only a source checkout.
5. **A testable UI toolkit**, with deterministic specs and automation hooks.
6. **A bridge for existing `qt-material-widgets` users**, but not permanently bound to the old API.

---

## 3. Release roadmap

## Phase 0 — Repository audit and stabilization branch

**Goal:** Freeze the current architecture and identify what is real, skeletal, experimental, or deprecated.

Deliverables:

| Area | Work |
|---|---|
| API inventory | List every public header, class, enum, property, signal, slot, and exported symbol. |
| Implementation status | Mark each widget as `complete`, `usable`, `partial`, `skeleton`, or `planned`. |
| Spec status | Verify every widget has a matching immutable spec and factory path. |
| Docs status | Classify docs as complete, stub, obsolete, or missing. |
| Test status | Map every widget/spec/effect/theme object to test coverage. |
| Compatibility status | Verify Qt 5.14, latest Qt 5, Qt 6 LTS, and latest Qt 6 builds. |
| Roadmap artifacts | Add `ROADMAP.md`, `STATUS.md`, `CONTRIBUTING.md`, `CHANGELOG.md`, `SECURITY.md`, and a real `LICENSE`. |

The missing license is urgent: the README currently says “Add the project license here,” and GitHub also shows no releases published. That blocks serious adoption.

Recommended branch policy:

```text
main          = stable development branch
next          = integration branch for large API/component changes
release/x.y   = release stabilization branches
```

---

## Phase 1 — 0.5.0 “Foundation Release”

**Goal:** Make the library buildable, installable, and usable by external consumers.

### 1.1 CMake and packaging

Current CMake already has install/export logic and CMake package files, so the next step is hardening rather than invention.

Deliverables:

- Validate `find_package(QtMaterial3Widgets REQUIRED)` from a clean external project.
- Test static and shared builds.
- Add `BUILD_SHARED_LIBS` support.
- Add symbol visibility and export macro audit.
- Add Windows DLL export/import validation.
- Add macOS install name/RPATH validation.
- Add Linux `DESTDIR` install validation.
- Add `CPack` packages:
  - `.zip`
  - `.tar.gz`
  - `.deb`
  - optional `.rpm`
  - optional NSIS or WiX installer for Windows.
- Add `pkg-config` file only if useful for non-CMake consumers.
- Add `FetchContent` consumption example.
- Add `CPM.cmake` example.
- Add `vcpkg` port plan.
- Add `Conan` recipe plan.

### 1.2 CI matrix

The repository already has `.github/workflows/ci.yml` and `docs.yml`, so the roadmap should expand those workflows.

Minimum CI matrix:

| OS | Qt versions | Compiler |
|---|---|---|
| Ubuntu | Qt 5.15, Qt 6 LTS, latest Qt 6 | GCC, Clang |
| Windows | Qt 6 LTS, latest Qt 6 | MSVC |
| macOS | Qt 6 LTS, latest Qt 6 | AppleClang |

CI jobs:

- Configure/build.
- Unit tests.
- Widget tests.
- Install/export consumer test.
- Examples build.
- Benchmarks build, not necessarily run.
- Docs build.
- clang-format check.
- clang-tidy optional.
- Sanitizers on Linux:
  - AddressSanitizer
  - UndefinedBehaviorSanitizer
  - optional LeakSanitizer.

### 1.3 External consumer guarantee

Examples already include `minimal-consumer` and `install-consumer`, which is good.

Add acceptance tests:

```text
tests/consumer/
  build-tree-consumer/
  install-tree-consumer/
  fetchcontent-consumer/
  shared-library-consumer/
  static-library-consumer/
```

Success criteria for 0.5.0:

- Clean build on Linux, Windows, macOS.
- External app can consume installed package.
- No missing public headers.
- No unresolved exported symbols.
- Examples build.
- License added.
- Changelog started.

---

## Phase 2 — 0.6.0 “Theme and Token Release”

**Goal:** Make theming the strongest and most stable subsystem.

The theming layer already includes `ThemeOptions`, `ThemeBuilder`, `ThemeSerializer`, and `ThemeManager`; the docs describe deterministic construction, JSON serialization, runtime theme application, seed color workflows, and light/dark generation.

### 2.1 Token model completion

Complete and freeze:

- `ColorScheme`
- `TypographyScale`
- `ShapeScale`
- `ElevationScale`
- `MotionTokens`
- `StateLayer`
- density tokens
- icon size tokens
- component-local token overrides.

### 2.2 Material color utilities backend

Current CMake has `QTMATERIAL3_USE_MCU`, so this should become a documented, tested backend choice.

Deliverables:

- Document what happens when MCU is enabled.
- Document fallback behavior when MCU is disabled.
- Add deterministic golden tests for seed color generation.
- Add contrast-mode tests.
- Add dark/light parity tests.
- Add regression snapshots for known seed colors.

### 2.3 JSON theme schema

Add:

```text
docs/schema/theme-v1.schema.json
docs/schema/theme-v2.schema.json
```

Support:

- `formatVersion`
- source seed color
- resolved color scheme
- typography scale
- shape scale
- elevation scale
- motion tokens
- density
- custom component overrides
- metadata:
  - generator version
  - library version
  - Qt version
  - timestamp optional.

Add strict read modes:

```cpp
enum class ThemeReadMode {
    Lenient,
    Strict,
    UpgradeIfPossible
};
```

### 2.4 Theme editor / playground maturity

The README already lists `theme-playground`; make it a serious developer tool.

Features:

- Seed color picker.
- Light/dark toggle.
- Contrast mode toggle.
- Typography preview.
- Shape preview.
- Elevation preview.
- State layer preview.
- Component preview grid.
- Export JSON.
- Import JSON.
- Copy C++ snippet.
- Copy CSS-like token report.
- Screenshot export.

### 2.5 System theme integration

Add optional helpers:

- Detect system light/dark mode.
- Reapply theme when OS appearance changes.
- Respect platform font where appropriate.
- Expose high-contrast mode if available.
- Add “follow system” mode.

Proposed API:

```cpp
enum class ThemeMode {
    Light,
    Dark,
    System
};
```

---

## Phase 3 — 0.7.0 “Core Interaction and Effects Release”

**Goal:** Make every widget feel consistent.

Current effects include ripple, state-layer painter, focus indicator, shadow renderer/cache, transition controller, and scrim widget.

### 3.1 Interaction states

Every interactive widget should support:

- enabled
- disabled
- hovered
- focused
- pressed
- checked/selected
- indeterminate where applicable
- dragged where applicable
- error where applicable
- read-only where applicable.

Add a common state test matrix:

```text
normal
hovered
focused
pressed
disabled
checked
checked + hovered
checked + focused
checked + disabled
error
error + focused
```

### 3.2 Ripple

Make ripple behavior consistent:

- bounded ripple
- unbounded ripple
- center-origin ripple
- pointer-origin ripple
- disabled ripple suppression
- high-DPI correctness
- reduced-motion mode.

### 3.3 Focus indication

Qt accessibility guidance emphasizes keyboard navigation and not relying only on mouse/touch input. The library should treat focus rings and keyboard behavior as first-class correctness, not cosmetic polish.

Deliverables:

- Consistent focus ring.
- Tab-order tests.
- Space/Enter activation.
- Escape handling for transient surfaces.
- Arrow-key navigation for menus, tabs, lists, segmented controls.
- Focus trapping for dialogs and modal bottom sheets.
- Focus restoration after transient surfaces close.

### 3.4 Shadows and elevation

Deliverables:

- Elevation tokens mapped to shadow presets.
- Cached shadow rendering.
- Device-pixel-ratio-aware shadow pixmaps.
- Benchmarks for shadow generation.
- Avoid unnecessary repaint on hover/focus if geometry unchanged.

Qt’s high-DPI model uses device-independent pixels and device pixel ratio mapping, so custom painting and pixmap caches must explicitly avoid blurry or incorrectly scaled assets.

### 3.5 Motion

Deliverables:

- Central motion token access.
- Standard durations.
- Standard easing curves.
- Reduced-motion mode.
- Transition cancellation behavior.
- Tests for animation end states.
- No animation in headless deterministic tests unless explicitly enabled.

---

## Phase 4 — 0.8.0 “Primary Widget Families Release”

**Goal:** Bring all already-public widget families to production quality.

## 4.1 Buttons

The button docs currently expose:

- `QtMaterialTextButton`
- `QtMaterialFilledButton`
- `QtMaterialFilledTonalButton`
- `QtMaterialOutlinedButton`
- `QtMaterialElevatedButton`
- `QtMaterialIconButton`
- `QtMaterialFab`
- `QtMaterialExtendedFab`

Future work:

| Widget | Roadmap |
|---|---|
| Text button | Finalize baseline, icon+text layout, density, mnemonic support. |
| Filled button | Finalize default action role, disabled colors, loading state. |
| Filled tonal button | Verify tonal roles and contrast. |
| Outlined button | Stroke token, focus stroke, error/destructive variant optional. |
| Elevated button | Elevation transition on hover/focus/press. |
| Icon button | Standard, filled, filled tonal, outlined variants. |
| FAB | Small, regular, large variants. |
| Extended FAB | Icon+label, collapse/expand behavior. |

Add optional future APIs:

```cpp
setLeadingIcon(QIcon)
setTrailingIcon(QIcon)
setLoading(bool)
setDestructive(bool)
setDensity(QtMaterialDensity)
setButtonRole(ButtonRole)
```

## 4.2 Selection controls

The public selection family currently includes checkbox, radio button, and switch.

Future work:

| Widget | Roadmap |
|---|---|
| Checkbox | checked, unchecked, indeterminate, error, disabled, label support. |
| Radio button | exclusive groups, keyboard arrow navigation, label hit area. |
| Switch | icons optional, disabled state, touch-friendly hit target. |

Add:

- `QtMaterialCheckboxGroup`
- `QtMaterialRadioGroup`
- form validation integration
- accessible names/descriptions
- label-click behavior.

## 4.3 Inputs

The input docs currently list outlined text field, filled text field, autocomplete popup, and date field.

Future work:

| Widget | Roadmap |
|---|---|
| Outlined text field | floating label, helper text, error text, prefix/suffix, leading/trailing icons. |
| Filled text field | same feature parity as outlined. |
| Autocomplete popup | model support, async provider, filtering, keyboard navigation. |
| Date field | parser/formatter, locale support, min/max date, date picker integration. |

Add future widgets:

- `QtMaterialSearchBar`
- `QtMaterialSearchView`
- `QtMaterialComboBox`
- `QtMaterialSpinBox`
- `QtMaterialSlider`
- `QtMaterialRangeSlider`
- `QtMaterialTimeField`
- `QtMaterialDatePicker`
- `QtMaterialTimePicker`
- `QtMaterialDateRangePicker`.

Material 3 officially includes date pickers for selecting dates or ranges, menus for temporary choice lists, and text-field-related interactions, so these are logical future targets.

## 4.4 Surfaces

The current surface family includes dialog, navigation drawer, bottom sheet, banner, card, and app bars.

Future work:

| Widget | Roadmap |
|---|---|
| Dialog | modal/non-modal, buttons area, focus trap, escape behavior, accessibility. |
| Navigation drawer | modal, standard, dismissible, permanent variants. |
| Bottom sheet | modal and standard variants, drag handle, scrim, focus behavior. |
| Banner | action layout, multiline text, severity variants. |
| Card | elevated, filled, outlined; clickable card; media/header/actions areas. |
| Top app bar | center-aligned, small, medium, large variants. |
| Bottom app bar | FAB notch optional, navigation/action slots. |
| Snackbar | queueing, action button, timeout policy, host placement. |

Material 3 describes navigation drawers as a way to switch between app views on larger devices, and snackbars as short process updates at the bottom of the screen, so those semantics should be reflected in widget behavior and docs.

## 4.5 Navigation

The navigation docs already describe a corrected `QtMaterialTabs` design with themed states, overflow menu, lazy loading, routes, URL navigation, automation test IDs, and synchronization through `QtMaterialNavigationController`.

Future work:

- Finalize `QtMaterialTabs`.
- Add primary and secondary tabs.
- Add scrollable tabs.
- Add fixed tabs.
- Add badges.
- Add route synchronization tests.
- Add `QStackedWidget` integration example.
- Add `QtMaterialNavigationRail`.
- Add `QtMaterialNavigationBar`.
- Add `QtMaterialMenu`.
- Add `QtMaterialMenuItem`.
- Add `QtMaterialBreadcrumbs` only if aligned with Qt desktop conventions.
- Add command-palette/search-navigation example.

## 4.6 Data widgets

The current data widget docs are minimal and say table, grid list, and carousel APIs are introduced in Phase 6, while `QtMaterialDivider` already exists.

Future work:

| Widget | Priority | Notes |
|---|---:|---|
| Divider | P0 | Finish orientation, inset, thickness, color token. |
| List item | P0 | One-line/two-line/three-line, leading/trailing slots. |
| List | P0 | Model-backed list, selection, keyboard navigation. |
| Table | P1 | Material-styled `QTableView` wrapper/delegate. |
| Data table | P1 | Header, sorting, selection, pagination. |
| Grid list | P2 | Responsive layout, image/content slots. |
| Carousel | P2 | Optional; desktop relevance lower. |
| Tree view | P2 | Qt desktop users will expect this more than Material docs do. |

For Qt Widgets users, data widgets are strategically important because many desktop apps are table/tree-heavy.

## 4.7 Compact controls

The current compact-controls doc only says chips and segmented controls live there.

Future work:

| Widget | Variants |
|---|---|
| Chip | assist, filter, input, suggestion |
| Chip set | single-select, multi-select, removable chips |
| Segmented button | single-select, multi-select |
| Badge | small dot, numeric, label badge |
| Tooltip | plain and rich tooltip |
| Avatar | image, initials, icon fallback |

Material 3 groups buttons broadly, including segmented buttons alongside elevated, filled, tonal, outlined, text, icon, and FAB-style buttons, so segmented controls belong near the button/compact-control boundary.

---

# 5. API stabilization roadmap

## 5.1 Naming policy

Keep canonical names:

```cpp
QtMaterialFilledButton
QtMaterialOutlinedTextField
QtMaterialNavigationDrawer
QtMaterialLinearProgressIndicator
```

Avoid legacy names as primary public API.

Use legacy aliases only in `qtmaterial3_legacy`.

## 5.2 Namespace policy

Current docs use `QtMaterial::...`. Keep that stable.

Recommended:

```cpp
namespace QtMaterial {
    class QtMaterialFilledButton;
}
```

Avoid:

```cpp
namespace QtMaterial3
namespace Material
namespace M3
```

unless there is a very strong reason.

## 5.3 Property policy

Every user-facing configurable value should be a Qt property where practical:

```cpp
Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
Q_PROPERTY(QtMaterial::Density density READ density WRITE setDensity NOTIFY densityChanged)
```

This helps:

- Qt Designer
- bindings
- test automation
- property animation
- introspection.

## 5.4 Signal policy

Every state mutation should be observable:

```cpp
themeChanged()
densityChanged()
specChanged()
resolvedSpecChanged()
checkedChanged(bool)
currentIndexChanged(int)
routeChanged(QtMaterialRoute)
```

## 5.5 ABI policy

Before 1.0:

- API may break between minor versions.
- ABI may break between minor versions.
- Document breaking changes in `CHANGELOG.md`.

At 1.0:

- Source compatibility within 1.x.
- ABI compatibility only if the project is ready to commit to it.
- If ABI stability is not guaranteed, say so clearly.

Recommended stance:

```text
1.x guarantees source compatibility for public headers.
Binary compatibility is best-effort unless otherwise stated.
```

## 5.6 Deprecation policy

Add macros:

```cpp
QTMATERIAL3_DEPRECATED
QTMATERIAL3_DEPRECATED_X("message")
```

Deprecation lifecycle:

```text
N     = introduce replacement
N+1   = warn
N+2   = remove, only before 1.0
1.x   = keep deprecated API until next major
```

---

# 6. Testing roadmap

The repository already has test folders for consumer, core, effects, legacy, render, specs, theme, and widgets.

## 6.1 Unit tests

Required coverage:

| Area | Tests |
|---|---|
| Theme | seed generation, JSON, mode switching, observer notification. |
| Specs | default values, resolution, immutability, equality. |
| Core | density, interaction state, focus, event handling. |
| Effects | ripple, state layer, focus indicator, shadows, transitions. |
| Widgets | size hints, painting smoke tests, state transitions, accessibility metadata. |
| Packaging | build-tree and install-tree consumers. |

## 6.2 Golden rendering tests

Add image-based tests carefully:

```text
tests/render/golden/
  linux/
  windows/
  macos/
```

Rules:

- Use stable fonts where possible.
- Account for platform differences.
- Test geometry and tokens more than exact antialiasing.
- Store hashes only if deterministic.
- Prefer semantic render assertions where possible.

## 6.3 Accessibility tests

Qt accessibility guidance highlights scalable UI, keyboard navigation, color contrast, and assistive tool metadata. These should become hard quality gates.

Tests:

- Accessible name present.
- Accessible role correct.
- Accessible description optional but supported.
- Keyboard activation works.
- Tab order works.
- Focus visible.
- Label association works.
- No color-only information.
- Minimum contrast checks for generated schemes.

## 6.4 Automation hooks

The tabs docs already introduce stable automation properties such as `materialTabId`, `materialTabTestId`, `materialTabRoute`, and `materialTabIndex`. Generalize this pattern.

Add common dynamic properties:

```text
materialComponent
materialVariant
materialRole
materialTestId
materialState
```

This is especially valuable for your broader Robot Framework / QAT / SikuliX Qt testing work.

## 6.5 Performance tests

The repository already has benchmark folders for effects, theme, and widgets.

Benchmarks should cover:

- theme build from seed
- JSON import/export
- spec resolution
- button paint
- text field paint
- shadow cache hit/miss
- ripple animation overhead
- list with many items
- table delegate painting
- app-wide theme switch time
- memory usage of common widget trees.

---

# 7. Documentation roadmap

The docs currently include public API guide pages and generated C++ API reference, but the generated reference is described as only lightly annotated and currently more of a structural API map until richer Doxygen comments are added.

## 7.1 Documentation hierarchy

Create:

```text
docs/
  getting-started.md
  installation.md
  quickstart.md
  theming.md
  architecture.md
  widget-gallery.md
  accessibility.md
  testing.md
  migration.md
  contributing.md
  release-process.md
  public-api/
  api/
```

## 7.2 Per-widget docs template

Every widget page should include:

```text
# QtMaterialFilledButton

Purpose
Material 3 mapping
Header
CMake target
Basic example
Properties
Signals
States
Theme tokens consumed
Spec type
Accessibility behavior
Keyboard behavior
Known limitations
Screenshots
```

The button docs already say generated API pages should answer what visual role the widget represents, what content it accepts, what states it supports, what tokens it consumes, and how behavior differs from Qt base classes. Reuse that as the global documentation standard.

## 7.3 Architecture docs

Add detailed pages for:

- Why specs exist.
- Authored spec vs resolved spec.
- Theme resolution pipeline.
- Widget paint pipeline.
- Interaction state pipeline.
- Accessibility integration.
- Effects pipeline.
- Legacy bridge.
- Install/export architecture.

## 7.4 Migration docs

Migration from `qt-material-widgets` should include:

| Old concept | New concept |
|---|---|
| legacy theme lookup | typed theme roles |
| ad hoc widget painting | resolved specs |
| old component names | Material 3 canonical names |
| qmake project | CMake package |
| manual copy source | installed package / `find_package` |

The README already states that the project is moving toward typed Material 3 roles, layered architecture, spec-driven rendering, canonical Material 3 widget names, and a temporary legacy alias bridge.

---

# 8. Component coverage roadmap

## Tier P0 — Must be solid before 1.0

- Theme system.
- Buttons.
- Text fields.
- Checkbox.
- Radio button.
- Switch.
- Dialog.
- Card.
- Snackbar.
- Progress indicators.
- Divider.
- List item.
- List.
- Tabs.
- Navigation drawer.
- Top app bar.
- Bottom app bar.
- Basic menu.
- Accessibility.
- Keyboard navigation.
- CMake packaging.
- Examples.
- Documentation.

## Tier P1 — Strongly recommended for 1.0 or 1.1

- Chips.
- Segmented buttons.
- Slider.
- Range slider.
- Search bar.
- Search view.
- Combo box.
- Date picker.
- Time picker.
- Tooltip.
- Badge.
- Navigation rail.
- Navigation bar.
- Data table.
- Table delegate styling.
- Form layout helpers.
- Theme editor.

## Tier P2 — Useful after the core is mature

- Carousel.
- Grid list.
- Tree view styling.
- Calendar view.
- Wizard/stepper.
- Toast-style non-Material helper, if desired.
- Split button.
- Command palette.
- Rich menu sections.
- Avatar.
- Skeleton/loading placeholder.
- Empty-state panel.
- Preference/settings widgets.

## Tier P3 — Experimental / optional

- Qt Designer plugin.
- Figma token importer.
- Material Theme Builder importer.
- QML bridge.
- Python bindings.
- Web documentation gallery with live screenshots.
- Visual regression dashboard.
- Runtime inspector overlay.

---

# 9. Accessibility roadmap

Accessibility must be a release blocker, not a later polish item.

## 9.1 Minimum accessibility contract

Every widget must define:

- accessible role
- accessible name
- accessible description where useful
- keyboard behavior
- focus behavior
- visible focus indicator
- contrast-safe colors
- disabled-state semantics
- screen-reader-friendly state text
- no information conveyed by color only.

## 9.2 Widget-specific requirements

| Widget | Accessibility requirement |
|---|---|
| Button | role, name, pressed/default state where relevant. |
| Icon button | must require accessible label if no visible text. |
| Checkbox | checked/unchecked/partially checked. |
| Radio | group semantics. |
| Switch | on/off state. |
| Text field | label, helper text, error text. |
| Dialog | focus trap, title, escape behavior. |
| Snackbar | polite announcement, action focusable. |
| Tabs | tab role, selected state, arrow navigation. |
| Navigation drawer | landmark-like behavior where possible. |
| Progress | determinate/indeterminate state and value. |

## 9.3 Accessibility tooling

Add examples:

```text
examples/accessibility/
  keyboard-navigation/
  screen-reader-labels/
  high-contrast/
  reduced-motion/
```

---

# 10. Performance and memory roadmap

## 10.1 Paint performance

Rules:

- No theme lookup inside hot paint loops unless cached.
- Resolve specs before paint.
- Cache paths where geometry unchanged.
- Cache shadows.
- Avoid heap allocation during paint.
- Avoid unnecessary `update()`.
- Avoid full-widget repaint when small regions suffice.

## 10.2 Theme switch performance

Target:

```text
Small app: instant
Medium app: < 16 ms if possible
Large app: bounded and measurable
```

Add benchmark:

```cpp
ThemeManager::setTheme(theme)
```

on widget trees of:

- 100 widgets
- 1,000 widgets
- 10,000 widgets.

## 10.3 Memory

Measure:

- per-widget overhead
- per-spec overhead
- shadow cache size
- icon cache size
- theme object size
- cost of dynamic properties
- cost of animations.

## 10.4 High-DPI correctness

Qt automatically handles many high-DPI cases for widgets and painting, but custom image/pixmap caches must still account for device pixel ratio.

Tests:

```bash
QT_SCALE_FACTOR=1
QT_SCALE_FACTOR=1.25
QT_SCALE_FACTOR=1.5
QT_SCALE_FACTOR=2
```

---

# 11. Developer experience roadmap

## 11.1 Examples

Current examples include gallery, minimal consumer, install consumer, theme playground, core playground, and forms.

Future examples:

```text
examples/
  gallery/
  minimal-consumer/
  install-consumer/
  fetchcontent-consumer/
  theme-playground/
  forms/
  navigation-shell/
  settings-page/
  data-table/
  accessibility/
  high-dpi/
  migration-from-qt-material-widgets/
```

## 11.2 Gallery app

The gallery should become the visual contract for the library.

Features:

- left navigation
- theme switcher
- density switcher
- component state matrix
- copy C++ example
- inspect resolved spec
- inspect theme tokens
- accessibility labels preview
- screenshot export.

## 11.3 Debug tooling

Add:

```cpp
QtMaterialDebug::dumpTheme()
QtMaterialDebug::dumpResolvedSpec(widget)
QtMaterialDebug::installPaintDebugOverlay()
QtMaterialDebug::setShowFocusRects(bool)
QtMaterialDebug::setShowHitRects(bool)
```

## 11.4 Logging

Add categories:

```cpp
Q_LOGGING_CATEGORY(qtmaterialTheme)
Q_LOGGING_CATEGORY(qtmaterialSpecs)
Q_LOGGING_CATEGORY(qtmaterialEffects)
Q_LOGGING_CATEGORY(qtmaterialWidgets)
Q_LOGGING_CATEGORY(qtmaterialAccessibility)
```

---

# 12. Governance roadmap

## 12.1 Project files

Add:

```text
LICENSE
CODE_OF_CONDUCT.md
CONTRIBUTING.md
SECURITY.md
SUPPORT.md
ROADMAP.md
STATUS.md
CHANGELOG.md
RELEASE.md
```

## 12.2 Issue labels

Recommended labels:

```text
type: bug
type: feature
type: docs
type: test
type: refactor
type: accessibility
type: performance
type: packaging
type: api
type: component
component: theme
component: core
component: specs
component: effects
component: widgets
priority: p0
priority: p1
priority: p2
good first issue
help wanted
breaking-change
```

## 12.3 Contribution rules

Require every component PR to include:

- public header
- implementation
- spec
- theme token mapping
- tests
- docs
- gallery entry
- accessibility notes
- keyboard behavior notes
- screenshot if visual.

## 12.4 Definition of done

For a widget to be “complete”:

```text
[ ] public header installed
[ ] CMake target includes source/header
[ ] spec exists
[ ] default spec factory path exists
[ ] theme resolution works
[ ] sizeHint/minimumSizeHint correct
[ ] paint path high-DPI safe
[ ] keyboard behavior implemented
[ ] accessibility metadata implemented
[ ] tests added
[ ] docs added
[ ] gallery demo added
[ ] migration notes added if replacing legacy widget
```

---

# 13. 1.0 release criteria

Do not release 1.0 until all of these are true:

## API

- Public API reviewed.
- Namespaces final.
- Header layout final.
- Deprecated legacy aliases separated.
- Source compatibility policy written.
- Binary compatibility policy written.

## Components

- P0 components complete.
- Component state matrix tested.
- Core examples complete.
- Gallery complete enough to demonstrate all P0 widgets.

## Theming

- Seed color generation stable.
- JSON import/export stable.
- Light/dark mode stable.
- Theme switching stable.
- Token docs complete.

## Accessibility

- Keyboard navigation complete.
- Focus visuals complete.
- Screen-reader metadata implemented.
- Contrast checks pass.

## Packaging

- `find_package` works.
- Install/export works.
- CI validates external consumer.
- Release archives produced.
- License present.

## Quality

- CI green on supported platforms.
- Tests documented.
- Benchmarks available.
- Sanitizer pass on Linux.
- No known P0 bugs.
- No skeleton public classes marked stable.

## Documentation

- Getting started.
- Installation.
- Theming.
- Component docs.
- Architecture.
- Migration.
- API reference.
- Examples.

---

# 14. Suggested version plan

| Version | Theme |
|---|---|
| 0.4.x | Current architecture cleanup, license, status inventory. |
| 0.5.0 | Packaging, install/export, CI, consumer readiness. |
| 0.6.0 | Theme/token stabilization, JSON schema, theme playground. |
| 0.7.0 | Core interaction, effects, accessibility, keyboard correctness. |
| 0.8.0 | P0 widget completion. |
| 0.9.0 | Beta: API freeze candidate, docs, gallery, migration guide. |
| 1.0.0 | Stable source release. |
| 1.1.x | Chips, segmented buttons, sliders, menus, search. |
| 1.2.x | Date/time pickers, navigation rail/bar, richer form controls. |
| 1.3.x | Data table, table/tree delegates, larger desktop-app support. |
| 2.0.0 | Only for major API/ABI redesign if necessary. |

---

# 15. Highest-priority next actions

Do these first:

1. Add a real license.
2. Add `ROADMAP.md` and `STATUS.md`.
3. Classify every public class as stable, experimental, partial, or planned.
4. Make install/export tests mandatory in CI.
5. Expand CI matrix.
6. Freeze the theme API.
7. Finish Doxygen comments for all public headers.
8. Build the gallery into the canonical visual test app.
9. Make accessibility/keyboard behavior a release gate.
10. Cut a `0.5.0` release once packaging and consumer usage are reliable.

The repository already has the right architectural direction. The future work is mainly about turning that architecture into a reliable product: **complete components, stable API, verified accessibility, reproducible packaging, strong examples, and disciplined releases**.

---

# Part 2 — Project Status Matrix

Below is a **status table of the whole project**, based on the public repository state I checked. I am using **“present”** to mean the class/module is in the source/header/CMake tree, **not** that it is final or production-stable. The repository itself still says it is **experimental / design-in-progress**, with both working building blocks and skeletal scaffolding.

## Legend

| Status | Meaning |
|---|---|
| ✅ Present | Exists in source/header/CMake and is part of the current tree. |
| 🧪 Present, experimental | Exists, but should still be treated as unstable / under API review. |
| 🟡 Partial | Exists, but docs/tests/completeness are not yet enough for stable release. |
| 📄 Documented plan | Mentioned in docs/README as planned, but not implemented in the current CMake widget list. |
| ❌ Missing / gap | Important project item not yet done. |
| ❔ Unknown | Cannot be proven from public repo metadata without deeper implementation audit. |

---

# 1. Global project status

| Area | Current status | Evidence / notes |
|---|---:|---|
| Project maturity | 🧪 Experimental | README explicitly marks the project “experimental / design-in-progress.” |
| Current version | 🧪 0.4.0 | CMake project version is `0.4.0`. |
| Qt support | ✅ Present | CMake requires Qt Core/Gui/Widgets and supports Qt 5 or Qt 6 discovery. |
| C++ standard | ✅ Present | Targets use C++17. |
| Main architecture | ✅ Present | README defines the layered architecture: theme, legacy, core, specs, effects, widgets. |
| Core design rule | ✅ Present | README states widgets render from resolved specs, not ad hoc theme lookups. |
| Public release | ❌ Missing | GitHub shows no releases published. |
| License | ❌ Missing / urgent | README still says “Add the project license here.” |
| Packages | ❌ Missing / unknown | GitHub package area shows no usable package data in the public page. |
| Repository activity | ✅ Present | Repository shows 76 commits on the public page. |

---

# 2. Build, packaging, CI, tests, examples

| Area | Current status | What exists now | Next status target |
|---|---:|---|---|
| CMake root build | ✅ Present | `cmake_minimum_required(VERSION 3.21)`, Qt discovery, build options, shared libraries. | Harden across OS/compiler matrix. |
| Build options | ✅ Present | Tests, examples, benchmarks, and Material Color Utilities backend are exposed as CMake options. | Add documented option matrix. |
| Install/export | ✅ Present | Installs all library targets, headers, exported targets, config file, version file, and macros. | Validate with CI on all platforms. |
| Consumer CMake package | ✅ Present | README shows `find_package(QtMaterial3Widgets REQUIRED)` usage. | Make consumer test mandatory in CI. |
| Installed-package consumer test | ✅ Present | `tst_installed_package_consumer` exists in tests CMake. | Run in CI, all platforms. |
| CI | 🟡 Skeleton | Current workflow is only a placeholder echo: “Phase 5 CI skeleton.” | Real Linux/Windows/macOS Qt matrix. |
| Unit/widget tests | ✅ Present | Tests cover theme, core, specs, effects, buttons, selection, surfaces, inputs, data, render, snackbar, consumer install. | Add accessibility, keyboard, CI gates. |
| Render tests | 🟡 Partial | Some render tests exist for filled/outlined buttons and text fields. | Add golden/state matrix coverage. |
| Benchmarks | ✅ Present | Benchmarks exist for theme switching, ripple, and widget creation. | Expand to painting, shadow cache, large widget trees. |
| Examples | ✅ Present | Theme playground, core playground, gallery; consumer examples are optional. | Expand gallery into canonical visual test app. |

---

# 3. Module status

| Module | Current status | Source/header evidence | Test/docs status |
|---|---:|---|---|
| `qtmaterial3_theme` | 🟡 Present, important but still stabilizing | Contains color scheme, theme options, theme, builder, manager, serializer, observer, typography, shape, elevation, motion, state layer. | Theme guide is relatively detailed; tests exist for builder, manager, serializer. |
| `qtmaterial3_legacy` | 🟡 Present bridge | Contains legacy alias map and legacy theme bridge. | Migration doc says legacy bridge is transitional. |
| `qtmaterial3_core` | 🟡 Present foundation | Contains widget base, interaction state, control, surface, overlay surface, abstract button, selection control, input control, accessibility helper. | Tests exist for interaction state, abstract button state, input control. |
| `qtmaterial3_specs` | 🟡 Present foundation | Contains specs for buttons, icon button, checkbox, radio, switch, dialog, text field, FAB, card, navigation drawer, bottom sheet, banner, app bars, list item, divider, autocomplete, date field, tabs, snackbar, progress. | Spec factory and phase specs tests exist. |
| `qtmaterial3_effects` | 🟡 Present foundation | Ripple controller, state layer painter, focus indicator, shadow renderer/cache, transition controller, scrim widget. | Tests exist for state layer, ripple, focus, shadow, transition, scrim. |
| `qtmaterial3_widgets` | 🟡 Present but experimental | Many widget classes are in the CMake source/header lists. | Broad tests exist, but docs are uneven and project remains experimental. |

---

# 4. Theme/token status

| Item | Status | Notes |
|---|---:|---|
| `ColorScheme` | ✅ Present | Source/header included in theme module. |
| `ThemeOptions` | ✅ Present | Used as explicit theme build input in guide. |
| `Theme` | ✅ Present | Resolved theme object described in guide. |
| `ThemeBuilder` | ✅ Present | Guide documents build-from-options and seed-color workflows. |
| `ThemeManager` | ✅ Present | Guide documents runtime theme state, seed application, import/export. |
| `ThemeSerializer` | ✅ Present | Guide documents JSON/file serialization and deserialization. |
| `TypographyScale` | ✅ Present | In theme module. |
| `ShapeScale` | ✅ Present | In theme module. |
| `ElevationScale` | ✅ Present | In theme module. |
| `MotionTokens` | ✅ Present | In theme module. |
| `StateLayer` | ✅ Present | In theme module. |
| Material Color Utilities backend option | ✅ Present | `QTMATERIAL3_USE_MCU` exists. |
| JSON schema documentation | ❌ Missing / next step | The theming guide itself says a JSON schema note is a suggested next documentation improvement. |
| System theme integration | ❔ Unknown / not visible | I did not see public evidence of a complete follow-system light/dark integration. |
| Theme playground | ✅ Present | Included as an example. |

---

# 5. Core infrastructure status

| Item | Status | Notes |
|---|---:|---|
| `QtMaterialWidget` | ✅ Present | Core source/header listed. |
| `QtMaterialInteractionState` | ✅ Present | Core source/header listed and tested. |
| `QtMaterialControl` | ✅ Present | Core source/header listed. |
| `QtMaterialSurface` | ✅ Present | Core source/header listed. |
| `QtMaterialOverlaySurface` | 🟡 Present but test disabled | Source/header listed; related test appears commented out. |
| `QtMaterialAbstractButton` | ✅ Present | Source/header listed and state test exists. |
| `QtMaterialSelectionControl` | 🟡 Present but layout test disabled | Source/header listed; selection-control layout test appears commented out. |
| `QtMaterialInputControl` | ✅ Present | Source/header listed and test exists. |
| Accessibility helper | 🟡 Present foundation | Private helper exists; accessibility/keyboard rules doc exists. |
| Accessibility test gate | ❌ Missing / planned | Tests CMake includes a note to add accessibility and keyboard checks. |

---

# 6. Effects status

| Effect | Status | Tests |
|---|---:|---|
| Ripple controller | ✅ Present | Test exists. |
| State layer painter | ✅ Present | Test exists. |
| Focus indicator | ✅ Present | Test exists. |
| Shadow renderer | ✅ Present | Test exists. |
| Shadow cache | ✅ Present | Source/header present; covered indirectly or needs explicit audit. |
| Transition controller | ✅ Present | Test exists. |
| Scrim widget | ✅ Present | Test exists. |

---

# 7. Widget family status

## Buttons

| Widget | Source/header | Test | Docs | Status |
|---|---:|---:|---:|---|
| `QtMaterialTextButton` | ✅ | ✅ | ✅ Family docs | 🟡 Present, experimental |
| `QtMaterialFilledButton` | ✅ | ✅ | ✅ Family docs | 🟡 Present, experimental |
| `QtMaterialFilledTonalButton` | ✅ | ✅ | ✅ Family docs | 🟡 Present, experimental |
| `QtMaterialOutlinedButton` | ✅ | ✅ | ✅ Family docs | 🟡 Present, experimental |
| `QtMaterialElevatedButton` | ✅ | ✅ | ✅ Family docs | 🟡 Present, experimental |
| `QtMaterialIconButton` | ✅ | ✅ | ✅ Family docs | 🟡 Present, experimental |
| `QtMaterialFab` | ✅ | ✅ | ✅ Family docs | 🟡 Present, experimental |
| `QtMaterialExtendedFab` | ✅ | ✅ | ✅ Family docs | 🟡 Present, experimental |

Button source/header files are listed in the widgets target, and tests exist for all eight button classes. The button docs also identify the family and explain what still needs better Doxygen documentation.

## Selection controls

| Widget | Source/header | Test | Docs | Status |
|---|---:|---:|---:|---|
| `QtMaterialCheckbox` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialRadioButton` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialSwitch` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |

Selection controls are listed in the widgets target and in tests; the public docs currently only list the family/classes briefly.

## Inputs

| Widget | Source/header | Test | Docs | Status |
|---|---:|---:|---:|---|
| `QtMaterialOutlinedTextField` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialFilledTextField` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialAutocompletePopup` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialDateField` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| Full `DatePicker` | ❌ | ❌ | 📄 Planned | 📄 Future advanced input |
| Full `TimePicker` | ❌ | ❌ | ❌ Not visible | Future roadmap item, not current repo item |

Input widgets are present in source/header/test lists, but the public input doc is currently only a one-line family listing. README separately says advanced inputs such as `DatePicker` and autocomplete maturity are planned.

## Surfaces

| Widget | Source/header | Test | Docs | Status |
|---|---:|---:|---:|---|
| `QtMaterialDialog` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialNavigationDrawer` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialBottomSheet` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialBanner` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialCard` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialTopAppBar` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialBottomAppBar` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialSnackbar` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialSnackbarHost` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |

Surface widgets are in the widgets target and tests list. The public surfaces doc currently names the family but does not yet provide full per-widget behavior docs.

## Navigation

| Widget / item | Source/header | Test | Docs | Status |
|---|---:|---:|---:|---|
| `QtMaterialTabs` | ✅ | ❌ Not visible in test list | 🟡 Docs conflict / sparse | 🟡 Present but needs audit |
| `QtMaterialNavigationController` | 🟡 Header listed | ❌ Not visible | 🟡 Sparse | 🟡 Public header, implementation status needs audit |
| `NavigationRail` | ❌ | ❌ | 📄 Planned | 📄 Future Phase 6 item |
| `Menu` | ❌ | ❌ | 📄 Planned | 📄 Future Phase 6 item |

`QtMaterialTabs` is in the widget source/header list, but I did not see a corresponding `tst_tabs` entry in the current tests CMake. The navigation doc currently says Phase 6 navigation widgets include navigation rail, menu, and related item models.

## Data widgets

| Widget / item | Source/header | Test | Docs | Status |
|---|---:|---:|---:|---|
| `QtMaterialListItem` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialList` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `QtMaterialDivider` | ✅ | ✅ | 🟡 Sparse | 🟡 Present, experimental |
| `Table` | ❌ | ❌ | 📄 Planned | 📄 Future Phase 6 item |
| `GridList` | ❌ | ❌ | 📄 Planned | 📄 Future Phase 6 item |
| `Carousel` | ❌ | ❌ | 📄 Planned | 📄 Future Phase 6 item |

List item, list, and divider are present in CMake and tests. The public data-widget doc says table, grid list, and carousel APIs are introduced in Phase 6.

## Progress indicators

| Widget | Source/header | Test | Docs | Status |
|---|---:|---:|---:|---|
| `QtMaterialLinearProgressIndicator` | ✅ | ❌ Not visible in test list | ❌ Not visible | 🟡 Present, needs tests/docs |
| `QtMaterialCircularProgressIndicator` | ✅ | ❌ Not visible in test list | ❌ Not visible | 🟡 Present, needs tests/docs |

The progress indicator source/header files and spec are listed in CMake, but I did not see dedicated progress-indicator tests in the current test list.

## Compact controls

| Widget / item | Source/header | Test | Docs | Status |
|---|---:|---:|---:|---|
| Chips | ❌ | ❌ | 📄 Planned | 📄 Future item |
| Segmented controls | ❌ | ❌ | 📄 Planned | 📄 Future item |

The compact-controls doc says chips and segmented controls “live here,” but I did not see chip or segmented-button source/header entries in the current widgets CMake list.

---

# 8. Spec coverage status

| Spec | Status |
|---|---:|
| Button spec | ✅ Present |
| Icon button spec | ✅ Present |
| Checkbox spec | ✅ Present |
| Radio button spec | ✅ Present |
| Switch spec | ✅ Present |
| Dialog spec | ✅ Present |
| Text field spec | ✅ Present |
| FAB spec | ✅ Present |
| Card spec | ✅ Present |
| Navigation drawer spec | ✅ Present |
| Bottom sheet spec | ✅ Present |
| Banner spec | ✅ Present |
| App bar spec | ✅ Present |
| List item spec | ✅ Present |
| Divider spec | ✅ Present |
| Autocomplete popup spec | ✅ Present |
| Date field spec | ✅ Present |
| Tabs spec | ✅ Present |
| Snackbar spec | ✅ Present |
| Progress indicator spec | ✅ Present |
| Chip spec | ❌ Not visible |
| Segmented button spec | ❌ Not visible |
| Table spec | ❌ Not visible |
| Grid list spec | ❌ Not visible |
| Carousel spec | ❌ Not visible |
| Menu spec | ❌ Not visible |
| Navigation rail spec | ❌ Not visible |

The visible spec list is taken from the CMake `QTMATERIAL3_SPECS_SOURCES` and `QTMATERIAL3_SPECS_HEADERS` sections.

---

# 9. Documentation status

| Documentation area | Status | Notes |
|---|---:|---|
| Public API guide index | ✅ Present | Lists theming, core infrastructure, widget families, and include style. |
| Theming guide | ✅ Strongest current guide | Covers theme options, builder, manager, serializer, JSON import/export, workflows. |
| Buttons guide | 🟡 Partial but useful | Lists button family and explains next Doxygen/doc work. |
| Inputs guide | 🟡 Very sparse | One-line family listing. |
| Selection guide | 🟡 Very sparse | One-line family listing. |
| Surfaces guide | 🟡 Very sparse | One-line family listing. |
| Navigation guide | 🟡 Planning note | Mentions Phase 6 navigation widgets. |
| Compact controls guide | 🟡 Planning note | Mentions chips and segmented controls. |
| Data widgets guide | 🟡 Planning note | Mentions table, grid list, carousel in Phase 6. |
| Generated C++ API reference | 🟡 Structural map | Docs say headers are lightly annotated and API pages should be treated as a structural map until richer Doxygen comments are added. |
| Architecture docs | 🟡 Present but thin | Architecture overview states the layered rule; advanced components page says Phase 6 extends architecture. |
| Accessibility/keyboard rules | ✅ Present | Defines minimum keyboard and accessibility rules. |
| Testing rules | 🟡 Present but brief | Lists unit, widget behavior, render snapshot, consumer build, benchmark layers. |
| Release process | 🟡 Present but brief | Lists release ladder and release gates. |
| Migration from old library | 🟡 Present but brief | Notes typed roles, layered architecture, specs, canonical names, legacy bridge. |

---

# 10. Release-readiness table

| Requirement for serious preview release | Current status | Priority |
|---|---:|---:|
| Add real license | ❌ Missing | P0 |
| Publish first GitHub release | ❌ Missing | P0 |
| Replace placeholder CI with real CI | ❌ Missing | P0 |
| Validate install/export on CI | 🟡 Test exists, CI not real | P0 |
| Stabilize public API names | 🟡 In progress | P0 |
| Add per-widget docs | 🟡 Very uneven | P0 |
| Add full Doxygen comments | 🟡 Lightly annotated only | P0 |
| Add accessibility tests | ❌ Planned but not present | P0 |
| Add keyboard behavior tests | ❌ Planned but not present | P0 |
| Add visual state matrix tests | 🟡 Partial render tests | P1 |
| Add benchmark thresholds | 🟡 Benchmarks exist | P1 |
| Add JSON theme schema | ❌ Missing | P1 |
| Complete planned Phase 6 widgets | 📄 Planned | P2 |
| Qt Designer plugin | ❌ Not visible | P3 |
| Python bindings | ❌ Not visible | P3 |

---

# 11. Condensed executive status

| Category | Status |
|---|---:|
| Architecture | ✅ Good foundation |
| Theme system | 🟡 Usable foundation, still needs schema/API hardening |
| Core base classes | 🟡 Present, needs deeper accessibility/keyboard validation |
| Specs | ✅ Broadly present |
| Effects | ✅ Broadly present |
| Buttons | 🟡 Broadly present and tested, still experimental |
| Selection controls | 🟡 Present and tested, docs thin |
| Inputs | 🟡 Present and tested, docs thin |
| Surfaces | 🟡 Present and tested, docs thin |
| Data widgets | 🟡 Basic list/divider present; table/grid/carousel planned |
| Navigation | 🟡 Tabs present; rail/menu planned |
| Compact controls | 📄 Planned |
| Progress indicators | 🟡 Source/header present; tests/docs need work |
| Tests | ✅ Broad test tree, but missing accessibility/keyboard gates |
| Examples | ✅ Present |
| Benchmarks | ✅ Present |
| Packaging | 🟡 CMake install/export present; release validation needed |
| CI | ❌ Placeholder only |
| Documentation | 🟡 Present but uneven |
| Release readiness | ❌ Not ready for stable release |
| License/release | ❌ Biggest immediate blocker |

Practical assessment: the repository is **past pure scaffolding** because many widgets, specs, tests, examples, install rules, and benchmarks already exist. But it is **not yet release-grade** because the license is missing, CI is placeholder-only, docs are uneven, and the project itself still declares experimental status.

