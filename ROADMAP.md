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

- No placeholder, compatibility-only, or unowned public API remains.
- Installed CMake components expose application-facing modules only.
- Every checked repository-health command is implemented and fail-closed.
- Every retained C++ test is registered with CTest or explicitly belongs to a standalone harness.
- Public headers and package targets receive a final ownership and naming audit.
- Windows Qt 5.14.2/MSVC v142, Windows Qt 6, Ubuntu Qt 6 and macOS Qt 6 remain green.

## 1.0 preparation

- Publish the final source-compatibility policy.
- Define the post-1.0 deprecation lifecycle; no pre-1.0 compatibility shims are retained.
- Freeze the final public API inventory.
- Publish the ABI policy and supported platform/toolchain matrix.
