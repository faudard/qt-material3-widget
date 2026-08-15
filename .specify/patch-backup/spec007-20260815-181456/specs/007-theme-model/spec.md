# Feature Specification: ThemeModel

**Feature Branch**: `007-theme-model`
**Created**: 2026-08-15
**Status**: Seeded; run `/speckit.analyze` before implementation
**Program/Milestone**: 0.5 Architecture Foundation
**Master-plan IDs**: THEME-001, THEME-003, THEME-004, THEME-005, THEME-006, THEME-007, THEME-013, THEME-016

## Problem Statement
After the target split, ThemeModel must be a coherent deterministic typed design-system model, not merely 'whatever is not IO/runtime'.

## Scope
### In Scope
- Own ThemeOptions/Theme/Builder, ColorScheme, typography, shape, elevation, motion/state/density/icon/accessibility values and model validation.
- Prove deterministic generation.
- Define value/copy/equality semantics.
- Exclude persistence/global runtime.
### Out of Scope
- Serialization/schema.
- ThemeContext/Manager lifecycle.
- Full typed token mappings (010).

## User / Maintainer Scenarios
### US1 - Theme builds without runtime/global services (P1)

**Value**: Model can be tested/tool-used independently.

**Independent Test**: Build same options twice with pinned backend.

**Acceptance Scenarios**
1. **Given** same input/backend, **When** ThemeBuilder runs twice, **Then** model values are equal.

### US2 - Invalid values have model diagnostics (P2)

**Value**: Resolvers require trustworthy inputs.

**Independent Test**: Validate invalid fixture.

**Acceptance Scenarios**
1. **Given** invalid model value, **When** validation runs, **Then** diagnostic is deterministic and IO-free.


## Requirements
### Functional Requirements
- **FR-001**: Define ThemeModel public value types/ownership.
- **FR-002**: Make generation deterministic for pinned input/backend.
- **FR-003**: Keep model invariant validation in ThemeModel.
- **FR-004**: Remove IO/runtime dependencies.
- **FR-005**: Document value semantics.
### Architecture Requirements
- **AR-001**: No ThemeManager/serializer/XML calls.
- **AR-002**: No QtWidgets/Core/Specs/Widgets links.
### Compatibility Requirements
- **CR-001**: Qt5/Qt6 compile.
- **CR-002**: Value types usable by ThemeIO/Runtime/Specs.
### Quality Requirements
- **QR-001**: New or changed behavior has automated evidence.
- **QR-002**: Fixed architecture debt is removed from the baseline in the same change.
- **QR-003**: Affected docs/registry/ADR artifacts are synchronized.

## Edge Cases
- A partial migration must not leave an installable public header whose implementation is not linkable.
- Optional backends/features must fail or fall back explicitly, never silently change semantics.
- CI must distinguish unsupported environment/setup failure from product regression.

## Success Criteria
- **SC-001**: ThemeModel builds without ThemeIO/Runtime.
- **SC-002**: Determinism corpus passes.
- **SC-003**: No IO/runtime dependency in model-owned code.

## Assumptions
- QColor/QFont QtGui value types may remain if needed.
- Backend identity is pinned in deterministic tests.

## Dependencies
- 006 split.
- 010 token system.

## Traceability
See `tasks.md` and `../MASTER-PLAN-MAPPING.md`.
