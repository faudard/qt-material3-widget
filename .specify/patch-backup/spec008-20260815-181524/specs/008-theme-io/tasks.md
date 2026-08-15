# Tasks: ThemeIO

**Seed status:** run `/speckit.analyze` before implementation.

## Execution Tasks

- [ ] T001 [FOUNDATION] Categorize all 37 ThemeIO/text-key baseline entries — `tools/architecture_contracts_baseline.txt`
  - **Evidence:** Research mapping
  - **Accept:** Every entry assigned to 008/010.
- [ ] T002 [US1] Define versioned JSON theme schema — `specs/008-theme-io/contracts/theme-schema.json`
  - **Evidence:** Schema tests
  - **Accept:** schemaVersion required.
- [ ] T003 [US1] Move/keep serializer only in theme_io — `src/theme/CMakeLists.txt + serializer files`
  - **Evidence:** Build/link graph
  - **Accept:** No model/runtime ownership.
- [ ] T004 [US2] Move/keep XML adapter only in theme_io — `src/theme/CMakeLists.txt + XML files`
  - **Evidence:** Build/link graph
  - **Accept:** No XML in model/specs.
- [ ] T005 [US1] Move serialization API out of ThemeManager/Context — `runtime headers/sources + IO API`
  - **Evidence:** Tests + checker
  - **Accept:** No runtime IO violation.
- [ ] T006 [US1] Add strict parse/migration diagnostics — `ThemeIO`
  - **Evidence:** Invalid input tests
  - **Accept:** Explicit deterministic errors.
- [ ] T007 [US1] Add JSON round-trip corpus — `tests/theme`
  - **Evidence:** Qt Test
  - **Accept:** Equivalent model.
- [ ] T008 [US2] Add XML legacy corpus — `tests/theme`
  - **Evidence:** Qt Test
  - **Accept:** Legacy isolated.
- [ ] T009 [GATE] Integrate typed text mappings from 010 — `ThemeIO mapping files`
  - **Evidence:** Checker
  - **Accept:** No text conversion outside IO.
- [ ] T010 [GATE] Remove fixed baseline entries — `tools/architecture_contracts_baseline.txt`
  - **Evidence:** Checker
  - **Accept:** Category=0 after 010.

## Dependencies

T001 first. T2-T8 mostly independent after target split; T9/T10 coordinate with 010.

## Final Evidence
- JSON/XML tests.
- Architecture ThemeIO category zero after 010.

## Completion Workflow
1. Run `/speckit.analyze`.
2. Implement in dependency order with build/tests after coherent changes.
3. Run all evidence.
4. Run `/speckit.converge` and resolve appended gaps.
5. Mark complete only when success criteria and constitution checks pass.
