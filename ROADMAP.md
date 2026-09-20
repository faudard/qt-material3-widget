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

- Chip family.
- Search, combo-box and slider families.
- Date/time picker maturity.
- Menu/navigation expansion.
- Data-widget maturity for desktop-heavy applications.

## 1.0 preparation

- Public source-compatibility policy.
- Deprecation lifecycle.
- Final public API inventory.
- ABI policy decision and published support matrix.
