# Feature Specification: Public / Private Headers

**Feature Branch**: `012-public-private-headers`  
**Status**: Install contract sealed / compiled consumer evidence pending
**Milestone**: 0.5 → 0.6 API Foundation  
**Created**: 2026-08-15

## Pre-seal audited state

Before phase 012.B, the top-level project installed the entire `include/` directory. The
Core target also listed `include/qtmaterial/core/private/qtmaterialaccessibilityhelper_p.h`
among its headers, so directory-based installation could leak implementation headers.

The repository already contained optional public-header hygiene/PIMPL/ABI CMake hooks, but
the public-header hygiene implementation globbed every widget header recursively, making
directory naming rather than an explicit API manifest the authority.

## Requirements

- **PPH-FR-001** Every project header under `include/qtmaterial` is explicitly classified.
- **PPH-FR-002** Public and private manifest sets are disjoint.
- **PPH-FR-003** The manifest is sorted, deterministic and version-controlled.
- **PPH-FR-004** `_p.*` and `private/` headers cannot be classified public.
- **PPH-FR-005** Public headers cannot include private headers.
- **PPH-FR-006** Public headers cannot include source-tree implementation paths.
- **PPH-FR-007** Root install does not use `install(DIRECTORY include/)`.
- **PPH-FR-008** Only manifest public headers are installed.
- **PPH-FR-009** Private headers are absent from install prefixes.
- **PPH-FR-010** Installed public headers equal the manifest exactly.
- **PPH-FR-011** Public-header compile tests are driven by the manifest, not `GLOB_RECURSE`.
- **PPH-FR-012** Every declared public header has a self-contained compile test.
- **PPH-FR-013** Adding/removing a header without updating the manifest fails CI.
- **PPH-FR-014** Consumer install scenarios validate the installed header surface.
- **PPH-FR-015** Repository health runs the source/install header contract.
- **PPH-FR-016** Header-surface tooling requires only the Python standard library.
- **PPH-FR-017** Existing private headers may remain physically under source `include/`
  temporarily, but their package exposure is zero.
- **PPH-FR-018** Future private-header physical migration must not change public include paths.
- **PPH-FR-019** Public classification does not itself imply ABI freeze.
- **PPH-FR-020** Qt5.14.2/Qt6 shared/static and installed-consumer gates remain required.

## Non-goals

No full ABI freeze. No forced PIMPL conversion of every public class in this feature.
No automatic promotion of new headers to public API without manifest review.
