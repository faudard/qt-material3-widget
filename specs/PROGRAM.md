# QtMaterial3 Spec Kit Program

**Baseline:** 2026-08-15 — repository 0.4.0

## Dependency spine

```text
001 -> 002 -> 003 -> 004
              \-> 005 -> 006 -> 007 -> 008/009 -> 010 -> 011
011 -> 013..019 (Material conformance) and 020..030 (runtime foundations)
then component families -> API/ABI/packaging -> 1.0 -> ecosystem/Expressive
```

## Catalog

| # | Spec | Milestone | Priority |
|---:|---|---|---|
| 001 | `001-repository-health-and-ci` | 0.5 | P0 |
| 002 | `002-component-registry-source-of-truth` | 0.5 | P0 |
| 003 | `003-build-and-consumer-matrix` | 0.5 | P0 |
| 004 | `004-qt5-qt6-compatibility-contract` | 0.5 | P0 |
| 005 | `005-remove-legacy-specfactory` | 0.5 | P0 |
| 006 | `006-theme-target-decomposition` | 0.5 | P0 |
| 007 | `007-theme-model` | 0.5 | P0 |
| 008 | `008-theme-io` | 0.5 | P0 |
| 009 | `009-theme-runtime` | 0.5 | P0 |
| 010 | `010-typed-token-system` | 0.5 | P0 |
| 011 | `011-architecture-zero-debt` | 0.5 | P0 |
| 012 | `012-public-private-header-boundaries` | 0.5/0.9 | P1 |
| 013 | `013-material-reference-model` | 0.5/0.6 | P1 |
| 014 | `014-material-structural-conformance` | 0.6 | P1 |
| 015 | `015-material-render-reference-generator` | 0.6 | P1 |
| 016 | `016-material-visual-comparator` | 0.6 | P1 |
| 017 | `017-material-conformance-report` | 0.6 | P1 |
| 018 | `018-material-motion-conformance` | 0.7+ | P2 |
| 019 | `019-material-conformance-ci-gates` | 0.6+ | P1 |
| 020 | `020-resolved-spec-contract` | 0.6 | P0 |
| 021 | `021-spec-invalidation-system` | 0.6 | P1 |
| 022 | `022-spec-cache-and-revisions` | 0.6 | P1 |
| 023 | `023-renderer-purity` | 0.6 | P0 |
| 024 | `024-effects-engine` | 0.6 | P1 |
| 025 | `025-motion-engine` | 0.6/1.x | P1 |
| 026 | `026-core-runtime-cleanup` | 0.6 | P0 |
| 027 | `027-theme-context-propagation` | 0.6 | P0 |
| 028 | `028-focus-interaction-framework` | 0.6 | P1 |
| 029 | `029-accessibility-foundation` | 0.6 | P0 |
| 030 | `030-high-dpi-platform-foundation` | 0.6 | P1 |
| 031 | `031-button-family-conformance` | 0.6 | P0 |
| 032 | `032-selection-family` | 0.6 | P0 |
| 033 | `033-chip-family` | 0.7 | P1 |
| 034 | `034-text-field-family` | 0.6 | P0 |
| 035 | `035-autocomplete` | 0.7 | P1 |
| 036 | `036-date-time-inputs` | 0.7 | P1 |
| 037 | `037-search-and-combobox` | 0.7 | P1 |
| 038 | `038-surface-family` | 0.6 | P0 |
| 039 | `039-navigation-model` | 0.7 | P1 |
| 040 | `040-navigation-components` | 0.7 | P1 |
| 041 | `041-menu-system` | 0.7 | P1 |
| 042 | `042-data-table` | 0.8 | P1 |
| 043 | `043-list-grid` | 0.8 | P1 |
| 044 | `044-carousel` | 0.8 | P1 |
| 045 | `045-slider-family` | 0.8 | P1 |
| 046 | `046-visual-regression-infrastructure` | 0.6 | P1 |
| 047 | `047-performance-budgets` | 0.8/0.9 | P1 |
| 048 | `048-robustness-sanitizers-fuzzing` | 0.9 | P1 |
| 049 | `049-public-api-freeze` | 0.9 | P0 |
| 050 | `050-abi-contract` | 0.9/1.0 | P0 |
| 051 | `051-packaging-release-system` | 0.9/1.0 | P0 |
| 052 | `052-documentation-system` | 0.6/1.0 | P1 |
| 053 | `053-gallery-reference-app` | 0.6/1.0 | P1 |
| 054 | `054-theme-studio` | 1.x | P2 |
| 055 | `055-developer-tooling` | 0.8/1.x | P2 |
| 056 | `056-security-supply-chain` | 0.9/1.0 | P1 |
| 057 | `057-governance-release-process` | 0.9/1.0 | P1 |
| 058 | `058-material3-coverage-completion` | 0.8/1.0 | P1 |
| 059 | `059-adaptive-layout-navigation` | 1.x | P2 |
| 060 | `060-material3-expressive-profile` | 1.x/2.x | P2 |
| 061 | `061-qt-designer-integration` | 1.x | P3 |
| 062 | `062-pyside-bindings` | 1.x | P3 |
| 063 | `063-qml-theme-bridge` | 1.x | P3 |
