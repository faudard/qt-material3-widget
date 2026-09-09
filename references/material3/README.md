# QtMaterial3 Material reference data

This directory is the versioned, machine-readable source of truth for Material
conformance. It does not contain QtMaterial3 self-regression goldens.

## Layout

```text
reference-catalog.json
schema/
  reference-catalog.schema.json
  reference-profile.schema.json
  reference-component.schema.json
  reference-suite.schema.json
  visual-contract.schema.json
  visual-capture-manifest.schema.json
  visual-reference-manifest.schema.json
  visual-comparison-report.schema.json
  material-conformance-report.schema.json
classic/<reference-version>/
  profile.json
  components/<family>.json
  suites/<family>-seed.json
  suites/<component>-structure.json
  suites/<component>-renderer.json
  visual/<family>.json
  reports/<family>.json
expressive/<reference-version>/
  profile.json
```

Classic and Expressive are independent profiles. An Expressive artifact or case
must never replace or satisfy a Classic reference.

## Source precedence

1. Material guideline/spec pages define normative intent.
2. A version-pinned AndroidX Material3 artifact is the executable reference.
3. AndroidX release notes establish version and release provenance.

If guidance and executable behavior differ, record the difference explicitly.
Do not silently change a `MUST_MATCH` expectation.

## Model levels

- A profile pins source provenance, comparison domains, classifications and
  cross-platform tolerance policy.
- A component model defines required variants, slots, states, token roles,
  behavior and its exact Qt `ResolvedSpec` binding.
- A suite defines the component/state/theme Cartesian cases and, once extracted,
  source-backed assertions.
- A visual contract fixes the capture fixture, raster environment, target
  identities, deterministic state phases and future reference inventory.
- A visual capture manifest records one actual-only execution, its exact
  runtime fingerprint and normalized pixel hashes; it is not a reference set.
- A visual reference manifest records one complete reviewed target inventory
  and the capture-manifest hash from which it was promoted.
- A visual comparison report records every exact metric and canonical failure
  artifact path produced by the read-only comparator.
- A Material conformance report aggregates those independent evidence layers
  for every registry component without treating missing runtime evidence as a
  pass.

`seeded` means identities and full-model coverage are active, while numerical
assertions are not complete. `evidence-backed` means every case has assertions
with file/line locators into a revision-pinned source and is eligible for
conformance scoring. A `vertical-slice` is always a separate evidence-backed
suite; it cannot weaken or partially promote the full-model seed.

The first evidence vertical has two independent suites for the Classic small
Filled Button fixture. Phases 015.B-E complete the renderer evidence for all
five required Button variants:

- `classic.buttons.filled.structure`: 10 cases, 90 structural assertions;
- `classic.buttons.filled.renderer`: 10 cases, 100 semantic render assertions;
- `classic.buttons.filled-tonal.renderer`: 10 cases, 100 semantic render assertions;
- `classic.buttons.elevated.renderer`: 10 cases, 100 semantic render assertions;
- `classic.buttons.outlined.renderer`: 10 cases, 130 semantic render assertions;
- `classic.buttons.text.renderer`: 10 cases, 100 semantic render assertions.

Phases 016.A-C1 add `classic.buttons.visual`: the same 50 logical Button cases,
50 canonical actual output paths, four isolated Qt/platform targets and 200
canonical future reference paths. Its runner is `source-implemented`, while the
contract remains `contract-only`; its standard-library comparator is also
source-implemented and synthetically verified, but no PNG is a conformance
result yet.

Phase 017.A adds `classic.buttons.conformance`, a generated registry-wide
report. It records 5 modeled and incomplete Buttons, 27 components without an
active Material model and zero fully conformant components. A passing visual
contract remains distinct from a visual-rendering pass.

## Validation

```bash
python tools/generate_material_button_reference_seed.py --check
python tools/generate_material_button_filled_structure_reference.py --check
python tools/generate_material_button_filled_renderer_reference.py --check
python tools/generate_material_button_filled_tonal_renderer_reference.py --check
python tools/generate_material_button_elevated_renderer_reference.py --check
python tools/generate_material_button_outlined_renderer_reference.py --check
python tools/generate_material_button_text_renderer_reference.py --check
python tools/generate_material_button_visual_contract.py --check
python tools/check_material_reference_model.py --root .
python tools/check_material_structural_conformance.py --root .
python tools/check_material_renderer_conformance.py --root .
python tools/check_material_visual_contract.py --root .
python tools/generate_material_conformance_report.py --check
python tools/check_material_conformance_harness.py --root .
```

These checks use only the Python standard library and run through repository health.
