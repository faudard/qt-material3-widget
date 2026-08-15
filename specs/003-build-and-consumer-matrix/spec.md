# Feature Specification: Build and Consumer Matrix

**Feature Branch**: `003-build-and-consumer-matrix`  
**Status**: Ready for application  
**Milestone**: 0.5 Architecture Foundation  
**Created**: 2026-08-15

## Current-state findings

The root build already:
- supports `BUILD_SHARED_LIBS`;
- uses top-level/subproject-sensitive defaults for tests/examples/install;
- installs exported targets under namespace `QtMaterial3::`;
- generates `QtMaterial3WidgetsConfig.cmake` and version files.

CI currently exercises normal Qt 6 source builds on Linux/Windows/macOS and one Ubuntu
install/export validation.

The current example consumers are too weak as package-contract evidence because they link
QtMaterial3 targets but do not materially compile against QtMaterial3 public API.

## Requirements

- **BCM-FR-001**: Define a machine-readable, versioned consumer matrix.
- **BCM-FR-002**: Support/test source-tree library-only builds in shared and static linkage.
- **BCM-FR-003**: Support/test `add_subdirectory` consumption.
- **BCM-FR-004**: Support/test local/offline `FetchContent` consumption.
- **BCM-FR-005**: Support/test install + `find_package(... CONFIG COMPONENTS Theme Widgets)`.
- **BCM-FR-006**: Consumer compilation MUST include public Theme and Widgets headers and reference exported symbols.
- **BCM-FR-007**: Static consumption MUST inherit `QTMATERIAL3_STATIC` transitively from exported targets.
- **BCM-FR-008**: Unknown REQUIRED package components MUST make `find_package` fail.
- **BCM-FR-009**: PR CI uses bounded pairwise coverage across consumption mechanisms and linkages.
- **BCM-FR-010**: A `full` profile covers source/add_subdirectory/FetchContent/install × shared/static.
- **BCM-FR-011**: Consumer validation MUST not require network access.
- **BCM-FR-012**: Library-only shared/static CMake presets MUST exist.
- **BCM-FR-013**: Matrix structure MUST be checked by repository health.
- **BCM-FR-014**: Exact Qt 5.14.2 certification is Spec 004, not Spec 003.

## Acceptance

```text
python tools/check_build_consumer_matrix.py
python scripts/ci/run-consumer-matrix.py --profile ci
python scripts/ci/run-consumer-matrix.py --profile full
```

`ci` is bounded/pairwise. `full` is exhaustive across consumer mechanism × linkage.
