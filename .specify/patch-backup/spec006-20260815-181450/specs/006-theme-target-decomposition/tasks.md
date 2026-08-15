# Tasks: Theme Target Decomposition

**Seed status:** run `/speckit.analyze` before implementation.

## Execution Tasks

- [ ] T001 [FOUNDATION] Classify every current theme source/header as Model/IO/Runtime — `src/theme/CMakeLists.txt + include/qtmaterial/theme`
  - **Evidence:** Ownership table
  - **Accept:** Every file has one owner.
- [ ] T002 [FOUNDATION] Add target skeletons — `src/theme/CMakeLists.txt`
  - **Evidence:** Configure
  - **Accept:** Required targets exist.
- [ ] T003 [US1] Move model-owned files to theme_model target — `src/theme/CMakeLists.txt`
  - **Evidence:** Build
  - **Accept:** No IO/runtime implementation in model.
- [ ] T004 [US1] Move serializer/XML files to theme_io target — `src/theme/CMakeLists.txt`
  - **Evidence:** Build
  - **Accept:** ThemeIO links ThemeModel.
- [ ] T005 [US1] Move context/manager/observer/system files to theme_runtime target — `src/theme/CMakeLists.txt`
  - **Evidence:** Build
  - **Accept:** Runtime links ThemeModel.
- [ ] T006 [US1] Change Specs dependency to theme_model — `src/specs/CMakeLists.txt`
  - **Evidence:** Build + architecture
  - **Accept:** No ThemeIO/Runtime link.
- [ ] T007 [US2] Define/remove/aggregate legacy Theme CMake target — `CMakeLists.txt packaging`
  - **Evidence:** Install consumer
  - **Accept:** Migration explicit.
- [ ] T008 [US2] Update install/export/package config — `CMakeLists.txt packaging`
  - **Evidence:** find_package consumer
  - **Accept:** Dependencies resolvable.
- [ ] T009 [GATE] Remove required-target baseline entries — `tools/architecture_contracts_baseline.txt`
  - **Evidence:** Architecture checker
  - **Accept:** ARCH-REQUIRED-TARGETS=0.
- [ ] T010 [GATE] Run Qt5/Qt6 build/test gates — `CI/local`
  - **Evidence:** Build/CTest
  - **Accept:** Supported majors green.

## Dependencies

T001 before T2-T5. T6-T8 integrate split. T9 only after checker confirms fixed entries.

## Final Evidence
- Architecture required targets = 0.
- Specs link graph excludes ThemeIO/Runtime.
- Install consumer green.

## Completion Workflow
1. Run `/speckit.analyze`.
2. Implement in dependency order with build/tests after coherent changes.
3. Run all evidence.
4. Run `/speckit.converge` and resolve appended gaps.
5. Mark complete only when success criteria and constitution checks pass.
