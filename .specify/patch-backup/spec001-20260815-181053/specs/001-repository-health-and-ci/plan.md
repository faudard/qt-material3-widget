# Implementation Plan: Repository Health and CI

**Date**: 2026-08-15
**Spec**: `spec.md`

## Summary
Repair quality workflow and add a thin health aggregator that invokes existing checks.

## Technical Context
- Language: C++17
- Build: CMake 3.21+
- UI: Qt Widgets
- Testing: Qt Test / CTest + Python/CMake project checks
- Relevant targets: tooling/CI
- Relevant paths: .github/workflows, tools, docs

## Constitution Check
- [ ] Layer direction remains valid.
- [ ] No forbidden target/include edge is introduced.
- [ ] Qt 5.14.2 and Qt 6 impact is evaluated.
- [ ] Public API/install/export impact is evaluated.
- [ ] Material/a11y/RTL/DPI impact is evaluated when applicable.
- [ ] Performance/cache/allocation impact is evaluated when applicable.
- [ ] No new architecture baseline violation is added.

## Current-State Audit
Main CI is valid. `quality.yml` currently contains only the docs job body with Doxygen/xmllint/Sphinx steps.

## Target Design
```text
Quality workflow -> docs gate + repository-health gate -> existing authoritative checkers
```

## Files
### Create
- tools/repo_health.py
### Modify
- .github/workflows/quality.yml
### Remove
- None expected.

## Test Strategy
- Workflow YAML validation.
- Doxygen XML + Sphinx `--fail-on-warning`.
- Health aggregator tests.

## Risks and Mitigations
| Risk | Mitigation |
|---|---|
| Duplicated truth | Aggregator invokes existing scripts. |
| Slow PR gate | Keep expensive matrices elsewhere. |

## Expected Debt Movement
- Before: Quality workflow structurally invalid/incomplete.
- After: Quality workflow valid; architecture debt tracked separately.

## Migration Strategy
Keep the tree buildable after each coherent change. Prefer removing legacy once the replacement is proven rather than maintaining two architectures.

## Completion Gate
Quality workflow is executable and repository-health command fails on critical contract violations.
