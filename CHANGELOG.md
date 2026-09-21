# Changelog

All notable changes to qt-material3-widget are documented here.

The project follows semantic versioning while it remains pre-1.0: minor releases may contain
source or ABI changes, and those changes must be called out explicitly.

## [0.8.0] - Unreleased

### Added
- Search Bar and Search View backed by Qt Model/View and QSortFilterProxyModel.
- Combo Box, Slider and Range Slider families for desktop form workflows.
- Time Field, Time Picker and Date Range Picker built from native Qt input primitives.
- Component-expansion release contract and CI gate covering the complete 0.8 scope.

### Changed
- Pre-1.0 cleanup removes repository scaffolding, forwarding headers, duplicate resolver/spec surfaces, placeholder tests, and accidental public APIs instead of deprecating them.
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
