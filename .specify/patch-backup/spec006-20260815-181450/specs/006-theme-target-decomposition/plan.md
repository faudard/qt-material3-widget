# Implementation Plan: Theme Target Decomposition

**Date**: 2026-08-15
**Spec**: `spec.md`

## Summary
Split the monolithic theme build target by responsibility first, minimizing behavior changes.

## Technical Context
- Language: C++17
- Build: CMake 3.21+
- UI: Qt Widgets
- Testing: Qt Test / CTest + Python/CMake project checks
- Relevant targets: qtmaterial3_theme_model, qtmaterial3_theme_io, qtmaterial3_theme_runtime, qtmaterial3_specs
- Relevant paths: src/theme/CMakeLists.txt, src/specs/CMakeLists.txt, CMakeLists.txt, packaging

## Constitution Check
- [ ] Layer direction remains valid.
- [ ] No forbidden target/include edge is introduced.
- [ ] Qt 5.14.2 and Qt 6 impact is evaluated.
- [ ] Public API/install/export impact is evaluated.
- [ ] Material/a11y/RTL/DPI impact is evaluated when applicable.
- [ ] Performance/cache/allocation impact is evaluated when applicable.
- [ ] No new architecture baseline violation is added.

## Current-State Audit
Architecture contracts already require theme_model/theme_io. Current theme CMake compiles context/host, schemes/options/theme/builder/backend/MCU, manager/serializer/validation/observer, scales/tokens, XML/system/accessibility into one target. Specs link `qtmaterial3_theme`.

## Target Design
```text
foundation -> theme_model -> specs
                 |-> theme_io
                 |-> theme_runtime
```

## Files
### Create
- possible theme model/io/runtime subdirectory CMake grouping
### Modify
- src/theme/CMakeLists.txt
- src/specs/CMakeLists.txt
- CMakeLists.txt
- packaging/QtMaterial3WidgetsConfig.cmake.in
### Remove
- monolithic target definition once migration policy permits

## Test Strategy
- Architecture required-target check.
- Full build/CTest.
- Install consumer.
- Qt5 and Qt6 gates.

## Risks and Mitigations
| Risk | Mitigation |
|---|---|
| Header cross-dependencies | Move in buildable increments and give targets exact PUBLIC dependencies. |
| Alias/export complexity | Use explicit INTERFACE compatibility target or documented breaking removal. |

## Expected Debt Movement
- Before: 80 baseline, including 2 required-target items.
- After: 78 expected after target entries removed, before later debt cleanup.

## Migration Strategy
Keep the tree buildable after each coherent change. Prefer removing legacy once the replacement is proven rather than maintaining two architectures.

## Completion Gate
Required targets exist, Specs depends on ThemeModel only, exports work, and required-target baseline entries are removed.
