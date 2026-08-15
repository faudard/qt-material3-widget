# Tasks: Component Registry Source of Truth

**Seed status:** run `/speckit.analyze` before implementation.

## Execution Tasks

- [ ] T001 [FOUNDATION] Audit existing registry and generator — `docs/components/component-registry.json`
  - **Evidence:** Snapshot output
  - **Accept:** All current fields classified.
- [ ] T002 [FOUNDATION] Create versioned registry JSON Schema — `docs/components/component-registry.schema.json`
  - **Evidence:** Schema test
  - **Accept:** IDs/maturity/core fields constrained.
- [ ] T003 [US1] Add/extend registry validator — `tools/check_component_registry.py`
  - **Evidence:** Positive/negative fixtures
  - **Accept:** Duplicates/missing refs fail.
- [ ] T004 [US1] Make status generator schema-aware/deterministic — `scripts/generate_component_status.py`
  - **Evidence:** Regenerate + diff
  - **Accept:** No drift.
- [ ] T005 [US2] Define maturity-specific evidence rules — `tools/check_component_registry.py`
  - **Evidence:** Negative complete fixture
  - **Accept:** `complete` requires evidence.
- [ ] T006 [US1] Detect public component/header absent from registry — `tools/check_component_registry.py`
  - **Evidence:** Repo validation
  - **Accept:** Missing representation fails.
- [ ] T007 [GATE] Integrate validation into repo health — `tools/repo_health.py`
  - **Evidence:** Health command
  - **Accept:** Divergence fails.

## Dependencies

T002 precedes validator/generator changes. 001 repo health is preferred before final integration.

## Final Evidence
- Schema validation.
- Clean regenerated status diff.
- Health gate includes registry.

## Completion Workflow
1. Run `/speckit.analyze`.
2. Implement in dependency order with build/tests after coherent changes.
3. Run all evidence.
4. Run `/speckit.converge` and resolve appended gaps.
5. Mark complete only when success criteria and constitution checks pass.
