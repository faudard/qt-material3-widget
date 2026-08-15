# Implementation Plan: ThemeRuntime

**Date**: 2026-08-15
**Spec**: `spec.md`

## Summary
Stabilize runtime context resolution/revision/lifecycle after the target split.

## Technical Context
- Language: C++17
- Build: CMake 3.21+
- UI: Qt Widgets
- Testing: Qt Test / CTest + Python/CMake project checks
- Relevant targets: qtmaterial3_theme_runtime, qtmaterial3_core
- Relevant paths: include/qtmaterial/theme, src/theme, src/core/qtmaterialthemecontextbinding.cpp, tests

## Constitution Check
- [ ] Layer direction remains valid.
- [ ] No forbidden target/include edge is introduced.
- [ ] Qt 5.14.2 and Qt 6 impact is evaluated.
- [ ] Public API/install/export impact is evaluated.
- [ ] Material/a11y/RTL/DPI impact is evaluated when applicable.
- [ ] Performance/cache/allocation impact is evaluated when applicable.
- [ ] No new architecture baseline violation is added.

## Current-State Audit
Current theme target includes ThemeContext, ThemeContextHost, ThemeManager, ThemeObserver and SystemTheme. Core already has ThemeContextBinding intended as the widget boundary.

## Target Design
```text
explicit context
      v
effective context <- parent/inherited
      ^
optional global/default provider
revision -> binding invalidation -> resolver
```

## Files
### Create
- runtime-focused tests if missing
### Modify
- ThemeContext/Host/Manager/Observer/SystemTheme
- Core ThemeContextBinding integration
### Remove
- serialization helpers from runtime, coordinated with 008

## Test Strategy
- Context precedence tests.
- Reparent/destruction tests.
- Global revision tests.
- Qt5/Qt6 compatibility.

## Risks and Mitigations
| Risk | Mitigation |
|---|---|
| Signal storms | Monotonic revision + deduplicated invalidation tests. |
| Dangling observers | QObject ownership/QPointer/disconnect tests. |

## Expected Debt Movement
- Before: Some IO violations currently live in runtime.
- After: Runtime persistence violations removed.

## Migration Strategy
Keep the tree buildable after each coherent change. Prefer removing legacy once the replacement is proven rather than maintaining two architectures.

## Completion Gate
Effective context semantics are documented/tested; ThemeManager is fallback only.
