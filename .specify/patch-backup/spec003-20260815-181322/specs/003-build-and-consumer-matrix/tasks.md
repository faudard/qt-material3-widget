# Tasks: Build and Consumer Matrix

**Seed status:** run `/speckit.analyze` before implementation.

## Execution Tasks

- [ ] T001 [FOUNDATION] Write critical build matrix contract — `specs/003-build-and-consumer-matrix/contracts/build-matrix.md`
  - **Evidence:** Review
  - **Accept:** Dimensions/coverage explicit.
- [ ] T002 [US2] Add/update named CMake presets — `CMakePresets.json`
  - **Evidence:** cmake --list-presets / configure
  - **Accept:** Critical modes reproducible.
- [ ] T003 [US1] Strengthen isolated installed consumer validation — `scripts/validate-install-consumer.sh`
  - **Evidence:** Run script
  - **Accept:** No source leakage.
- [ ] T004 [US1] Add add_subdirectory consumer validation — `examples or tests/consumer`
  - **Evidence:** Configure/build
  - **Accept:** Works library-only.
- [ ] T005 [US1] Add FetchContent/local-source consumer validation — `examples or tests/consumer`
  - **Evidence:** Configure/build
  - **Accept:** Works through FetchContent contract.
- [ ] T006 [US1] Add selected shared/static CI coverage — `.github/workflows/ci.yml`
  - **Evidence:** CI
  - **Accept:** Both linkage modes proven.
- [ ] T007 [US1] Add MCU OFF/requested-ON coverage — `.github/workflows/ci.yml or release-readiness`
  - **Evidence:** CI
  - **Accept:** Backend behavior explicit.
- [ ] T008 [GATE] Run tests/examples OFF library-only build — `CMakePresets.json / CI`
  - **Evidence:** Configure/build
  - **Accept:** No hidden dependency.

## Dependencies

T001 defines required coverage. Source consumers can be built in parallel.

## Final Evidence
- Installed/add_subdirectory/FetchContent consumers link.
- Selected shared/static/MCU modes green.

## Completion Workflow
1. Run `/speckit.analyze`.
2. Implement in dependency order with build/tests after coherent changes.
3. Run all evidence.
4. Run `/speckit.converge` and resolve appended gaps.
5. Mark complete only when success criteria and constitution checks pass.
