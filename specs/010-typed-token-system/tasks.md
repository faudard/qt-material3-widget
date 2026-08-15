# Tasks: Typed Token System

**Seed status:** run `/speckit.analyze` before implementation.

## Execution Tasks

- [ ] T001 [FOUNDATION] Inventory current token categories/keys/persistence text — `qtmaterialcomponenttokens.* + named resolvers`
  - **Evidence:** Mapping review
  - **Accept:** No key omitted.
- [ ] T002 [FOUNDATION] Define typed token ID/category types — `include/qtmaterial/theme`
  - **Evidence:** Compile tests
  - **Accept:** C++17/Qt5 compatible.
- [ ] T003 [US1] Migrate button/action-button resolver keys — `src/specs/qtmaterialbuttonspecresolver.cpp + action resolver`
  - **Evidence:** Resolver tests
  - **Accept:** No raw key strings.
- [ ] T004 [US1] Migrate input resolver keys — `src/specs autocomplete/date/textfield resolvers`
  - **Evidence:** Resolver tests
  - **Accept:** No raw key strings.
- [ ] T005 [US1] Migrate selection/navigation/surface/dialog resolver keys — `src/specs relevant resolvers`
  - **Evidence:** Resolver tests
  - **Accept:** No raw key strings.
- [ ] T006 [US2] Implement ThemeIO text mapping — `ThemeIO files`
  - **Evidence:** Round-trip/uniqueness tests
  - **Accept:** Stable text<->ID.
- [ ] T007 [US1] Remove runtime string-key overloads/conversions — `qtmaterialcomponenttokens.*`
  - **Evidence:** Build + scan
  - **Accept:** No outside-IO conversion.
- [ ] T008 [US2] Pin text mapping with golden tests — `tests/theme`
  - **Evidence:** Qt Test
  - **Accept:** No accidental rename.
- [ ] T009 [GATE] Run affected spec/resolver tests — `tests/specs`
  - **Evidence:** CTest
  - **Accept:** Green.
- [ ] T010 [GATE] Remove remaining ThemeIO/text baseline entries after 008 — `tools/architecture_contracts_baseline.txt`
  - **Evidence:** Architecture checker
  - **Accept:** Category=0.
- [ ] T011 [GATE] Run Qt5/Qt6 builds — `CI/local`
  - **Evidence:** Build/CTest
  - **Accept:** Green.

## Dependencies

T001/T2 first. Resolver family migrations may run in parallel. Text mapping depends on T2; removal depends on all migrations and 008.

## Final Evidence
- Architecture text-key category zero.
- Mapping round-trip/uniqueness tests.
- Affected resolver tests.

## Completion Workflow
1. Run `/speckit.analyze`.
2. Implement in dependency order with build/tests after coherent changes.
3. Run all evidence.
4. Run `/speckit.converge` and resolve appended gaps.
5. Mark complete only when success criteria and constitution checks pass.
