# Tasks: ThemeRuntime

**Seed status:** run `/speckit.analyze` before implementation.

## Execution Tasks

- [ ] T001 [FOUNDATION] Write runtime precedence/lifecycle contract — `specs/009-theme-runtime/contracts/runtime-contract.md`
  - **Evidence:** Requirements review
  - **Accept:** Explicit/inherited/global order unambiguous.
- [ ] T002 [US1] Add explicit-context-without-global tests — `tests/theme or tests/core`
  - **Evidence:** Qt Test
  - **Accept:** Local correctness independent of ThemeManager.
- [ ] T003 [US1] Add inherited context + reparenting tests — `tests/core`
  - **Evidence:** Qt Test
  - **Accept:** Effective context updates correctly.
- [ ] T004 [US2] Implement/test monotonic theme revision — `ThemeRuntime files`
  - **Evidence:** Qt Test
  - **Accept:** Logical changes have deterministic revision behavior.
- [ ] T005 [US2] Add global fallback switch tests — `ThemeManager/Context tests`
  - **Evidence:** Qt Test
  - **Accept:** Only eligible contexts follow global.
- [ ] T006 [US1] Add destruction/observer cleanup tests — `tests/core/theme`
  - **Evidence:** Qt Test; later ASan
  - **Accept:** No dangling callback/UAF.
- [ ] T007 [FOUNDATION] Remove runtime persistence responsibilities with 008 — `runtime headers/sources`
  - **Evidence:** Architecture checker
  - **Accept:** No serialization-api violation.
- [ ] T008 [GATE] Run Qt5/Qt6 runtime tests — `CI/local`
  - **Evidence:** CTest
  - **Accept:** Both majors green.

## Dependencies

T001 precedes semantics. T007 coordinates with 008. Tests can be developed in parallel.

## Final Evidence
- Context precedence/reparent/destruction tests.
- Runtime target does not own IO API.

## Completion Workflow
1. Run `/speckit.analyze`.
2. Implement in dependency order with build/tests after coherent changes.
3. Run all evidence.
4. Run `/speckit.converge` and resolve appended gaps.
5. Mark complete only when success criteria and constitution checks pass.
