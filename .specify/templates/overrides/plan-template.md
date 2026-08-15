# Implementation Plan: [FEATURE]

**Branch**: `[###-feature-name]`
**Date**: [DATE]
**Spec**: `spec.md`

## Summary
[Requirement + technical approach.]

## Technical Context
- Language: C++17
- Build: CMake 3.21+
- UI: Qt Widgets
- Compatibility: Qt 5.14.2 while advertised + explicit Qt 6 minimum
- Testing: Qt Test / CTest + project tooling
- Relevant targets: [...]
- Relevant paths: [...]

## Constitution Check
### Architecture
- [ ] Layer direction remains valid.
- [ ] No forbidden link/include edge is introduced.
- [ ] Renderer/paint remains ResolvedSpec-only where applicable.
- [ ] Textual token ownership remains ThemeIO-only where applicable.
### Compatibility
- [ ] Qt 5.14.2 impact evaluated.
- [ ] Qt 6 impact evaluated.
- [ ] MSVC/GCC/Clang impact evaluated.
### API / ABI
- [ ] Public API/install/export impact identified.
- [ ] Breaking change is deliberate before 1.0.
### Material / UI
- [ ] Reference profile/version identified if UI/design changes.
- [ ] Accessibility/keyboard/RTL/DPI impact evaluated.
### Performance
- [ ] Paint/allocation/cache impact evaluated.

## Current-State Audit
[...]

## Target Design
```text
[module/dependency graph]
```

## Files
### Create
- ...
### Modify
- ...
### Remove
- ...

## Test Strategy
- ...

## Risks and Mitigations
| Risk | Mitigation |
|---|---|
| ... | ... |

## Expected Debt Movement
- Before: ...
- After: ...

## Completion Gate
[Exact conditions.]
