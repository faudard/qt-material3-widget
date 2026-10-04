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
- Promotion to release scope requires the normal API, rendering, states, accessibility, keyboard,
  HiDPI, RTL, tests, example and documentation evidence.
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
