# Implementation Plan: Build and Consumer Matrix

**Date**: 2026-08-15
**Spec**: `spec.md`

## Summary
Codify the existing CMake options into a critical pairwise matrix and add missing consumer fixtures.

## Technical Context
- Language: C++17
- Build: CMake 3.21+
- UI: Qt Widgets
- Testing: Qt Test / CTest + Python/CMake project checks
- Relevant targets: all exported targets
- Relevant paths: CMakeLists.txt, CMakePresets.json, examples, scripts/ci

## Constitution Check
- [ ] Layer direction remains valid.
- [ ] No forbidden target/include edge is introduced.
- [ ] Qt 5.14.2 and Qt 6 impact is evaluated.
- [ ] Public API/install/export impact is evaluated.
- [ ] Material/a11y/RTL/DPI impact is evaluated when applicable.
- [ ] Performance/cache/allocation impact is evaluated when applicable.
- [ ] No new architecture baseline violation is added.

## Current-State Audit
Root CMake exposes `BUILD_SHARED_LIBS`, tests/examples/benchmarks/install and MCU options. Install exports targets; main CI validates installed consumer only on ubuntu-qt6.

## Target Design
```text
source -> direct/add_subdirectory/FetchContent ; install -> find_package ; selected shared/static/debug/release/MCU modes
```

## Files
### Create
- source consumer fixture(s) if absent
### Modify
- CMakePresets.json
- scripts/validate-install-consumer.sh
- .github/workflows/ci.yml
### Remove
- None expected.

## Test Strategy
- Preset smoke tests.
- Install consumer.
- add_subdirectory consumer.
- FetchContent consumer.
- Shared/static selected gates.

## Risks and Mitigations
| Risk | Mitigation |
|---|---|
| Matrix explosion | Use PR pairwise matrix + broader scheduled/release matrix. |
| Network dependency | Use local-source FetchContent validation. |

## Expected Debt Movement
- Before: No architecture baseline impact.
- After: No architecture baseline impact.

## Migration Strategy
Keep the tree buildable after each coherent change. Prefer removing legacy once the replacement is proven rather than maintaining two architectures.

## Completion Gate
Three consumer modes and critical build modes have automated evidence.
