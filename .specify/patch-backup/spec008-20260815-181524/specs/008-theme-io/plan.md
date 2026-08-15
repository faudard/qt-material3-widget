# Implementation Plan: ThemeIO

**Date**: 2026-08-15
**Spec**: `spec.md`

## Summary
Make ThemeIO the exclusive persistence/text boundary and remove IO responsibilities from runtime/resolvers.

## Technical Context
- Language: C++17
- Build: CMake 3.21+
- UI: Qt Widgets
- Testing: Qt Test / CTest + Python/CMake project checks
- Relevant targets: qtmaterial3_theme_io, qtmaterial3_theme_model
- Relevant paths: src/theme, include/qtmaterial/theme, tests/theme

## Constitution Check
- [ ] Layer direction remains valid.
- [ ] No forbidden target/include edge is introduced.
- [ ] Qt 5.14.2 and Qt 6 impact is evaluated.
- [ ] Public API/install/export impact is evaluated.
- [ ] Material/a11y/RTL/DPI impact is evaluated when applicable.
- [ ] Performance/cache/allocation impact is evaluated when applicable.
- [ ] No new architecture baseline violation is added.

## Current-State Audit
Baseline lines include component-token string conversion/overloads, resolver component-key lists, and ThemeManager/ThemeContext serialization APIs; total category = 37.

## Target Design
```text
ThemeModel <- ThemeIO(JSON/XML/schema/text mapping)
Specs use typed model IDs only; Runtime uses model only
```

## Files
### Create
- versioned theme schema
- ThemeIO mapping tables
### Modify
- serializer/XML files
- ThemeManager/Context APIs
- resolver key uses with 010
### Remove
- string-key runtime overloads outside approved IO/tooling

## Test Strategy
- JSON round-trip/schema tests.
- Invalid version diagnostics.
- XML legacy tests.
- Architecture text-key rule.

## Risks and Mitigations
| Risk | Mitigation |
|---|---|
| Coupled to typed migration | Coordinate last mappings with 010. |
| Breaking convenience APIs | Document pre-1.0 migration to explicit ThemeIO service. |

## Expected Debt Movement
- Before: 37 ThemeIO/text-key items.
- After: 0 after coordinated 008+010.

## Migration Strategy
Keep the tree buildable after each coherent change. Prefer removing legacy once the replacement is proven rather than maintaining two architectures.

## Completion Gate
ThemeIO exclusively owns persistence/text mapping and architecture category is zero.
