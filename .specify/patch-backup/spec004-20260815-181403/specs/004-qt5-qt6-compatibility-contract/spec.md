# Feature Specification: Qt 5 / Qt 6 Compatibility Contract

**Feature Branch**: `004-qt5-qt6-compatibility-contract`
**Created**: 2026-08-15
**Status**: Seeded; run `/speckit.analyze` before implementation
**Program/Milestone**: 0.5 Architecture Foundation
**Master-plan IDs**: BASE-004, BASE-005, BASE-006, CORE-013

## Problem Statement
README advertises Qt >5.14.x or Qt 6.x while main PR CI currently exercises Qt 6 only. Newer Qt API can therefore break the advertised Qt5 baseline without failing the main gate.

## Scope
### In Scope
- Define exact Qt5/Qt6 minimums and toolchains.
- Add real pinned Qt 5.14.2/MSVC gate while advertised.
- Add explicit Qt6 minimum + recent gates.
- Isolate compatibility shims.
### Out of Scope
- Dropping Qt5.
- Extending unsupported platforms.
- Changing C++ standard.

## User / Maintainer Scenarios
### US1 - Advertised Qt5 support is real (P1)

**Value**: Consumers need truthful support.

**Independent Test**: Compile/test pinned Qt5.14.2/MSVC.

**Acceptance Scenarios**
1. **Given** Qt5-incompatible API, **When** compatibility CI runs, **Then** it fails.

### US2 - Qt6 has explicit minimum (P2)

**Value**: Avoid floating system assumptions.

**Independent Test**: Build minimum and selected recent Qt6.

**Acceptance Scenarios**
1. **Given** declared Qt6 minimum, **When** project configures, **Then** public targets compile.


## Requirements
### Functional Requirements
- **FR-001**: Publish canonical compatibility policy.
- **FR-002**: Pin Qt5.14.2/MSVC acquisition and gate.
- **FR-003**: Define/test Qt6 minimum.
- **FR-004**: Retain recent Qt6 gate.
- **FR-005**: Centralize Qt5/Qt6 shims.
### Architecture Requirements
- **AR-001**: Compatibility shims do not duplicate component logic.
- **AR-002**: Low-layer constraints remain unchanged.
### Compatibility Requirements
- **CR-001**: MSVC toolset matches selected Qt5 binary.
- **CR-002**: README/CMake/CI support statements agree.
### Quality Requirements
- **QR-001**: New or changed behavior has automated evidence.
- **QR-002**: Fixed architecture debt is removed from the baseline in the same change.
- **QR-003**: Affected docs/registry/ADR artifacts are synchronized.

## Edge Cases
- A partial migration must not leave an installable public header whose implementation is not linkable.
- Optional backends/features must fail or fall back explicitly, never silently change semantics.
- CI must distinguish unsupported environment/setup failure from product regression.

## Success Criteria
- **SC-001**: Qt5.14.2 build/test is executable while advertised.
- **SC-002**: Qt6 minimum is explicit/tested.
- **SC-003**: No scattered unguarded newer Qt API in touched areas.

## Assumptions
- Qt5.14.2 remains desired baseline for 0.5.

## Dependencies
- 003 matrix.

## Traceability
See `tasks.md` and `../MASTER-PLAN-MAPPING.md`.
