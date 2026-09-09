# Material reference model

QtMaterial3 now separates Material conformance evidence from internal visual
regression.

An internal golden answers: “did QtMaterial3 change relative to itself?” The
reference model answers: “does this component match a named Material profile,
apart from explicitly declared platform adaptations?”

## Active profiles

| Track | AndroidX coordinate | Release | Stability | Role |
|---|---|---:|---|---|
| Classic | `androidx.compose.material3:material3:1.4.0` | 2025-09-24 | stable | Active baseline |
| Expressive | `androidx.compose.material3:material3:1.5.0-alpha27` | 2026-08-26 | experimental | Independent future track |

The profiles were retrieved and reviewed on 2026-09-01. Their source URIs,
artifact versions and comparison policies live under `references/material3/`.

## Four machine-readable levels

| Level | Defines | Current v1 artifact |
|---|---|---|
| Profile | source precedence, version, classifications, domains, tolerance policy | Classic and Expressive profiles |
| Component model | family, required variants, slots, states, token roles, behavior and Qt binding | Classic Buttons |
| Suite | deterministic component × state × theme cases and evidence assertions | Buttons seed + Filled structure + five renderer verticals |
| Visual contract | fixture, raster environment, fixed state phases, isolated targets, reference-set lifecycle and paths | Classic Buttons contract-only matrix |

The Classic Buttons model is explicit:

- 5 required variants: text, filled, filled tonal, outlined and elevated;
- 7 structural slots, including optional leading icon and the desktop focus
  adaptation;
- 5 required states: enabled, disabled, hovered, focused and pressed;
- 17 semantic token roles, including Outlined width as a first-class token;
- 3 expected behavior contracts;
- all 40 public `ButtonSpec` fields mapped exactly once, either to a semantic
  token role or to an explicit runtime-support field.

The seeded suite is the complete Cartesian product:

```text
5 variants × 5 states × 2 themes = 50 cases
```

The evidence-backed verticals are:

```text
button.filled × 5 states × 2 themes = 10 cases
button.filled-tonal × 5 states × 2 themes = 10 cases
button.elevated × 5 states × 2 themes = 10 cases
button.outlined × 5 states × 2 themes = 10 cases
button.text × 5 states × 2 themes = 10 cases
```

It fixes the fixture to Classic small size, default density and label-only
content. The structural suite carries nine assertions per case (90 located
assertions). Four renderer suites carry ten assertions per case; Outlined
carries thirteen to include outline role, alpha and width. Together they
provide 530 located assertions for effective colors, interaction
color/type/opacity, elevation and the variant-specific outline. The full
50-case suite remains seeded and unchanged; a vertical slice never partially
promotes it.

The visual contract reuses the full 50 logical cases and maps each to four
isolated targets: Windows Qt 5, Windows Qt 6, Linux Qt 6 and macOS Qt 6. This
defines 50 actual paths and 200 future reference paths. Phase 016.B supplies a
statically checked actual-only runner and closed output-manifest schema. Phase
016.C1 supplies closed reviewed-reference/report schemas plus a read-only exact
comparator tested with synthetic images. There are still no reference PNGs and
no visual pass claim.

Case identity is canonical and stable:

```text
suite + component id + state + theme
```

## Classifications

- `MUST_MATCH` is Material behavior expected to match within an explicit
  comparison tolerance where exact raster identity is inappropriate.
- `PLATFORM_ADAPTATION` is a deliberate Qt, desktop or platform difference and
  requires a rationale.
- `QTMATERIAL_EXTENSION` is functionality beyond Material. It is reported
  separately and cannot replace a missing Material requirement.

The comparison domains are geometry, tokens, colors, typography, states, motion,
accessibility and behavior. Typography compares the specification, not raw glyph
pixels across different font engines.

## Evidence rule

No numerical Material value may enter the model from memory. Every assertion must
cite one or more source IDs from its profile. Evidence-backed assertions also
carry a file and immutable line locator, and at least one locator must resolve
against a revision-pinned source. The Classic 1.4.0 source tree is pinned to
`5113b82a4ed6416d6344efa50e41db8cb65928ff`, the release tip linked by the
official AndroidX 1.4.0 notes.

A suite is either fully `seeded` (zero assertions in every case) or fully
`evidence-backed` (located evidence in every assertion of every case). Partial
evidence cannot be hidden behind either status.

## Repository gate

Run:

```bash
python tools/check_material_reference_model.py --root .
python tools/check_material_structural_conformance.py --root .
python tools/check_material_renderer_conformance.py --root .
python tools/check_material_visual_contract.py --root .
```

The checker verifies schemas, official source provenance, immutable AndroidX
coordinates, Classic/Expressive separation, catalog completeness, registry
bindings, the `referenceCandidate` anchor, all component axes, exhaustive
`ResolvedSpec` field mapping, canonical case IDs and deterministic generated
content. It is part of `tools/repo_health.py`.

The structural checker currently certifies only the first Filled Button fixture.
It verifies the registry binding, widget-to-resolver chain, component-token
scopes, five modeled slots, required state/theme coverage, Filled variant
identity and eight structural values carried by `ButtonSpec`. See
[Structural conformance](structural-conformance.md) for the exact boundary.

The renderer checker certifies semantic colors and disabled alpha, state-layer
and ripple ownership, all required state branches, exact per-state elevation
and renderer-only `ResolvedSpec` consumption for the Text, Filled, Filled
Tonal, Elevated and Outlined fixtures. See
[Renderer conformance](renderer-conformance.md).

The visual contract checker fixes the deterministic environment, target
isolation, state injection and zero-tolerance policy and audits both the private
capture source and read-only comparator for forbidden timing, grab,
reference-write and bypass paths. See
[Visual comparator](visual-comparator.md). Target execution, reviewed-reference
activation remain later increments of phase 016. Phase 017.A now consolidates
the current evidence without promoting missing results; see the
[Material conformance report](material-conformance-report.md).
