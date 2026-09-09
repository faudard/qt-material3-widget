# Feature Specification: Architecture Zero Debt

**Feature Branch**: `011-architecture-zero-debt`  
**Status**: Static convergence complete / Qt build gates pending
**Milestone**: 0.5 Architecture Foundation  
**Created**: 2026-08-15

## Audited current state

The public baseline currently contains 80 active debt keys. The existing checker already
supports strict mode, in which every violation fails regardless of baseline. A second,
independent architecture checker also exists under `scripts/architecture`.

Specs 006-010 are expected to eliminate the required-target and ThemeIO portions. Component
registry ownership has already moved toward Tooling on current main, making some historical
Core/docs baseline entries stale. The remaining substantive migration is
`ARCH-WIDGETS-RESOLVED-SPEC-ONLY`.

## Requirements

- **AZD-FR-001** Architecture debt baseline has zero active entries.
- **AZD-FR-002** The baseline cannot be regenerated from normal CMake/CI.
- **AZD-FR-003** `tools/check_architecture_contracts.py --strict` is mandatory.
- **AZD-FR-004** `scripts/architecture/check_architecture.py` is also mandatory until rule parity is proven.
- **AZD-FR-005** One aggregate zero-debt command is the release/CI authority.
- **AZD-FR-006** Architecture suppressions/waivers in source are forbidden.
- **AZD-FR-007** Required physical targets include ThemeModel, ThemeIO and ThemeRuntime.
- **AZD-FR-008** ThemeModel cannot link IO/runtime/specs/core/widgets.
- **AZD-FR-009** ThemeIO cannot link runtime/specs/core/widgets.
- **AZD-FR-010** ThemeRuntime cannot link ThemeIO/specs/core/widgets.
- **AZD-FR-011** Specs cannot link ThemeIO/ThemeRuntime/Core/Widgets/QtWidgets.
- **AZD-FR-012** Core cannot link ThemeIO or Widgets.
- **AZD-FR-013** Widgets/render helpers contain no Theme reads outside the designated resolution boundary.
- **AZD-FR-014** Resolved specs contain concrete values needed for rendering/layout/motion.
- **AZD-FR-015** No legacy SpecFactory consumer remains as part of convergence.
- **AZD-FR-016** Tooling/docs/gallery metadata does not migrate back into Core.
- **AZD-FR-017** Repository health runs the aggregate zero-debt gate.
- **AZD-FR-018** CI has a dedicated architecture-zero-debt job.
- **AZD-FR-019** Python is mandatory when architecture contracts are enabled; missing Python is not a silent skip.
- **AZD-FR-020** Qt5.14.2/Qt6 shared/static builds are run only after static architecture is zero.

## Acceptance

```text
architecture baseline active entries = 0
legacy strict checker violations      = 0
layer checker violations              = 0
architecture suppression markers      = 0
```
