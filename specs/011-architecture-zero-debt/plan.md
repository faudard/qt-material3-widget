# Implementation Plan: Architecture Zero Debt

**Date**: 2026-08-15
**Spec**: `spec.md`

## Summary
Close Core/tooling and ResolvedSpec categories, then remove the transitional baseline and make strict checking permanent.

## Technical Context
- Language: C++17
- Build: CMake 3.21+
- UI: Qt Widgets
- Testing: Qt Test / CTest + Python/CMake project checks
- Relevant targets: qtmaterial3_core, qtmaterial3_tooling, qtmaterial3_widgets, qtmaterial3_specs, architecture tooling
- Relevant paths: src/core, include/qtmaterial/tooling, src/widgets, src/widgets/resolution, tools, cmake, docs/architecture

## Constitution Check
- [ ] Layer direction remains valid.
- [ ] No forbidden target/include edge is introduced.
- [ ] Qt 5.14.2 and Qt 6 impact is evaluated.
- [ ] Public API/install/export impact is evaluated.
- [ ] Material/a11y/RTL/DPI impact is evaluated when applicable.
- [ ] Performance/cache/allocation impact is evaluated when applicable.
- [ ] No new architecture baseline violation is added.

## Current-State Audit
Audited baseline: 20 Core/docs-gallery + 2 required-target + 37 ThemeIO/text-key + 21 ResolvedSpec-only = 80. Named widget debt includes button helpers/FAB, list item helper, AutocompletePopup, Tabs, selection helper, surface helper, Card and Snackbar.

## Target Design
```text
Foundation -> ThemeModel -> Specs -> Core/Effects -> Widgets
ThemeModel -> ThemeIO / ThemeRuntime
Tooling peripheral
Renderers = ResolvedSpec only
Accepted baseline = none
```

## Files
### Create
- tooling-owned component registry implementation path as needed
- docs/architecture/dependency-graph.md
### Modify
- Core component registry ownership/build lists
- named widget/render helper files
- architecture checker/CMake integration
### Remove
- architecture baseline file once tooling supports strict no-baseline operation

## Test Strategy
- Architecture checker after every group.
- Affected behavior/render tests.
- Install consumer.
- Negative rule tests.
- Qt5/Qt6 full build.

## Risks and Mitigations
| Risk | Mitigation |
|---|---|
| Behavior drift during extraction | Move resolution logic without changing resolved values; compare visual tests. |
| Large review | Reduce baseline in category/family-sized commits. |
| Checker assumes baseline file | First support empty/no-baseline strict mode, then remove it. |

## Expected Debt Movement
- Before: 80 at audited start; ~41 remain after 006/008/010 if those remove 39.
- After: 0.

## Migration Strategy
Keep the tree buildable after each coherent change. Prefer removing legacy once the replacement is proven rather than maintaining two architectures.

## Completion Gate
Strict checker reports zero actual/accepted debt; final graph documented and gated.
