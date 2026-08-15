# Feature Specification: ThemeModel

**Feature Branch**: `007-theme-model`  
**Status**: Ready for application / build convergence  
**Milestone**: 0.5 Architecture Foundation  
**Created**: 2026-08-15

## Audited current state

`Theme` is already a non-QObject aggregate of options and token/value containers, but it
does not currently declare full copy/move/equality semantics. ThemeContext determines
whether a Theme changed by serializing it with `ThemeSerializer::toJson()`, which couples
runtime identity to ThemeIO and to the current JSON representation.

The ThemeModel containers use finite typed role enums for Material values, while component
overrides still retain string/custom extension points that will be addressed more deeply
by Specs 008/010.

## Requirements

- **TM-FR-001** Theme remains a non-QObject value type.
- **TM-FR-002** Theme explicitly supports copy construction/assignment and move construction/assignment.
- **TM-FR-003** Theme provides structural `operator==` and `operator!=`.
- **TM-FR-004** Structural identity covers all semantically exposed Theme fields.
- **TM-FR-005** Structural identity must not depend on QHash iteration order.
- **TM-FR-006** Structural identity must not depend on ThemeSerializer/JSON/XML.
- **TM-FR-007** ThemeContext must use structural Theme equality for no-op updates.
- **TM-FR-008** ThemeContext must no longer include/use serializer or JSON for identity.
- **TM-FR-009** ThemeModel source/headers must not depend on QObject/QWidget/application runtime.
- **TM-FR-010** ThemeModel source/headers must not depend on ThemeManager/ThemeContext/SystemTheme.
- **TM-FR-011** ThemeModel source/headers must not depend on ThemeIO classes or QJson/QXml/file persistence.
- **TM-FR-012** Published runtime Theme values are copied and exposed as const snapshot state.
- **TM-FR-013** No new external/textual fingerprint format is introduced without a versioned canonicalization contract.
- **TM-FR-014** Equality/value semantics have C++ regression tests.
- **TM-FR-015** Repository health enforces ThemeModel autonomy.
- **TM-FR-016** Qt 5.14.2 + Qt6 and shared/static builds remain required gates.

## Non-goals

- Theme textual-key ownership (Spec 008).
- ThemeManager/runtime API cleanup (Spec 009).
- Full typed runtime token ID migration (Spec 010).
- Removing compatibility typography aliases in this feature.
- Introducing a second ThemeSnapshot class without demonstrated need.

## Acceptance

```text
python tools/check_theme_model.py
python tools/repo_health.py
ctest -R "themeidentity|themecontext"
```

and Qt5/Qt6 shared/static build gates remain green.
