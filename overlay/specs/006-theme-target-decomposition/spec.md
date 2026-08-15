# Feature Specification: Theme Target Decomposition

**Feature Branch**: `006-theme-target-decomposition`  
**Status**: Ready for application / build convergence  
**Milestone**: 0.5 Architecture Foundation  
**Created**: 2026-08-15

## Audited current state

`src/theme/CMakeLists.txt` currently builds one `qtmaterial3_theme` library containing:
- theme values/tokens;
- builder/color backend/MCU adapter;
- ThemeContext/ThemeManager/SystemTheme/observer runtime;
- JSON serializer;
- XML adapter.

`src/specs/CMakeLists.txt` links that whole target publicly, so Specs receives runtime
and IO capabilities it must not depend on.

`ThemeContext` currently fingerprints a Theme by calling `ThemeSerializer::toJson()`;
`ThemeManager` also exposes JSON/file import/export methods. Those edges explain why
ThemeRuntime temporarily depends on ThemeIO after the physical split.

## Requirements

- **TTD-FR-001** Create physical `qtmaterial3_theme_model` / `QtMaterial3::ThemeModel`.
- **TTD-FR-002** Create physical `qtmaterial3_theme_io` / `QtMaterial3::ThemeIO`.
- **TTD-FR-003** Create physical `qtmaterial3_theme_runtime` / `QtMaterial3::ThemeRuntime`.
- **TTD-FR-004** Keep `qtmaterial3_theme` / `QtMaterial3::Theme` as source-less INTERFACE umbrella.
- **TTD-FR-005** Specs MUST link ThemeModel directly and MUST NOT link Theme/ThemeIO/ThemeRuntime.
- **TTD-FR-006** Specs source/public headers MUST NOT include runtime or IO theme headers.
- **TTD-FR-007** ThemeModel owns value tokens, validation, builder/backend and MCU integration.
- **TTD-FR-008** ThemeIO owns serializer and XML adapter.
- **TTD-FR-009** ThemeRuntime owns context/host/manager/observer/system-theme.
- **TTD-FR-010** Shared builds MUST use separate MODEL/IO/RUNTIME export macros.
- **TTD-FR-011** Static builds continue to use the project-wide `QTMATERIAL3_STATIC`.
- **TTD-FR-012** Install/export set MUST publish ThemeModel, ThemeIO, ThemeRuntime and Theme.
- **TTD-FR-013** Package COMPONENTS MUST recognize ThemeModel, ThemeIO, ThemeRuntime and legacy Theme.
- **TTD-FR-014** Existing downstream code linking `QtMaterial3::Theme` remains source-compatible at CMake level.
- **TTD-FR-015** ThemeRuntime -> ThemeIO is explicitly transitional and must be removed by Specs 008/009.
- **TTD-FR-016** Repository health enforces the physical decomposition.
- **TTD-FR-017** Qt5.14.2 and Qt6 shared/static consumer gates remain mandatory.

## Non-goals

- Do not redesign Theme values yet (Spec 007).
- Do not move textual key ownership yet (Spec 008).
- Do not remove ThemeManager IO methods/fingerprint serialization yet (Spec 009).
- Do not migrate every Core/Widget consumer off the Theme umbrella in this feature.

## Acceptance

```text
python tools/check_theme_target_decomposition.py
python tools/repo_health.py
cmake --build ...  # shared + static
```

And consumer/package validation must resolve all four targets:
`ThemeModel`, `ThemeIO`, `ThemeRuntime`, `Theme`.
