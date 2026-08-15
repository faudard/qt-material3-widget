# Tasks: Repository Health and CI

- [x] **T001 [RH-FR-001]** Wrap current docs job in a valid quality workflow.
- [x] **T002 [RH-FR-002]** Add `tools/repo_health.py`.
- [x] **T003 [RH-FR-003/RH-FR-004]** Reuse architecture checker in baseline-aware mode.
- [x] **T004 [RH-FR-005]** Reuse component-status generator in `--check` mode.
- [x] **T005 [RH-FR-006]** Add unit tests for workflow structure and subprocess failure propagation.
- [x] **T006 [RH-FR-007]** Preserve Doxygen XML and Sphinx fail-on-warning job.
- [x] **T007 [RH-FR-008]** Provide future `repo_health.py --strict`.
- [ ] **T008 [GATE]** Apply patch to real checkout and run repository health.
- [ ] **T009 [GATE]** Push branch/PR and confirm GitHub `Quality / repository-health` and `Quality / docs`.
- [ ] **T010 [GATE]** Run `/speckit.analyze` against the applied repository and record remaining convergence work.
