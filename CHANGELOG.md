# Changelog

All notable changes to qt-material3-widget are documented here.

The project follows semantic versioning. Starting with 1.0, documented installed public
headers are source-compatible within the 1.x line; binary compatibility is best-effort
unless a stricter ABI policy is published.

## [Unreleased]

### Fixed
- Slider supports parent-only construction for the gallery, Designer plugin and generated `.ui` forms, while retaining explicit orientation construction.
- Progress reduced-motion tests qualify Theme types, and the Qt 5.14.2 Designer CI lane uses the base archive without the unavailable `qttools` add-on.
- Restore valid CI YAML after duplicate job definitions were embedded in the Windows Designer environment script.
- Command Palette equal-score provider results now use stable IDs and locale-independent section ordering, with deterministic duplicate ownership regardless of reply order.
- Navigation Rail and Menu expose individual accessible items, roles, geometry, selection/check states and activation actions, including focus and structure notifications.
- Date Picker weekday header now inherits the Material surface through the native `QPalette::AlternateBase` role, preventing a light strip in dark themes.
- Editable ComboBox labels now synchronize to the native editor without overwriting an application-provided accessible name.
- Autocomplete initializes and refreshes its accessible fallback when the placeholder changes.
- Search View and Date Range Picker now label their composed result/calendar surfaces more explicitly.
- Segmented Button now keeps its accessible description synchronized after programmatic segment, selection and mode changes.

### Added
- Designer 3.0 adds specialized color/token, enum, date/time and collection editors; token/default reset support through Designer's reset path; Compact/Medium/Expanded, DPR 1x/2x and LTR/RTL preview rendering; six theme presets; and promotes Adaptive Shell, Tooltip and Badge for a 35-control palette.
- Adaptive Shell exposes serializable `automaticDensity` and `supportingPaneWidth` properties for no-code Designer authoring.
- Native Qt button adaptation lets existing `QPushButton` instances opt into Text/Filled/FilledTonal/Outlined/Elevated Material rendering, density, effective ThemeContext updates, widget-tree migration and opt-out without replacing their Qt type or signal contract.
- Desktop Scale 2.0 adds million-row Table/Tree and proxy/lazy-model workloads, 100k Command Palette scale, theme/resize/surface/cache storms, p95/p99 budgets, long-run memory detection, per-commit performance history and scheduled Massif heap profiling.
- Designer 2.0 adds Material task-menu property authoring, reversible Light/Dark/Expressive previews and explicit Tabs/Adaptive Shell container extensions when Qt's Designer development API is available.
- The Designer palette grows to 32 persistence-safe controls, adding Split Button, Button Group, Navigation Bar, Loading Indicator and Segmented List with AUTOUIC round-trip coverage.
- Button Group, Navigation Bar and Segmented List expose serializable QStringList authoring properties for their labels.
- Desktop Productivity 1.12 hardens native Table/Tree keyboard selection, inline editing, proxy sort/filter compatibility, indexed context menus and drag/drop; adds fail-safe workspace persistence for Table, Tree View, Command Palette, Navigation Suite and Split View; and certifies exact split-size restore, large async/cancelled command providers and repeated Navigation Bar ↔ Rail state preservation.
- Expressive Catalogue 1.10 adds Split Button, Button Group, Floating Toolbar and morphing Loading Indicator widgets with keyboard/accessibility contracts and reduced-motion handling.
- Expressive Data 1.11 adds opt-in expressive List Item geometry, automatic Segmented List grouping, expressive Menu metrics and press/selected Chip shape morphing.
- Adaptive/Desktop 1.9 certification adds five-class LTR/RTL visual evidence, responsive Navigation Suite accessible focus, observable supporting-pane state, shell accessibility summaries, a native AT fixture and fail-closed promotion tooling.
- Missing Material 3 certification adds per-destination Navigation Bar accessibility, modal Side Sheet focus containment/restoration, a pinned 1.7 visual matrix with repeatability evidence, and fail-closed visual/NVDA/Orca/VoiceOver promotion tooling.
- Enterprise 1.5 closure now has a fail-closed NVDA/Orca/VoiceOver evidence ledger and release checker gate for the six remaining Navigation/Desktop components.
- Material 3 Expressive buttons add XS/S/M/L/XL sizing, Round/Square shape semantics and state-driven press/selected morphing through the shared button renderer.
- Expressive FAB adds Small/Standard/Medium/Large sizing while reusing the shared shape-morph and reduced-motion pipeline.
- Adaptive window size classes, Navigation Suite and Adaptive Shell add Compact/Medium/Expanded/Large/ExtraLarge behavior, responsive Bar/Rail navigation, supporting panes, RTL layout and automatic desktop density.
- Material 3 catalogue coverage adds Navigation Bar, Side Sheet, Tooltip and Badge with theme-aware rendering, focused tests, Gallery examples and 1.7 maturity tracking.
- The initial Designer foundation expanded the curated palette from 21 to 27 persistence-safe widgets, including Range Slider, Date Field, Search Bar, Top/Bottom App Bars and Divider.
- Outlined/Filled Text Fields expose their common authored values as Qt properties for direct editing and UIC serialization.
- A real `examples/designer-form/designerform.ui` AUTOUIC application demonstrates production `.ui` consumption.
- The `designer-dev` CMake preset configures, builds and runs focused Designer contracts.
- The Designer plugin installs as an explicit `Designer` component with dedicated package metadata.
- Navigation/Desktop certification suites cover focus traversal, keyboard-only operation, LTR/RTL and real QAccessible interfaces for Tabs, Navigation Rail, Menu, Breadcrumb, Command Palette and Split View.
- Command Palette stress coverage exercises 10,000 commands, rapid debounced/queued requests, cancellation and destruction, stale replies and deterministic ordering.
- Pinned navigation visual evidence covers 27 matrix/focus cases in two independent render passes, with renderer provenance and a repeatability report; visual approval and platform screen-reader review remain explicit maturity gaps.
- Command Palette 2.0 adds fuzzy search, sections, favorites/history, icons and secondary text, configurable Ctrl+K/Ctrl+P shortcuts, and application-owned synchronous/asynchronous providers with cancellation and stale-response rejection.
- Breadcrumb 2.0 adds editable locations, segment/overflow icons, bounded responsive labels and opt-in URL drag/drop, including a strict visible-item limit when the current segment is internal to the path.
- Split View 2.0 adds native constrained keyboard movement with Home/End and Enter, configurable reset sizes, extended collapsed-pane state persistence, remember-on-show behavior and opt-in animated collapse with constraint restoration.
- Material 3 Expressive foundation adds Standard/Expressive motion schemes, six semantic spatial/effects profiles, ThemeIO/reduced-motion integration and an internal cached shape-morphing primitive.
- Focused desktop-navigation and Expressive foundation test targets cover the new contracts; the gallery demonstrates providers, editable navigation and persistent animated split panes.
- Opt-in responsive Breadcrumb elision that moves hidden path ranges into the native overflow menu as the widget narrows.
- Breadcrumb overflow menus via `maximumVisibleItems`, Command Palette empty-state/shortcut rendering, and Split View pane constraints/reset behavior.
- Consolidated maturity evidence for Slider, Range Slider, Navigation, Table, Tree View, Pagination and Split View.
- Candidate visual matrices for Slider/Range Slider and Desktop Productivity controls.

- Shared non-slider Input-family maturity coverage for accessibility, keyboard behavior, RTL propagation and DPR 2.0 rendering.
- Selection and Input visual state matrices produce reviewable golden candidates for light, dark and high-contrast themes.
- Ubuntu/Fusion CI publishes family visual candidate goldens without weakening the existing stable release baselines.
- Shared Selection-family maturity coverage for accessibility, disabled keyboard behavior, RTL direction and DPR 2.0 rendering.

### Changed
- Standalone public List, Autocomplete, Date Picker and Menu widgets are now component-registry owned instead of being classified as support headers.
- API-freeze validation rejects duplicate widget-header ownership between the component registry and support-header allowlist.

## [1.0.0] - 2026-09-27

### Added
- Desktop productivity components add Tree View, Pagination, Split View, Breadcrumb and Command Palette while preserving native Qt Model/View and layout ownership.
- Table adds explicit column-reordering, cell-selection and internal drag/drop policies.
- API-freeze validation is integrated into the unified release checker and repository-health gate.
- Public widget-header ownership is explicit: component-registry entries plus a reviewed support-header allowlist.
- Release validation requires every retained C++ test source to be registered with CTest or explicitly classified as a standalone consumer harness.
- The first stable API signature baseline is checked in from Doxygen XML and restricted to the canonical installed-public-header manifest.
- Seven reviewed visual goldens cover deterministic token boards and representative component grids in the pinned Qt 6.4.0/Fusion release environment.

### Changed
- Component spec resolvers and render-only spec types are internalized instead of being installed as application API.
- `QtMaterialAutocompletePopup` is internalized as an implementation detail; applications use `QtMaterialAutocomplete` instead.
- Public widget inspection hooks for resolved runtime specs are removed; authored configuration remains public where applicable.
- Qt-major event compatibility details no longer leak through the public core headers.
- `QtMaterialInputControl` now exposes only the canonical form-field base contract; TextField vocabulary such as `labelText()` and `supportingText()` lives on the public TextField API.
- Supported `find_package(... COMPONENTS ...)` entry points are frozen to ThemeModel, ThemeIO, ThemeRuntime, Widgets and Integration.
- Remaining stateful public widgets use PIMPL where needed so implementation state is not part of the supported source surface.
- Native child-widget accessors retained for 1.x are explicitly documented as parent-owned extension points.
- Stable component-grid goldens use reduced-motion final states and are re-run with exact zero-pixel tolerance before release.

### Compatibility
- 1.0 establishes the first stable source-API baseline for documented installed public headers.
- Additive declarations are allowed within 1.x; removing or changing a baseline signature is treated as a breaking change by CI.
- Binary compatibility is best-effort for 1.x unless a stricter ABI policy is published.
- Minimum supported versions are Qt 5.14.2 for Qt 5 and Qt 6.4.0 for Qt 6.
- C++17 and CMake 3.21+ remain required.

## [0.8.0] - 2026-09-21

### Added
- Search Bar and Search View backed by Qt Model/View and QSortFilterProxyModel.
- Combo Box, Slider and Range Slider families for desktop form workflows.
- Time Field, Time Picker and Date Range Picker built from native Qt input primitives.
- Component-expansion release contract and CI gate covering the complete 0.8 scope.

### Changed
- Pre-1.0 cleanup removes repository scaffolding, forwarding headers, duplicate resolver/spec surfaces, placeholder tests, and accidental public APIs instead of deprecating them.
- The obsolete `CompactSpecResolver` duplicate is removed in favor of canonical `ChipSpecResolver`, and `DividerSpec` no longer requires a placeholder translation unit.
- Repository-only Tooling/Testing C++ modules, legacy qt-material XML compatibility, redundant playgrounds, migration-only documentation, and dead compatibility tests are removed before the first public release.
- Unfinished Material-conformance scaffolding and obsolete reference-model harnesses are removed; active visual regression remains the repository rendering guardrail.
- Theme JSON reading now accepts only the canonical formatVersion 1 source/resolved shape; unpublished pre-1.0 aliases and legacy layouts are removed.
- Spec resolution terminology replaces retired SpecFactory compatibility tests, migration scripts, and historical hardening checklists.
- Qt 5/Qt 6 adaptation is consolidated on the active core event compatibility layer; repository-only Tooling/Testing exports no longer leak into the public global header.
- The public CMake component list is reduced to application-facing ThemeModel, ThemeIO, ThemeRuntime, Widgets and Integration; internal dependency targets remain exported transitively.
- Internal theme-context glue, metadata names, ThemeIO codecs/token mappings and paint/shadow caches move to private headers and are removed from the installed API.
- Redundant `qtmaterialthemeidentity.h` is removed; Theme equality remains declared by `qtmaterialtheme.h`.
- `QtMaterialProxyStyle` moves from Core to the Integration module so the native Qt bridge has a single owner and Core no longer depends on Integration.
- Five overlapping theming examples are consolidated into one public workflow example.
- Chip is promoted from planned to usable and reuses the shared 0.7 ripple, state-layer and focus policies.
- Menu supports exclusive check groups for radio-style actions.
- Table exposes explicit multi-selection while preserving QTableView sorting, headers and Model/View behavior.
- Component registry expands from 32 to 40 tracked components.

### Compatibility
- Minimum supported Qt remains Qt 5.14.2.
- C++17 and CMake 3.21+ remain required.

## [0.7.0] - 2026-09-20

### Added
- Unified interaction-state certification for enabled, disabled, hover, focus, press, checked/selected, indeterminate, drag, error, and read-only states.
- Ripple origin/bounds/reduced-motion policies.
- Focus visibility, trapping, and restoration contracts for transient surfaces.
- Device-pixel-ratio-aware shadow cache keys and performance coverage.
- Central reduced-motion behavior for transition controllers and deterministic tests.
- Interaction/effects release checker and CI gate.

### Changed
- Shared effects consume the same interaction and accessibility policies instead of widget-local timing/painting conventions.
- Motion cancellation and end-state behavior are deterministic.
- Shadow and ripple benchmarks cover DPR and reduced-motion paths.

### Compatibility
- Minimum supported Qt remains Qt 5.14.2.
- C++17 and CMake 3.21+ remain required.

## [0.6.0] - 2026-09-20

### Added
- Frozen Theme JSON formatVersion 1 contract with deterministic serializer fixtures and strict validation.
- Token certification matrix for light/dark, contrast modes, theme variants, and color backends.
- Component-local override compatibility coverage, including opaque extension round-trips.
- System-theme and Theme Studio release gates.
- Runtime propagation and high-fanout theme benchmarks.

### Changed
- Theme Studio is promoted from a development skeleton to the supported theme authoring example.
- Theme JSON imports in Theme Studio use strict validation by default.
- Theme runtime contracts explicitly require no-op equality, single revision increments, and isolated ThemeContext instances.

### Compatibility
- Theme JSON formatVersion 1 is frozen for 0.6.x compatible additions.
- Minimum supported Qt remains Qt 5.14.2.
- C++17 and CMake 3.21+ remain required.

## [0.5.0] - 2026-09-20

### Added
- Release packaging through CPack with ZIP and TGZ install archives and source archives.
- A release-readiness contract that validates version surfaces, release-scoped component maturity,
  public package metadata, and required repository governance files.
- CI certification for release packages, examples/benchmarks, sanitizers, and cross-platform
  installed consumers.
- Focused maturity coverage for Tabs, Linear Progress Indicator, and Snackbar.

### Changed
- Release-scoped components must be at least `usable` before a 0.5.x release can be cut.
- Tabs keyboard navigation follows visual direction in right-to-left layouts.
- Installed packages include the license, README, and changelog.

### Compatibility
- Minimum supported Qt remains Qt 5.14.2.
- C++17 remains required.
- CMake 3.21+ remains required.
- Windows Qt 5.14.2 is certified with the MSVC v142 toolchain.

## [0.4.1] - 2026-09-19

### Changed
- Stabilized the component registry as the canonical maturity source.
- Hardened Qt 5.14.2, Qt 6, macOS, Windows, Linux, documentation, and consumer CI.
- Restored public-header and installed-package validation.
- Fixed cross-platform widget tests and several resolved-spec regressions.
