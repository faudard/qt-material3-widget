# Tasks: Remove Legacy SpecFactory

**Seed status:** run `/speckit.analyze` before implementation.

## Execution Tasks

- [ ] T001 [FOUNDATION] Search/classify all SpecFactory occurrences — `include src tests docs examples tools`
  - **Evidence:** Saved audit
  - **Accept:** Every occurrence classified.
- [ ] T002 [FOUNDATION] Add obsolete API/header guard — `tools or header hygiene`
  - **Evidence:** Run before deletion
  - **Accept:** Stale exposure reproducible.
- [ ] T003 [US1] Delete stale public header if present — `include/qtmaterial/specs/qtmaterialspecfactory.h`
  - **Evidence:** Header hygiene
  - **Accept:** Not installed.
- [ ] T004 [US1] Delete stale source if present — `src/specs/qtmaterialspecfactory.cpp`
  - **Evidence:** Build
  - **Accept:** No source.
- [ ] T005 [US1] Remove docs/tests/examples references — `docs tests examples`
  - **Evidence:** grep + tests
  - **Accept:** 0 active references.
- [ ] T006 [GATE] Run install consumer/header hygiene — `scripts/validate-install-consumer.sh`
  - **Evidence:** Build
  - **Accept:** No zombie API.
- [ ] T007 [GATE] Run architecture/repo health — `tools`
  - **Evidence:** Checks
  - **Accept:** No regression.

## Dependencies

T001 before deletion; T002 captures prevention; T3-T5 coherent removal.

## Final Evidence
- `git grep SpecFactory` only historical notes if intentionally retained.
- Install consumer green.

## Completion Workflow
1. Run `/speckit.analyze`.
2. Implement in dependency order with build/tests after coherent changes.
3. Run all evidence.
4. Run `/speckit.converge` and resolve appended gaps.
5. Mark complete only when success criteria and constitution checks pass.
