# Implementation Plan

## Phase A — prepare

Install:
- aggregate zero-debt checker;
- inventory/report tool;
- post-010 target graph;
- remediation catalog;
- strict CMake/CI gate.

Do not claim convergence yet.

## Phase B — resolve every strict violation

Expected workstreams:

1. remove stale legacy baseline entries that are already fixed by 006-010;
2. delete Theme-based render-helper overloads where resolved equivalents already exist;
3. migrate button/selection/list helpers to resolved fields;
4. complete Card/Snackbar concrete runtime spec values;
5. move Autocomplete/Tabs Theme-to-Spec conversion into a designated resolution boundary;
6. migrate FAB/Card/Snackbar/other legacy SpecFactory consumers to dedicated resolvers;
7. run both architecture engines after every coherent commit.

## Phase C — seal

Only when `check_architecture_contracts.py --strict` and the second architecture engine both
return zero:
- baseline becomes comments-only;
- normal CMake target is zero-debt;
- CTest/CI/repo-health use the aggregate gate;
- update-baseline target is absent.

No “temporary” architecture exception survives sealing.
