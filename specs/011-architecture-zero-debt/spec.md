# Feature Specification: Architecture Zero Debt

**Feature Branch**: `011-architecture-zero-debt`
**Created**: 2026-08-15
**Status**: Seeded; run `/speckit.analyze` before implementation
**Program/Milestone**: 0.5 Architecture Foundation
**Master-plan IDs**: ARCH-001, ARCH-009, ARCH-010, ARCH-012, ARCH-013, ARCH-014, ARCH-015, ARCH-016, ARCH-017, ARCH-018, ARCH-019, ARCH-020

## Problem Statement
Current baseline contains 80 accepted violations: 20 Core/docs-gallery, 2 required-target, 37 ThemeIO/text-key and 21 Widgets/ResolvedSpec-only. 006/008/010 target the 2+37; 0.5 requires the remaining debt and the baseline mechanism itself to reach zero.

## Scope
### In Scope
- Move component registry/docs metadata out of Core.
- Migrate all remaining widget/render helpers to approved resolution boundaries.
- Confirm target/text categories zero.
- Remove transitional accepted baseline and enable strict zero-debt mode.
- Document final graph and add negative architecture tests.
### Out of Scope
- Full component conformance.
- Unrelated visual redesign.
- Performance optimization beyond preserving behavior.

## User / Maintainer Scenarios
### US1 - Architecture checker is strict zero-debt (P1)

**Value**: 0.5 must prevent relapse.

**Independent Test**: Run checker without accepted debt and inject representative violation.

**Acceptance Scenarios**
1. **Given** clean tree, **When** checker runs, **Then** 0 violations.
2. **Given** forbidden Theme include in renderer, **When** checker runs, **Then** it fails.

### US2 - Core/renderers have correct ownership (P1)

**Value**: Remaining categories are concrete design debt.

**Independent Test**: Run targeted tests/scans.

**Acceptance Scenarios**
1. **Given** registry metadata, **When** Core scan runs, **Then** docs/gallery violations are zero.
2. **Given** widget render paths, **When** scanned, **Then** Theme access outside resolution is zero.


## Requirements
### Functional Requirements
- **FR-001**: Eliminate 20 Core/docs-gallery entries.
- **FR-002**: Eliminate 21 ResolvedSpec-only entries.
- **FR-003**: Confirm 2 required-target and 37 ThemeIO/text categories already zero.
- **FR-004**: Remove/no longer use accepted baseline.
- **FR-005**: Strengthen negative architecture tests.
- **FR-006**: Document final target/dependency graph.
### Architecture Requirements
- **AR-001**: No waiver may be added to hit 0.5 date.
- **AR-002**: Renderer extraction preserves resolved values/observable behavior.
- **AR-003**: Tooling metadata does not become Core runtime dependency.
### Compatibility Requirements
- **CR-001**: Qt5/Qt6 builds stay green.
- **CR-002**: Install/header behavior remains coherent after tooling move.
### Quality Requirements
- **QR-001**: New or changed behavior has automated evidence.
- **QR-002**: Fixed architecture debt is removed from the baseline in the same change.
- **QR-003**: Affected docs/registry/ADR artifacts are synchronized.

## Edge Cases
- A partial migration must not leave an installable public header whose implementation is not linkable.
- Optional backends/features must fail or fall back explicitly, never silently change semantics.
- CI must distinguish unsupported environment/setup failure from product regression.

## Success Criteria
- **SC-001**: Architecture baseline count = 0.
- **SC-002**: Checker passes with no accepted debt.
- **SC-003**: Representative negative fixtures fail.
- **SC-004**: Final docs graph matches CMake.

## Assumptions
- 006/008/010 land first or within same coherent sequence.
- Some violating code may be valid logic in wrong physical boundary; move it rather than delete semantics.

## Dependencies
- 006 targets.
- 008 IO.
- 010 tokens.
- 001 health.

## Traceability
See `tasks.md` and `../MASTER-PLAN-MAPPING.md`.
