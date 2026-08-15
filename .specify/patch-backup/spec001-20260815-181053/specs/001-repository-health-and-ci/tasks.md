# Tasks: Repository Health and CI

**Seed status:** run `/speckit.analyze` before implementation.

## Execution Tasks

- [ ] T001 [FOUNDATION] Wrap existing docs job in valid GitHub Actions workflow metadata/jobs — `.github/workflows/quality.yml`
  - **Evidence:** YAML/workflow validation
  - **Accept:** Workflow is structurally complete.
- [ ] T002 [FOUNDATION] Add local workflow validation or equivalent preflight — `tools or quality workflow`
  - **Evidence:** Run validator
  - **Accept:** Malformed YAML fails.
- [ ] T003 [US1] Keep docs generation strict and artifact available on failure — `.github/workflows/quality.yml`
  - **Evidence:** Doxygen/Sphinx
  - **Accept:** Warnings fail.
- [ ] T004 [US2] Create health aggregator invoking existing authoritative checks — `tools/repo_health.py`
  - **Evidence:** Run locally
  - **Accept:** Single non-zero exit on critical issue.
- [ ] T005 [US2] Add tests for health success/failure — `tests/tools or scripts tests`
  - **Evidence:** CTest/Python
  - **Accept:** Regression covered.
- [ ] T006 [GATE] Document intentional critical skips — `docs/development or STATUS.md`
  - **Evidence:** Review
  - **Accept:** No silent critical skip.

## Dependencies

T001 precedes CI validation. Health script may be developed in parallel once existing checks are enumerated.

## Final Evidence
- Doxygen/Sphinx strict build.
- Repository-health success and forced-failure case.

## Completion Workflow
1. Run `/speckit.analyze`.
2. Implement in dependency order with build/tests after coherent changes.
3. Run all evidence.
4. Run `/speckit.converge` and resolve appended gaps.
5. Mark complete only when success criteria and constitution checks pass.
