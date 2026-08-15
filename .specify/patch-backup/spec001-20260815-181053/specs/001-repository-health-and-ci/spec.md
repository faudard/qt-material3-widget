# Feature Specification: Repository Health and CI

**Feature Branch**: `001-repository-health-and-ci`
**Created**: 2026-08-15
**Status**: Seeded; run `/speckit.analyze` before implementation
**Program/Milestone**: 0.5 Architecture Foundation
**Master-plan IDs**: BASE-001, BASE-002, BASE-003, BASE-020, BASE-021, BASE-022, BASE-025

## Problem Statement
The repository has substantial CI, but `.github/workflows/quality.yml` currently starts directly with a `docs:` job and lacks a complete workflow wrapper. Quality cannot be trusted until its own infrastructure is valid.

## Scope
### In Scope
- Repair quality workflow structure.
- Make docs validation blocking.
- Add one local repository-health entry point reusing authoritative checks.
- Expose critical disabled/skipped checks.
### Out of Scope
- Theme refactoring.
- New components.
- Full packaging/release automation.

## User / Maintainer Scenarios
### US1 - Trustworthy PR quality signal (P1)

**Value**: Broken quality infrastructure must fail visibly.

**Independent Test**: Intentionally break workflow/docs fixture and verify failure.

**Acceptance Scenarios**
1. **Given** valid repo, **When** quality jobs run, **Then** they pass.
2. **Given** malformed docs/workflow input, **When** quality runs, **Then** it fails.

### US2 - Local health summary (P2)

**Value**: Short feedback loop.

**Independent Test**: Run one documented command from repo root.

**Acceptance Scenarios**
1. **Given** a critical contract failure, **When** local health runs, **Then** exit code is non-zero.


## Requirements
### Functional Requirements
- **FR-001**: Create complete valid `quality.yml`.
- **FR-002**: Keep Doxygen XML validation and Sphinx warnings blocking.
- **FR-003**: Create `tools/repo_health.py` or equivalent aggregator.
- **FR-004**: Integrate architecture and component-registry health checks.
### Architecture Requirements
- **AR-001**: Health tooling belongs in tools/scripts/CI, not Core.
- **AR-002**: Reuse authoritative checks; do not duplicate their rules.
### Compatibility Requirements
- **CR-001**: Runs on project-supported Python/CI environment.
- **CR-002**: Does not require GUI display for purely static checks.
### Quality Requirements
- **QR-001**: New or changed behavior has automated evidence.
- **QR-002**: Fixed architecture debt is removed from the baseline in the same change.
- **QR-003**: Affected docs/registry/ADR artifacts are synchronized.

## Edge Cases
- A partial migration must not leave an installable public header whose implementation is not linkable.
- Optional backends/features must fail or fall back explicitly, never silently change semantics.
- CI must distinguish unsupported environment/setup failure from product regression.

## Success Criteria
- **SC-001**: `quality.yml` is a valid workflow.
- **SC-002**: Docs warning/error fails quality.
- **SC-003**: One local health command returns meaningful exit status.

## Assumptions
- Existing Doxygen/Sphinx commands remain intended.
- Architecture checker remains authoritative.

## Dependencies
- Existing CI/docs/tooling.

## Traceability
See `tasks.md` and `../MASTER-PLAN-MAPPING.md`.
