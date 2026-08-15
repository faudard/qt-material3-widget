# Tasks: Architecture Zero Debt

**Seed status:** run `/speckit.analyze` before implementation.

## Execution Tasks

- [ ] T001 [FOUNDATION] Capture current category counts — `tools/architecture_contracts_baseline.txt`
  - **Evidence:** Architecture report
  - **Accept:** Audit baseline count recorded.
- [ ] T002 [US2] Move ComponentRegistry implementation/metadata out of Core — `src/core/qtmaterialcomponentregistry.cpp + tooling target`
  - **Evidence:** Build/tests
  - **Accept:** Core owns no docs/gallery metadata.
- [ ] T003 [US2] Remove Core/docs-gallery baseline entries — `tools/architecture_contracts_baseline.txt`
  - **Evidence:** Checker
  - **Accept:** Category=0.
- [ ] T004 [US2] Migrate button helpers/FAB to resolved-only inputs — `src/widgets/buttons`
  - **Evidence:** Button/visual tests
  - **Accept:** No Theme outside resolution.
- [ ] T005 [US2] Migrate list/data render helper — `src/widgets/data`
  - **Evidence:** Data tests
  - **Accept:** No violation.
- [ ] T006 [US2] Migrate selection render helper — `src/widgets/selection`
  - **Evidence:** Selection tests
  - **Accept:** No violation.
- [ ] T007 [US2] Migrate surface helper/Card/Snackbar — `src/widgets/surfaces`
  - **Evidence:** Surface tests
  - **Accept:** No violation.
- [ ] T008 [US2] Migrate AutocompletePopup to approved resolution boundary — `src/widgets/inputs + resolution`
  - **Evidence:** Input tests
  - **Accept:** No violation.
- [ ] T009 [US2] Migrate Tabs to approved resolution boundary — `src/widgets/navigation + resolution`
  - **Evidence:** Tabs tests
  - **Accept:** No violation.
- [ ] T010 [US1] Remove all ResolvedSpec-only baseline entries — `tools/architecture_contracts_baseline.txt`
  - **Evidence:** Checker
  - **Accept:** Category=0.
- [ ] T011 [US1] Confirm target/text categories zero from 006/008/010 — `tools`
  - **Evidence:** Checker
  - **Accept:** All four categories zero.
- [ ] T012 [US1] Enable strict no-accepted-debt mode/remove baseline dependency — `tools + cmake`
  - **Evidence:** Negative tests
  - **Accept:** Cannot silently whitelist new debt.
- [ ] T013 [US1] Add representative negative architecture fixtures — `tools/tests`
  - **Evidence:** Checker tests
  - **Accept:** Theme-in-render/text-outside-IO/QtWidgets-low-layer fail.
- [ ] T014 [GATE] Document final dependency graph — `docs/architecture/dependency-graph.md`
  - **Evidence:** Review vs CMake
  - **Accept:** Docs match live graph.
- [ ] T015 [GATE] Run full Qt5/Qt6 build/tests/install consumer — `CI/local`
  - **Evidence:** CTest/consumer
  - **Accept:** Green.
- [ ] T016 [GATE] Run repo health and verify zero — `tools/repo_health.py`
  - **Evidence:** Health report
  - **Accept:** 0 architecture debt.

## Dependencies

006/008/010 should clear target/text categories. Core and widget family migrations can proceed independently. Strict-mode conversion is last.

## Final Evidence
- Architecture strict = 0.
- Affected component tests/visual tests.
- Qt5/Qt6 full gates.
- Install consumer.
- Repository health.

## Completion Workflow
1. Run `/speckit.analyze`.
2. Implement in dependency order with build/tests after coherent changes.
3. Run all evidence.
4. Run `/speckit.converge` and resolve appended gaps.
5. Mark complete only when success criteria and constitution checks pass.
