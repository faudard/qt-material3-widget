# Implementation Plan: Qt 5 / Qt 6 Compatibility Contract

**Date**: 2026-08-15
**Spec**: `spec.md`

## Summary
Create one support policy and executable pinned gates.

## Technical Context
- Language: C++17
- Build: CMake 3.21+
- UI: Qt Widgets
- Testing: Qt Test / CTest + Python/CMake project checks
- Relevant targets: all public targets
- Relevant paths: .github/workflows, README.md, docs/compatibility.md, compatibility helpers

## Constitution Check
- [ ] Layer direction remains valid.
- [ ] No forbidden target/include edge is introduced.
- [ ] Qt 5.14.2 and Qt 6 impact is evaluated.
- [ ] Public API/install/export impact is evaluated.
- [ ] Material/a11y/RTL/DPI impact is evaluated when applicable.
- [ ] Performance/cache/allocation impact is evaluated when applicable.
- [ ] No new architecture baseline violation is added.

## Current-State Audit
Main CI uses Qt6 (system Linux, 6.7.3 Windows, 6.8.4 macOS). Root CMake finds Qt6 or Qt5. README advertises Qt >5.14.x or Qt6.

## Target Design
```text
compatibility policy -> pinned Qt5.14.2/MSVC + Qt6 minimum + recent Qt6 + isolated shims
```

## Files
### Create
- docs/compatibility.md
### Modify
- .github/workflows/ci.yml
- README.md
- compatibility helper files as discovered
### Remove
- None expected.

## Test Strategy
- Pinned Qt5 build/CTest.
- Qt6 minimum build/CTest.
- Public header hygiene both majors.

## Risks and Mitigations
| Risk | Mitigation |
|---|---|
| Qt5 installer fragility | Pin tested acquisition or deliberately revise support policy. |
| Matrix cost | One authoritative Qt5 platform + selected Qt6 platforms. |

## Expected Debt Movement
- Before: No architecture baseline impact.
- After: No architecture baseline impact.

## Migration Strategy
Keep the tree buildable after each coherent change. Prefer removing legacy once the replacement is proven rather than maintaining two architectures.

## Completion Gate
Every advertised Qt baseline has an executable gate and documentation agrees.
