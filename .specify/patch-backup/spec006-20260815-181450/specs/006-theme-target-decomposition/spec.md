# Feature Specification: Theme Target Decomposition

**Feature Branch**: `006-theme-target-decomposition`
**Created**: 2026-08-15
**Status**: Seeded; run `/speckit.analyze` before implementation
**Program/Milestone**: 0.5 Architecture Foundation
**Master-plan IDs**: ARCH-003, ARCH-004, ARCH-005, ARCH-006, ARCH-007, ARCH-008, ARCH-011

## Problem Statement
The architecture contract requires `qtmaterial3_theme_model` and `qtmaterial3_theme_io`, while live `src/theme/CMakeLists.txt` builds one `qtmaterial3_theme` containing model values, runtime context/manager/observer/system theme and JSON/XML serialization. Specs currently link the monolith.

## Scope
### In Scope
- Create ThemeModel, ThemeIO and ThemeRuntime targets.
- Assign every current theme file to an owner.
- Make Specs link ThemeModel only.
- Update install/export targets and define any temporary legacy Theme target policy.
### Out of Scope
- Redesigning every theme class API.
- Typed-token internal migration (010).
- Material conformance.

## User / Maintainer Scenarios
### US1 - Linker enforces theme boundaries (P1)

**Value**: Bad dependencies become impossible.

**Independent Test**: Inspect target graph/build Specs.

**Acceptance Scenarios**
1. **Given** split targets, **When** Specs links, **Then** it links ThemeModel only.
2. **Given** package install, **When** supported theme targets are consumed, **Then** all dependencies resolve.

### US2 - Consumers have an explicit target migration (P2)

**Value**: Target split can break CMake users.

**Independent Test**: Build documented install consumer.

**Acceptance Scenarios**
1. **Given** former Theme target usage, **When** 0.5 migration policy is followed, **Then** behavior is unambiguous.


## Requirements
### Functional Requirements
- **FR-001**: Create `qtmaterial3_theme_model`, `qtmaterial3_theme_io`, `qtmaterial3_theme_runtime` or explicitly justified equivalent runtime target.
- **FR-002**: Assign each existing theme source/header ownership.
- **FR-003**: Make Specs link ThemeModel only.
- **FR-004**: Update root install/export/package config.
- **FR-005**: Define whether `QtMaterial3::Theme` is removed or a time-bounded compatibility aggregate.
### Architecture Requirements
- **AR-001**: ThemeModel has no QtWidgets/Core/Specs/Widgets.
- **AR-002**: ThemeIO -> ThemeModel only for design data.
- **AR-003**: ThemeRuntime -> ThemeModel only for design data.
- **AR-004**: Specs do not link ThemeIO/ThemeRuntime.
### Compatibility Requirements
- **CR-001**: Qt5/Qt6 builds and installed consumers remain valid.
- **CR-002**: No installed header requires unexported target dependencies.
### Quality Requirements
- **QR-001**: New or changed behavior has automated evidence.
- **QR-002**: Fixed architecture debt is removed from the baseline in the same change.
- **QR-003**: Affected docs/registry/ADR artifacts are synchronized.

## Edge Cases
- A partial migration must not leave an installable public header whose implementation is not linkable.
- Optional backends/features must fail or fall back explicitly, never silently change semantics.
- CI must distinguish unsupported environment/setup failure from product regression.

## Success Criteria
- **SC-001**: 2 required-target baseline entries reach 0.
- **SC-002**: Specs no longer links monolithic theme target.
- **SC-003**: Installed package exposes coherent documented targets.

## Assumptions
- Breaking CMake target changes acceptable pre-1.0.
- 007/008/009 refine ownership after skeleton split.

## Dependencies
- 005 recommended first.
- 003/004 gates.
- 007/008/009 follow.

## Traceability
See `tasks.md` and `../MASTER-PLAN-MAPPING.md`.
