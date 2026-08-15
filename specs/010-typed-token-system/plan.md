# Implementation Plan: Typed Token System

**Date**: 2026-08-15
**Spec**: `spec.md`

## Summary
Introduce typed IDs in ThemeModel, migrate resolver usage, and place stable textual mappings exclusively in ThemeIO.

## Technical Context
- Language: C++17
- Build: CMake 3.21+
- UI: Qt Widgets
- Testing: Qt Test / CTest + Python/CMake project checks
- Relevant targets: qtmaterial3_theme_model, qtmaterial3_theme_io, qtmaterial3_specs
- Relevant paths: include/qtmaterial/theme, src/theme, src/specs, tests

## Constitution Check
- [ ] Layer direction remains valid.
- [ ] No forbidden target/include edge is introduced.
- [ ] Qt 5.14.2 and Qt 6 impact is evaluated.
- [ ] Public API/install/export impact is evaluated.
- [ ] Material/a11y/RTL/DPI impact is evaluated when applicable.
- [ ] Performance/cache/allocation impact is evaluated when applicable.
- [ ] No new architecture baseline violation is added.

## Current-State Audit
Baseline explicitly names string conversions/key overloads in component tokens and component-key lists in action-button, autocomplete-popup, button, date-field, dialog, navigation-rail, selection, surface and text-field resolvers.

## Target Design
```text
ThemeModel typed IDs -> Specs resolvers
ThemeModel typed IDs <-> ThemeIO stable text mappings
(no Specs -> ThemeIO)
```

## Files
### Create
- typed token ID header/table if needed
- ThemeIO mapping table
### Modify
- qtmaterialcomponenttokens.*
- named resolver cpp files
### Remove
- runtime string-key overloads/converters outside IO

## Test Strategy
- Type/category tests.
- Mapping uniqueness/round-trip.
- Affected resolver tests.
- Architecture text-key checker.
- Qt5/Qt6 builds.

## Risks and Mitigations
| Risk | Mitigation |
|---|---|
| Mechanical migration hides semantic error | Migrate by resolver family with tests. |
| Persistence drift | Golden text mapping tests pin existing keys. |

## Expected Debt Movement
- Before: 37 ThemeIO/text-key entries shared with 008.
- After: 0 after 008+010.

## Migration Strategy
Keep the tree buildable after each coherent change. Prefer removing legacy once the replacement is proven rather than maintaining two architectures.

## Completion Gate
Runtime/resolvers use typed IDs only and persistence mappings are stable in ThemeIO.
