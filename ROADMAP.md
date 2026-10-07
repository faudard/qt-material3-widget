# Roadmap

The canonical component maturity source is `docs/components/component-registry.json`.
This roadmap defines release-level goals; it does not override the registry.

## 0.5.0 — Foundation Release

Goal: make qt-material3-widget a reproducibly buildable, installable and externally consumable
Qt Widgets library.

Release gates:

- Windows Qt 5.14.2 / MSVC v142 builds and tests.
- Windows Qt 6, Linux Qt 6 and macOS Qt 6 build and test.
- Shared and static source consumption.
- `add_subdirectory`, FetchContent and installed-package consumers.
- Public-header, architecture and ABI audits.
- Documentation builds without unapproved warnings.
- Examples and benchmarks compile.
- ASan/UBSan validation on Linux.
- CPack ZIP and TGZ install packages plus source archives.
- Every release-scoped component is at least `usable`.
- Tabs, Linear Progress Indicator and Snackbar are promoted from `partial` only with executable
  maturity evidence.

## 0.6.0 — Theme and Tokens

Goal: freeze the theme persistence contract and certify token generation, overrides,
system appearance integration, authoring tooling, and runtime propagation.

Release gates:

- Freeze Theme JSON `formatVersion: 1` and keep schema, serializer, documentation, and fixtures aligned.
- Strict reads reject unsupported versions, missing required blocks, invalid block types, and unknown contract fields.
- Lenient reads retain forward-compatible root data handling without weakening malformed-input checks.
- Theme → JSON → Theme round-trips are semantically lossless and byte-deterministic for identical themes.
- Opaque third-party component override names survive import/export without entering ThemeModel string APIs.
- Token matrices cover Light/Dark, Standard/Medium/High contrast, TonalSpot/Expressive, fallback, and MCU when available.
- Component-local override precedence and canonical ComponentId serialization are executable contracts.
- SystemTheme supports explicit Light/Dark, FollowSystem, platform-font application, safe Qt 5.14.2 fallback,
  and native Qt 6 color-scheme/high-contrast observation when available.
- Theme Studio provides strict JSON import/export, validation diagnostics, backend/variant controls,
  live compare, and reset/apply workflows.
- ThemeContext and ThemeManager guarantee no revision for equal themes and exactly one revision/change sequence
  for a semantic change; independent contexts remain isolated.
- Theme fan-out benchmarks include 500 and 1000 observers.
- Windows Qt 5.14.2/MSVC v142, Windows Qt 6, Ubuntu Qt 6, macOS Qt 6, ASan/UBSan,
  examples/benchmarks, documentation, package and installed-consumer gates remain green.

## 0.7.0 — Interaction and Effects

Goal: make interactive behavior and effects consistent, deterministic, accessible, and
performance-safe across the widget library.

Release gates:

- A shared interaction-state contract covers enabled/disabled, hover, focus, press,
  checked/selected, indeterminate, dragged, error, read-only, busy, and expanded states.
- State transitions clear impossible transient combinations and expose stable automation metadata.
- Ripple supports bounded/unbounded painting, pointer/center origins, disabled suppression,
  high-DPI correctness, and reduced-motion completion.
- State layers resolve one canonical opacity from the interaction state and theme tokens.
- Focus indication follows keyboard-focus visibility policy and shared focus-ring tokens.
- Dialog and modal transient surfaces trap focus while open and restore the previous focus target
  when dismissed.
- Tabs, menus, lists, selection controls, and buttons retain deterministic keyboard navigation.
- Motion uses theme motion tokens, supports cancellation/re-targeting, has deterministic final states,
  and collapses to immediate completion under reduced motion.
- Shadow cache identity includes device pixel ratio; cache/render benchmarks exercise 1x and 2x paths.
- Effects do not request unnecessary animation frames when inactive or reduced motion is enabled.
- Windows Qt 5.14.2/MSVC v142, Windows Qt 6, Ubuntu Qt 6, macOS Qt 6, ASan/UBSan,
  examples/benchmarks, documentation, package, and installed-consumer gates remain green.

## 0.8.0 — Component Expansion

Goal: expand the library into the common input, selection, navigation and desktop-data
workflows needed by production Qt Widgets applications while reusing the 0.5-0.7
foundation instead of introducing parallel infrastructure.

Release gates:

- Chip Assist, Filter, Input and Suggestion variants are release-scoped and at least usable.
- Search Bar and Search View preserve external QAbstractItemModel ownership and proxy filtering.
- Combo Box remains compatible with the native QComboBox model/delegate/editable contracts.
- Slider and Range Slider support keyboard, pointer, RTL and range invariants.
- DatePicker remains canonical; Time Field, Time Picker and Date Range Picker share Qt date/time primitives.
- Menu supports separators, disabled actions, shortcuts, checkable actions and exclusive action groups.
- Table preserves QTableView sorting/selection/model semantics and supports dense and multi-selection modes.
- GridList and Carousel remain Model/View-friendly desktop data components.
- Every 0.8 release-scoped component has a public header, production source, test target, docs and registry evidence.
- No 0.8 release-scoped component may remain planned, skeleton or partial.
- Windows Qt 5.14.2/MSVC v142, Windows Qt 6, Ubuntu Qt 6, macOS Qt 6, ASan/UBSan,
  examples/benchmarks, documentation, package and installed-consumer gates remain green.

## 0.9.0 — API Freeze Candidate

Goal: reduce the remaining pre-1.0 surface to the API that is intended to survive into 1.0.

Release gates:

- Tree View, Pagination, Split View, Breadcrumb and Command Palette are release-scoped only after registry ownership, public-header audit and cross-platform tests.
- Desktop Table policies preserve native QTableView model/delegate ownership.
- No placeholder, compatibility-only, or unowned public API remains.
- Installed CMake components expose application-facing modules only.
- Every checked repository-health command is implemented and fail-closed.
- Every retained C++ test is registered with CTest or explicitly belongs to a standalone harness.
- Public headers and package targets receive a final ownership and naming audit.
- Windows Qt 5.14.2/MSVC v142, Windows Qt 6, Ubuntu Qt 6 and macOS Qt 6 remain green.

## 1.0.0 — Stable API Baseline

Goal: publish the first stable QtMaterial3 source API with reproducible release
certification across the supported Qt 5 and Qt 6 toolchains.

Release gates:

- Freeze the installed public-header inventory as the canonical 1.x source surface.
- Check in a Doxygen-derived signature baseline for that installed surface; removals or
  signature changes are breaking while additive declarations remain allowed in 1.x.
- Keep implementation state behind private/PIMPL boundaries and exclude internal headers
  from the stable baseline.
- Check in reviewed visual goldens generated with pinned Qt 6.4.0 and Fusion, and verify
  them with exact zero-pixel strict comparison.
- Certify Qt 5.14.2/MSVC v142 and the exact Qt 6.4.0 minimum in CI alongside the broader
  Windows, Linux and macOS matrix.
- Publish the source-compatibility, ABI and deprecation policies for the 1.x line.
- Require release packaging, consumers, sanitizers, documentation, architecture and
  repository-health gates to remain green before tagging.


## 1.5.0 — Production / Enterprise Quality

Goal: progressively promote the remaining production-facing component families from `usable`
to `complete` without a monolithic maturity PR.

Promotion order:

1. Selection
2. Inputs
3. Navigation
4. Surfaces
5. Data
6. Progress
7. Compact controls

Family promotion gate:

- Promote one family at a time; do not batch unrelated component families into the same maturity PR.
- Every promoted component must score `4/4` on API, rendering, states, accessibility, keyboard,
  HiDPI, RTL, tests, example/gallery coverage, and documentation.
- Registry evidence must name the executable tests, production implementation, gallery route,
  documentation, and deterministic visual evidence supporting each score.
- Interactive controls must cover enabled/disabled, focus, hover/press where applicable,
  keyboard activation/navigation, accessibility state synchronization, and reduced-motion behavior.
- Directional/custom-painted controls must include deterministic RTL and DPR 2.0 evidence.
- Visual promotion requires reviewed deterministic goldens on the pinned visual-regression toolchain;
  candidate-only artifacts do not qualify as `complete`.
- Generated component status and maturity documentation must remain synchronized with the canonical registry.
- Windows Qt 5.14.2/MSVC v142, Windows Qt 6, Ubuntu Qt 6, macOS Qt 6, sanitizers,
  examples/benchmarks, documentation, package and installed-consumer gates remain green.

## 1.6.0 — Developer Experience

Goal: make the library pleasant to adopt from Qt Creator/Designer and predictable to package without weakening the stable 1.x API contract.

Release gates:

- Qt Designer/Qt Creator exposes a curated palette of widgets whose authored state can be serialized safely.
- Common Text Field authoring properties are visible through the Qt property system and round-trip through UIC.
- The Designer collection supplies useful initial DOM values instead of blank, zero-information controls.
- A real application example is authored as a `.ui` file and compiled by AUTOUIC.
- The Designer smoke fixture validates generated code and authored property values, not only plugin metadata.
- A dedicated `designer-dev` CMake preset configures, builds and runs the focused Designer contracts.
- The plugin installs as an explicit `Designer` component with an overridable plugin directory.
- Components that need task-menu, model or custom-container persistence are not exposed until that editing contract exists.
- Conan/vcpkg support is added only with executable shared/static, relocation and Qt-major consumer validation; the installed CMake package remains the canonical package-manager boundary until then.
- Windows Qt 5.14.2/MSVC v142 and Linux Qt 6 Designer lanes remain green, alongside normal examples, consumers, packaging and documentation.

## 1.7.0 — Missing Material 3

Goal: close high-value gaps in the Material 3 widget catalogue without changing the already
certified 1.5 release scope.

Initial catalogue additions:

- Navigation Bar for compact primary destinations with keyboard, disabled-state and RTL behavior.
- Side Sheet for modal/non-modal edge-anchored supporting content.
- Tooltip with hover and keyboard-focus parity plus screen-aware placement.
- Badge with dot and bounded numeric variants.
- Theme-aware rendering uses the existing Material color roles and ThemeContext pipeline rather
  than introducing parallel styling infrastructure.
- Focused CTest coverage, Gallery examples, public-header ownership and consumer documentation
  are required in the same change.

Promotion gate:

- The four 1.7 components remain outside the 1.5 release-scope count until deterministic family
  visual goldens are reviewed and platform accessibility verification is recorded.
- `missing_material3_matrix` provides pinned Qt 6.4/Fusion light, dark and high-contrast
  candidates; CI captures it twice and rejects renderer or pixel drift before human review.
- Navigation Bar exposes per-destination accessible items/actions and Side Sheet owns modal
  focus containment, initial focus and focus restoration before native AT review begins.
- Native NVDA, Orca and VoiceOver results are recorded per component in the fail-closed
  `material3-catalogue-certification-1.7.json` ledger.
- `promote_material3_catalogue_1_7.py --apply` may move the four widgets to release-scope
  `complete` only after reviewed goldens and all native AT checks pass.
- Existing package, consumer, sanitizer, architecture and public API gates must remain green.

## 1.8.0 — Material 3 Expressive

Goal: move from an Expressive foundation to production widget behavior without
changing the stable default appearance of existing 1.x applications.

Release gates:

- Common buttons expose ExtraSmall, Small, Medium, Large and ExtraLarge Expressive sizes.
- Round and Square shapes are public semantic choices and morph on press/selection.
- FAB exposes dedicated Small, Standard, Medium and Large Expressive sizing.
- Expressive widget motion uses semantic spatial motion tokens and honors reduced motion.
- Expressive rendering is shared by production button pipelines rather than example-only styling.
- Keyboard, accessibility, RTL, HiDPI, package, consumer and visual-regression gates remain green.

## 1.9.0 — Adaptive / Desktop

Goal: make Qt Widgets applications respond to available window space with one
navigation/content shell instead of application-specific breakpoint code.

Release gates:

- Window size classes cover Compact, Medium, Expanded, Large and ExtraLarge widths.
- Navigation Suite switches between Navigation Bar and Navigation Rail without losing state.
- Adaptive Shell owns responsive navigation, main content and an optional supporting pane.
- Expanded layouts can expose a secondary pane while preserving a usable main-content width.
- Desktop density is resolved and propagated to QtMaterial controls with an opt-out for app-owned density.
- RTL, keyboard navigation, accessibility, resize/tiling behavior and cross-platform CI are executable contracts.
- `adaptive_desktop_*` covers Compact/Medium/Expanded/Large/ExtraLarge in LTR/RTL and light/dark/high-contrast on the pinned Qt 6.4/Fusion lane, with two independent renders.
- Navigation Suite keeps per-destination accessible focus while switching Bar/Rail; Adaptive Shell exposes effective supporting-pane state and a synchronized accessibility summary.
- Native NVDA, Orca and VoiceOver results are recorded per component in `adaptive-desktop-certification-1.9.json`.
- `promote_adaptive_desktop_1_9.py --apply` may move Navigation Suite and Adaptive Shell to release-scope `complete` only after all 30 visual references and native AT checks pass.

## 1.10.0 — Expressive Catalogue

Goal: expose the high-value Material 3 Expressive action and feedback patterns as
first-class Qt Widgets while preserving the stable 1.x defaults of existing controls.

Release gates:

- Split Button separates primary and secondary actions with independent focus and activation.
- Button Groups provide exclusive/action modes, checked-state synchronization and RTL-aware keyboard traversal.
- Floating Toolbar supports horizontal/vertical layouts, expanded/collapsed presentation and keyboard-only navigation.
- Loading Indicator provides the Expressive morphing indeterminate silhouette and honors reduced motion.
- New widgets use the existing ThemeContext, interaction, accessibility and 1.x additive API boundaries.
- Focused CTest coverage, public API documentation, registry ownership and package/header-surface validation remain green.

## 1.11.0 — Expressive Data

Goal: extend the Expressive language into dense application data and command surfaces
without replacing the existing List, Menu or Chip contracts.

Release gates:

- List Item exposes opt-in Expressive row geometry and semantic segment positions.
- Segmented List automatically maintains Single/First/Middle/Last segment roles as content changes.
- Menu exposes opt-in Expressive item geometry and container shape while preserving authored base specs.
- Chip exposes opt-in press/selected shape morphing with deterministic reduced-motion completion.
- Existing stable List/Menu/Chip appearance remains unchanged when Expressive is disabled.
- Visual matrices, Gallery coverage and native assistive-technology evidence are required before promotion from usable to complete.

## 1.12.0 — Desktop Productivity & Workspace Persistence

Goal: harden long-lived desktop workflows around the existing Qt Model/View,
navigation and layout components without adding parallel widgets or taking
ownership away from application models, delegates, proxies, menus or settings.

Release gates:

- Table and Tree View keep advanced keyboard selection under native
  `QAbstractItemView` semantics, including modifier-based range/toggle
  selection, Home/End/page navigation and Tree expand/collapse.
- Table and Tree View expose explicit native inline-editing policies
  (DoubleClick/F2 with delegate commit/cancel) and indexed context-menu hooks;
  application code still owns editors, actions and business commands.
- Sorting/filtering remains compatible with application-owned
  `QSortFilterProxyModel` chains. Table column order, widths, visibility,
  sort presentation and desktop policies round-trip through a versioned,
  fail-safe workspace payload.
- Tree View restores header presentation, desktop policies, current item and the
  visible expanded hierarchy relative to the installed model/root index.
- Native internal drag/drop remains model-owned through MIME/drop contracts for
  Table and Tree View; enabling Material policy only configures the view.
- Split View restores pane sizes and collapse metadata across instances through
  its established versioned pane-state schema, also exposed under uniform
  `saveWorkspaceState()` / `restoreWorkspaceState()` naming.
- Command Palette remains responsive with large providers and certifies
  asynchronous result delivery, rapid request supersession, cancellation and
  stale-result rejection; provider/source-model ownership stays application-side.
- Navigation Suite preserves selected destination, enabled states, destination
  identity, keyboard focus and accessible selection while repeatedly switching
  Navigation Bar ↔ Navigation Rail across adaptive width classes.
- Command Palette persists favorites, recents and fuzzy-search preference, while
  providers, source models, activation shortcuts and transient queries remain
  application-owned.
- Every workspace restore validates magic/version and structural compatibility
  before applying user-visible state; corrupt or incompatible payloads fail
  safely without partial restore.
- `tst_desktop_workspace_state`, `tst_desktop_productivity_112` and the
  existing Command Palette/adaptive stress suites provide executable 1.12
  evidence for persistence plus the interaction contracts above.
- Public documentation shows direct `QSettings` integration and specifies the
  required model/destination setup order before restore.
- Qt 5.14.2, Qt 6, Windows, Linux, macOS, sanitizers, installed consumers and
  documentation gates remain green.

## 1.13.0 — Designer 2.0

Goal: turn the Qt Designer plugin from a palette integration into a persistence-safe
authoring environment for Material 3 desktop applications.

Release gates:

- The default palette grows from 27 to 32 controls only where authored state has a stable
  `Q_PROPERTY` representation and survives Designer -> `.ui` -> UIC round trips.
- Button Group, Navigation Bar and Segmented List expose `QStringList` authoring properties
  for their labels instead of relying on runtime-only item construction.
- A Material task menu provides a focused editor for writable scalar, enum and string-list
  properties and applies changes through the active form cursor when possible.
- Light, Dark and Expressive preview actions are available from the task menu and can restore
  the pre-preview theme without persisting preview state into the form.
- Explicit container extensions cover QtMaterialTabs and the two authored content slots of
  QtMaterialAdaptiveShell.
- The richer extension layer is compiled when Qt's Designer development module is present;
  UiPlugin-only environments retain a functioning palette and serialization path.
- AUTOUIC smoke coverage validates the new list properties and Expressive controls, while
  advanced-extension tests cover property discovery, reversible previews and container behavior.
- Qt 5.14.2 compatibility remains fail-open for optional Designer APIs and fail-closed for
  the core palette/.ui contract.


## 1.16.0 — Gallery & Documentation 2.0

Goal: turn the component Gallery, generated documentation, Dashboard and Theme Studio into a coherent user-facing product surface for the complete public catalogue.

Release gates:

- Gallery search covers component name, family, public widget type and canonical route.
- Family/component navigation is generated from the canonical registry and stays in parity with every documented public component.
- Stable deep links can be entered in the Gallery and passed through the command line.
- Preview controls cover Light/Dark, Standard/Medium/High contrast, Standard/Expressive, LTR/RTL and supported density enums.
- The component inspector exposes the common interaction-state workbench, readable/writable Qt properties and copyable C++/.ui snippets.
- Generated consumer documentation covers every public registry entry with Screenshot / When to use / API / States / Keyboard / Accessibility / RTL / Example sections.
- Reviewed visual-regression goldens are automatically reused as documentation assets when a component/family match exists; Gallery deep links remain the fallback live visual source.
- Dashboard and Theme Studio are documented and maintained as first-class application/theme showcases alongside the component Gallery.
- Documentation CI validates registry, Gallery catalogue and generated page parity before Sphinx builds.


## 1.18.0 — Native Qt Adaptation Layer

Goal: let established Qt Widgets applications adopt Material 3 incrementally
without replacing every native widget class or rewriting Designer-authored forms.

Initial release scope:

- `QtMaterialButtonAdapter` materializes an existing `QPushButton` as Text,
  Filled, Filled Tonal, Outlined or Elevated while preserving its native
  signals, ownership, object name, menu/default/checkable behavior and C++ type.
- `QtMaterialSelectionAdapter` materializes native `QCheckBox` and `QRadioButton`
  controls while preserving tristate, auto-exclusive grouping, signals and C++ type.
- Adapted buttons resolve the canonical `ButtonSpecResolver` instead of
  maintaining a parallel color/metric implementation.
- Runtime variant/density setters and namespaced dynamic properties support both
  C++ migration and Designer-authored forms.
- `applyToDescendants()` enables progressive form/window migration while an
  explicit opt-out property preserves exceptional native controls.
- `remove()` restores the previously installed per-widget style.
- Effective `ThemeContext` changes re-resolve adapted buttons through the same
  widget resolution boundary used by the rest of the library.
- Qt 5.14.2 and Qt 6 tests cover variants, density, rendering smoke, signal
  preservation, tree adaptation and restoration.

Follow-up scope:

- Extend the same opt-in adapter architecture next to `QSlider` and `QComboBox`
  only where native complex-control semantics can be retained without emulation drift.
- Add richer Designer property editors for adapter dynamic properties.
- Keep ripple, Expressive morphing and component-specific advanced behavior in
  first-class `QtMaterial*` widgets rather than silently emulating incomplete
  behavior in native controls.


## 1.19.0 — Designer 3.0

Goal: make Qt Designer a first-class Material 3 authoring environment for adaptive desktop forms,
with richer property tooling and realistic non-persistent previews.

Release gates:

- Material widgets expose writable/designable/stored properties through the focused editor without
  maintaining a narrow property-name allowlist; unsupported Qt/property types remain hidden.
- QColor overrides use a dedicated color editor and resettable theme-owned values can return to
  their Material token/default through Designer's reset/undo path.
- Enum properties use enumerated choices, while Button Group, Navigation Bar and Segmented List
  use visual collection editors with add/remove/reorder instead of raw multiline text.
- Adaptive Shell exposes serializable `automaticDensity` and `supportingPaneWidth` properties
  and retains deterministic two-slot container authoring.
- The default palette grows from 32 to 35 persistence-safe controls by promoting Adaptive Shell,
  Tooltip and Badge; all three are covered by Designer -> `.ui` -> UIC round-trip tests.
- A Designer preview surface covers Compact/Medium/Expanded representative widths, DPR 1x/2x
  and LTR/RTL without persisting those preview choices into the form.
- Theme preview selection includes the built-in Material Default Light/Dark, Blue Light,
  Green Light, Amber Dark and Rose Expressive presets and restores the previous process theme.
- Existing Light/Dark/Expressive live preview actions remain reversible and compatible with the
  richer preview dialog.
- Qt 5.14.2 keeps the UiPlugin-only fallback; advanced Designer APIs stay optional and no runtime
  consumer gains a Qt Designer dependency.
- Designer collection, extension and AUTOUIC smoke contracts remain green on the supported
  Windows Qt 5.14.2 and Qt 6 Designer lanes.
