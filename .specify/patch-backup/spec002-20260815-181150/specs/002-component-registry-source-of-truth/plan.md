# Implementation Plan: Component Registry Source of Truth

**Date**: 2026-08-15
**Spec**: `spec.md`

## Summary
Strengthen the existing registry instead of creating a parallel manifest.

## Technical Context
- Language: C++17
- Build: CMake 3.21+
- UI: Qt Widgets
- Testing: Qt Test / CTest + Python/CMake project checks
- Relevant targets: tooling/docs
- Relevant paths: docs/components, scripts, tools

## Constitution Check
- [ ] Layer direction remains valid.
- [ ] No forbidden target/include edge is introduced.
- [ ] Qt 5.14.2 and Qt 6 impact is evaluated.
- [ ] Public API/install/export impact is evaluated.
- [ ] Material/a11y/RTL/DPI impact is evaluated when applicable.
- [ ] Performance/cache/allocation impact is evaluated when applicable.
- [ ] No new architecture baseline violation is added.

## Current-State Audit
`docs/component-status.md` declares itself generated from `docs/components/component-registry.json` and already lists maturity/header/spec/test/docs/gallery fields.

## Target Design
```text
component-registry.json -> schema/validator -> status/docs/gallery/maturity checks
```

## Files
### Create
- docs/components/component-registry.schema.json
- tools/check_component_registry.py if no equivalent exists
### Modify
- scripts/generate_component_status.py
- docs/components/component-registry.json
### Remove
- None expected.

## Test Strategy
- Schema positive/negative tests.
- Regenerate + git diff.
- Missing reference and duplicate ID tests.

## Risks and Mitigations
| Risk | Mitigation |
|---|---|
| Partial components overconstrained | Rules depend on maturity. |
| Runtime coupling | All parsing remains tooling-only. |

## Expected Debt Movement
- Before: No direct architecture change.
- After: No direct architecture change.

## Migration Strategy
Keep the tree buildable after each coherent change. Prefer removing legacy once the replacement is proven rather than maintaining two architectures.

## Completion Gate
Registry validation and deterministic regeneration are part of repo health.
