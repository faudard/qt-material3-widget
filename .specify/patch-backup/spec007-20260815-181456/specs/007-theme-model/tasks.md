# Tasks: ThemeModel

**Seed status:** run `/speckit.analyze` before implementation.

## Execution Tasks

- [ ] T001 [FOUNDATION] Confirm model-owned file list after 006 — `src/theme/CMakeLists.txt include/qtmaterial/theme`
  - **Evidence:** Review
  - **Accept:** Ownership matches live code.
- [ ] T002 [US1] Add deterministic ThemeBuilder corpus — `tests/theme`
  - **Evidence:** Qt Test
  - **Accept:** Same input/backend = same model.
- [ ] T003 [US1] Remove model dependencies on serializer/runtime — `ThemeModel src/include`
  - **Evidence:** Build + scan
  - **Accept:** No IO/runtime calls.
- [ ] T004 [US2] Define structured model validation diagnostics — `ThemeModel src/include`
  - **Evidence:** Invalid fixture tests
  - **Accept:** Deterministic diagnostics.
- [ ] T005 [US1] Document/test value copy/equality semantics — `ThemeModel headers/tests`
  - **Evidence:** Unit tests
  - **Accept:** Semantics explicit.
- [ ] T006 [GATE] Build isolated theme_model target — `CMake`
  - **Evidence:** Build
  - **Accept:** No IO/runtime required.
- [ ] T007 [GATE] Run Qt5/Qt6 compatibility — `CI/local`
  - **Evidence:** Build
  - **Accept:** Green.

## Dependencies

006 precedes. Determinism/validation tests may run in parallel after ownership stabilizes.

## Final Evidence
- ThemeModel isolated build.
- Determinism tests.
- Architecture scan.

## Completion Workflow
1. Run `/speckit.analyze`.
2. Implement in dependency order with build/tests after coherent changes.
3. Run all evidence.
4. Run `/speckit.converge` and resolve appended gaps.
5. Mark complete only when success criteria and constitution checks pass.
