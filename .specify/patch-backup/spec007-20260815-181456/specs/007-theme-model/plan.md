# Implementation Plan: ThemeModel

**Date**: 2026-08-15
**Spec**: `spec.md`

## Summary
Refine separated model target into deterministic value APIs and invariant validation.

## Technical Context
- Language: C++17
- Build: CMake 3.21+
- UI: Qt Widgets
- Testing: Qt Test / CTest + Python/CMake project checks
- Relevant targets: qtmaterial3_theme_model
- Relevant paths: include/qtmaterial/theme, src/theme, tests/theme

## Constitution Check
- [ ] Layer direction remains valid.
- [ ] No forbidden target/include edge is introduced.
- [ ] Qt 5.14.2 and Qt 6 impact is evaluated.
- [ ] Public API/install/export impact is evaluated.
- [ ] Material/a11y/RTL/DPI impact is evaluated when applicable.
- [ ] Performance/cache/allocation impact is evaluated when applicable.
- [ ] No new architecture baseline violation is added.

## Current-State Audit
Architecture contracts classify color backend/scheme, ThemeOptions/Theme/Builder/validation, typography/shape/elevation/motion/state/density/icon/component/accessibility tokens as model candidates.

## Target Design
```text
ThemeOptions + backend -> ThemeBuilder -> Theme(value aggregate) -> typed scales/tokens
(no IO, no ThemeManager)
```

## Files
### Create
- model-focused tests if missing
### Modify
- ThemeModel-owned files
- tests/theme
### Remove
- model convenience methods that serialize or reach runtime

## Test Strategy
- Determinism tests.
- Value semantics/invariant tests.
- Qt5/Qt6 compile.
- Forbidden-link scan.

## Risks and Mitigations
| Risk | Mitigation |
|---|---|
| Too much API churn | Change ownership first; refine only required API. |
| MCU/fallback differences | Pin backend in corpus. |

## Expected Debt Movement
- Before: Required-target debt handled by 006; text-key debt remains.
- After: No new debt; model ready for IO/runtime/token cleanup.

## Migration Strategy
Keep the tree buildable after each coherent change. Prefer removing legacy once the replacement is proven rather than maintaining two architectures.

## Completion Gate
ThemeModel independently builds and deterministic tests pass.
