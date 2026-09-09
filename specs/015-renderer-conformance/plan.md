# Implementation Plan

## Phase 015.A — Filled Button renderer vertical

1. Consume the separate 10-case semantic renderer suite.
2. Extract active resolver, state-token and renderer semantics.
3. Correct disabled colors/alpha and per-state elevation.
4. Assign hover/focus to the fixed state layer and press to ripple.
5. Enforce disabled feedback guards and `ResolvedSpec`-only paint.
6. Add mutation tests and the repository-health gate.

## Phase 015.B — Elevated Button renderer vertical

1. Add a separate 10-case Elevated semantic renderer suite.
2. Reuse one declarative suite builder for Filled and Elevated evidence.
3. Resolve normal and hover elevation roles/styles independently.
4. Correct Elevated disabled colors/alpha and all five elevation states.
5. Certify the inherited Filled paint path and the Elevated variant binding.
6. Compare 200 cumulative assertions and add Elevated drift mutations.

## Phase 015.C — Filled Tonal Button renderer vertical

1. Add a separate 10-case Filled Tonal semantic renderer suite.
2. Reuse the shared declarative suite builder and pinned AndroidX evidence.
3. Correct Filled Tonal disabled roles/alpha and hover-only elevation.
4. Apply variant overrides through typed semantic roles.
5. Certify variant identity for both constructors and the inherited paint path.
6. Compare 300 cumulative assertions and add Filled Tonal drift mutations.

## Phase 015.D — Outlined Button renderer vertical

1. Add a separate 10-case Outlined semantic renderer suite.
2. Include outline role, disabled alpha and 1 dp width as sourced assertions.
3. Resolve Outlined content, border, flat elevation and width into `ButtonSpec`.
4. Preserve semantic roles through typed component overrides.
5. Certify the distinct Outlined paint path, disabled guards and ripple ownership.
6. Compare 430 cumulative assertions and add Outlined drift mutations.

## Phase 015.E — Text Button renderer vertical

1. Add the final separate 10-case Text semantic renderer suite.
2. Follow the executable AndroidX `Primary` content correction and retain exact
   pinned evidence for the documented token mismatch.
3. Resolve Text disabled content alpha and flat elevation into `ButtonSpec`.
4. Preserve Text semantic roles through a typed component-token branch.
5. Certify the distinct Text paint path and all disabled feedback guards.
6. Compare 530 cumulative assertions and complete all required Button variants.

The next phase should start deterministic visual comparison for this fully
certified Button fixture without broadening the renderer refactor.
