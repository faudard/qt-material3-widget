# Feature Specification: Build and Consumer Matrix

**Feature Branch**: `003-build-and-consumer-matrix`
**Created**: 2026-08-15
**Status**: Seeded; run `/speckit.analyze` before implementation
**Program/Milestone**: 0.5 Architecture Foundation
**Master-plan IDs**: BASE-007, BASE-008, BASE-009, BASE-010, BASE-011, BASE-012, BASE-013, BASE-014

## Problem Statement
The root CMake already exposes shared/static, tests/examples/install and MCU options, plus installed-consumer validation, but supported modes are not yet an explicit executable matrix.

## Scope
### In Scope
- Define critical pairwise build matrix.
- CMakePresets for local reproducibility.
- Validate installed, add_subdirectory and FetchContent consumers.
- Exercise library-only and MCU modes.
### Out of Scope
- Full CPack/release packaging.
- ABI freeze.
- Package managers.

## User / Maintainer Scenarios
### US1 - Supported consumers build (P1)

**Value**: Library integration is a product contract.

**Independent Test**: Build each consumer mode.

**Acceptance Scenarios**
1. **Given** installed prefix, **When** find_package consumer builds, **Then** it links.
2. **Given** source integration, **When** add_subdirectory/FetchContent is used, **Then** no tests/examples dependency is required.

### US2 - CI modes reproduce locally (P2)

**Value**: Avoid CI-only failures.

**Independent Test**: Configure named presets.

**Acceptance Scenarios**
1. **Given** a critical preset, **When** configured locally, **Then** it matches intended CI options.


## Requirements
### Functional Requirements
- **FR-001**: Define matrix for linkage/build-type/MCU/consumer modes.
- **FR-002**: Add named CMakePresets.
- **FR-003**: Strengthen install consumer isolation.
- **FR-004**: Add source-consumer validations.
- **FR-005**: Verify tests/examples OFF library-only build.
### Architecture Requirements
- **AR-001**: Consumer fixtures stay outside runtime targets.
- **AR-002**: Do not weaken exported dependency boundaries.
### Compatibility Requirements
- **CR-001**: Qt-version dimension is owned by spec 004.
- **CR-002**: Shared/static works on selected primary platforms.
### Quality Requirements
- **QR-001**: New or changed behavior has automated evidence.
- **QR-002**: Fixed architecture debt is removed from the baseline in the same change.
- **QR-003**: Affected docs/registry/ADR artifacts are synchronized.

## Edge Cases
- A partial migration must not leave an installable public header whose implementation is not linkable.
- Optional backends/features must fail or fall back explicitly, never silently change semantics.
- CI must distinguish unsupported environment/setup failure from product regression.

## Success Criteria
- **SC-001**: All supported consumer modes configure/link.
- **SC-002**: Critical modes have named presets/jobs.
- **SC-003**: MCU OFF and requested-ON behavior are tested.

## Assumptions
- Existing consumer examples may be reused.

## Dependencies
- 001 health.
- 004 compatibility.

## Traceability
See `tasks.md` and `../MASTER-PLAN-MAPPING.md`.
