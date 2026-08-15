# Tasks: Qt 5 / Qt 6 Compatibility Contract

**Seed status:** run `/speckit.analyze` before implementation.

## Execution Tasks

- [ ] T001 [FOUNDATION] Create canonical support policy — `docs/compatibility.md`
  - **Evidence:** Review
  - **Accept:** Exact Qt/toolchain versions named.
- [ ] T002 [FOUNDATION] Select reproducible Qt5.14.2/MSVC acquisition — `docs/compatibility.md / CI`
  - **Evidence:** Clean setup proof
  - **Accept:** No floating package.
- [ ] T003 [US1] Add pinned Qt5.14.2 Windows/MSVC job — `.github/workflows/ci.yml or release-readiness`
  - **Evidence:** Build/CTest
  - **Accept:** Qt5 breakage blocks gate.
- [ ] T004 [US2] Add explicit Qt6 minimum job — `.github/workflows/ci.yml`
  - **Evidence:** Build/CTest
  - **Accept:** Minimum proven.
- [ ] T005 [US2] Retain selected recent Qt6 gate — `.github/workflows/ci.yml`
  - **Evidence:** Build/CTest
  - **Accept:** Forward coverage.
- [ ] T006 [US1] Audit/isolate compatibility shims — `cmake/src/include`
  - **Evidence:** Qt5 compile
  - **Accept:** No unguarded newer API in touched area.
- [ ] T007 [GATE] Synchronize README support statement — `README.md`
  - **Evidence:** Review
  - **Accept:** Docs match CI.
- [ ] T008 [GATE] Run public header hygiene Qt5/Qt6 — `tests/cmake`
  - **Evidence:** CTest
  - **Accept:** Headers compile both.

## Dependencies

T001/T2 precede pinned version jobs. Spec 003 owns the matrix shape.

## Final Evidence
- Pinned Qt5 green.
- Qt6 min/recent green.
- README = compatibility policy.

## Completion Workflow
1. Run `/speckit.analyze`.
2. Implement in dependency order with build/tests after coherent changes.
3. Run all evidence.
4. Run `/speckit.converge` and resolve appended gaps.
5. Mark complete only when success criteria and constitution checks pass.
