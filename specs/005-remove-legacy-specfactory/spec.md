# Feature Specification: Remove Legacy SpecFactory

**Feature Branch**: `005-remove-legacy-specfactory`  
**Status**: Ready for application  
**Milestone**: 0.5 Architecture Foundation  
**Created**: 2026-08-15

## Audited current state

The current public repository has already completed most of the implementation migration:

- `src/specs/CMakeLists.txt` compiles resolver implementations and does not list a factory source;
- the public `include/qtmaterial/specs` tree exposes dedicated resolver headers and no
  `qtmaterialspecfactory.h`;
- active `tests/CMakeLists.txt` registers resolver-oriented spec tests;
- root installation copies the entire public include tree, so absence from `include/`
  is also necessary for install-surface removal.

Remaining public-facing debt is documentary: README/roadmap text still describes a
factory path. This feature turns the already-mostly-complete migration into an
enforced architectural tombstone.

## Requirements

- **SF-FR-001**: No active source/header/test/example/build file may reference `SpecFactory`.
- **SF-FR-002**: No active filename may contain the legacy `specfactory` identity.
- **SF-FR-003**: No public installed header for the retired facade may remain.
- **SF-FR-004**: Current README/module documentation must describe dedicated resolvers.
- **SF-FR-005**: Current roadmap/DoD language must describe a resolver path, not a factory path.
- **SF-FR-006**: The repository must document the resolver-based replacement architecture.
- **SF-FR-007**: CI/repository health must reject reintroduction.
- **SF-FR-008**: Historical ADR/spec/migration records may mention the retired facade.
- **SF-FR-009**: No compatibility alias/shim is retained because the project is pre-1.0.
- **SF-FR-010**: The removal checker must require only the Python standard library.
- **SF-FR-011**: Applying the patch to an older/local branch deletes known legacy files
  if they still exist, after backing them up.
- **SF-FR-012**: Dedicated resolver tests remain the behavioral authority.

## Acceptance

```text
python tools/check_no_legacy_specfactory.py
python -m unittest discover -s tests/tools -p "test_*.py" -v
python tools/repo_health.py
```

All return zero.
