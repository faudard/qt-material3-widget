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

- Freeze and document the theme JSON schema.
- Expand token round-trip, contrast and color-backend goldens.
- Mature system-theme integration and the theme playground.
- Formalize component-local override compatibility.

## 0.7.0 — Interaction and Effects

- Common interaction-state conformance across all interactive widgets.
- Focus restoration/trapping contracts for transient surfaces.
- Reduced-motion policy.
- HiDPI shadow/cache performance certification.

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
