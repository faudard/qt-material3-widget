# Implementation Plan: Component Registry Source of Truth

## Strategy

Do not replace the existing 630-line generator. Build on it.

```text
component-registry.json
        │
        ├── existing generator validator
        │      ├ paths
        │      ├ test targets
        │      ├ maturity mechanics
        │      └ generated documents
        │
        └── governance checker
               ├ schema v1 contract
               ├ global uniqueness
               ├ complete evidence policy
               ├ reference-candidate policy
               └ migration warnings
```

## Files

### Add
- `docs/components/component-registry.schema.json`
- `docs/components/component-registry-governance.md`
- `tools/check_component_registry.py`
- `tests/tools/test_component_registry.py`

### Modify cumulatively
- `tools/repo_health.py`
- `tests/tools/test_repo_health.py`
- `.github/workflows/quality.yml`

## Deliberate boundaries

1. Do not make every public/helper header a registry component.
2. Do not introduce a second hand-maintained component inventory.
3. Do not introduce `jsonschema` as a required Python dependency.
4. Do not force all `usable/partial/planned` entries to full maturity evidence in one migration.
5. Do make `complete` a strong, enforceable state now.

## Follow-on

Spec 012 later audits all installed public/private headers.
Component-family specs later fill maturity axes/evidence for non-complete entries.
