# Implementation Plan: Repository Health and CI

## Change set

### Modify
- `.github/workflows/quality.yml`

### Add
- `tools/repo_health.py`
- `tests/tools/test_repo_health.py`

## Design decisions

1. Do not create a second architecture-rule implementation.
2. Do not require PyYAML or another new runtime dependency for the local health script.
3. The workflow-structure checker is deliberately a smoke check; GitHub remains the authority for full Actions semantics.
4. Use `scripts/generate_component_status.py --check` because the repository already provides the correct non-mutating contract.
5. Keep default architecture validation baseline-aware until spec 011.
6. Keep `--strict` available now so the same health entry point can become the zero-debt gate later.

## CI topology

```text
quality.yml
├ repository-health
│  ├ test repo_health.py
│  └ python tools/repo_health.py
│      ├ workflow structural smoke check
│      ├ check_architecture_contracts.py
│      └ generate_component_status.py --check
└ docs
   ├ doxygen
   ├ xmllint
   └ sphinx --fail-on-warning
```

## Non-goals

- Qt 5.14.2 CI: spec 004.
- Architecture zero debt: spec 011.
- Registry schema expansion: spec 002.
- Full GitHub workflow static linter dependency: optional future hardening.
