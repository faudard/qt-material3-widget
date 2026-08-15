# Implementation Plan: Remove Legacy SpecFactory

**Date**: 2026-08-15
**Spec**: `spec.md`

## Summary
Audit, add a failing guard if stale API is exposed, delete the legacy files and validate install consumers.

## Technical Context
- Language: C++17
- Build: CMake 3.21+
- UI: Qt Widgets
- Testing: Qt Test / CTest + Python/CMake project checks
- Relevant targets: qtmaterial3_specs
- Relevant paths: include/qtmaterial/specs, src/specs, docs, tests, tools

## Constitution Check
- [ ] Layer direction remains valid.
- [ ] No forbidden target/include edge is introduced.
- [ ] Qt 5.14.2 and Qt 6 impact is evaluated.
- [ ] Public API/install/export impact is evaluated.
- [ ] Material/a11y/RTL/DPI impact is evaluated when applicable.
- [ ] Performance/cache/allocation impact is evaluated when applicable.
- [ ] No new architecture baseline violation is added.

## Current-State Audit
Active specs target does not list `qtmaterialspecfactory.*`; root install copies the entire include directory.

## Target Design
```text
before: stale public header possible -> after: resolver/spec public surface only
```

## Files
### Create
- small obsolete-API guard if needed
### Modify
- docs/tests if references found
### Remove
- include/qtmaterial/specs/qtmaterialspecfactory.h if present
- src/specs/qtmaterialspecfactory.cpp if present

## Test Strategy
- Repository grep.
- Specs tests.
- Header hygiene.
- Install consumer.
- Architecture checker.

## Risks and Mitigations
| Risk | Mitigation |
|---|---|
| Unknown pre-1.0 consumer | Document breaking removal. |
| Wrong deletion | Classify every reference first. |

## Expected Debt Movement
- Before: No specific baseline category.
- After: No SpecFactory debt.

## Migration Strategy
Keep the tree buildable after each coherent change. Prefer removing legacy once the replacement is proven rather than maintaining two architectures.

## Completion Gate
No SpecFactory is built, installed or referenced.
