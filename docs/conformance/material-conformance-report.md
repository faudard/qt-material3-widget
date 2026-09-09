# Material conformance report

Phase 017.A assembles the reference-model, structural, renderer and visual
contract gates into one deterministic component report. It answers the same
four questions for every entry in the public component registry without
turning missing evidence into a pass:

| Headline | Evidence used | Current meaning of `pass` |
|---|---|---|
| Structure | structural suite and static product comparator | slots, sizes, padding, shape and structural token paths match the named fixture |
| Tokens | non-state renderer assertions and static product comparator | semantic colors, geometry and other renderer token values match the suite |
| States | renderer state assertions and static product comparator | every required state branch in the named fixture matches the suite |
| Rendering | semantic renderer result plus target visual result | both semantics and reviewed runtime pixels pass |

The generated report is:

```text
references/material3/classic/compose-material3-1.4.0/reports/buttons.json
```

Its closed schema is
`references/material3/schema/material-conformance-report.schema.json`.
Components always follow `docs/components/component-registry.json` order.

## Current 017.A result

| Registry group | Components | Structure | Tokens | States | Rendering | Overall |
|---|---:|---|---|---|---|---|
| Filled Button | 1 | pass | pass | pass | pending runtime | incomplete |
| Other modeled Buttons | 4 | not evaluated | pass | pass | pending runtime | incomplete |
| No Material model yet | 27 | not modeled | not modeled | not modeled | not modeled | not modeled |

The report therefore records 32 components, 5 modeled components, 0 fully
conformant components, 5 incomplete components and 27 not modeled components.
The headline pass counts are structure 1, tokens 5, states 5 and rendering 0.

The five Button renderer suites contribute 530 semantic assertions. Token
reporting excludes their 150 state-selection assertions, producing 70 token
assertions for Text, Filled, Filled Tonal and Elevated and 100 for Outlined.
The separate states dimension reports 30 assertions per variant. Only Filled
has the 90-assertion structural suite.

All four visual target sets remain `pending`. A valid deterministic visual
contract proves only that capture and comparison are well-defined; it does not
prove a pixel result. Every modeled Button therefore reports
`visualRendering.status = pending-runtime`, zero passed targets and
`overallStatus = incomplete`.

## Status rules

- `pass` requires evidence plus a successful owning gate.
- `fail` is propagated when an owning source gate fails.
- `pending-runtime` means the static contract exists but reviewed target
  execution has not been ingested.
- `not-evaluated` means the component is modeled but lacks that evidence suite.
- `not-modeled` means no active Material component model covers the registry
  entry.

An aggregate gate pass only means its own source contract is sound. In
particular, `visual-contract: pass` cannot promote a component's visual result.
The root becomes `complete` only when every registry component is fully
conformant; any propagated source-gate failure makes it `failed`.

## Generate and validate

Run:

```bash
python tools/generate_material_conformance_report.py --root . --check
python tools/check_material_conformance_harness.py --root .
```

The generator invokes the four checker Python APIs directly and serializes the
result deterministically. The harness independently validates the schema,
generated freshness, complete registry order, current modeled boundary,
evidence counts and honest pending/not-modeled states. It also audits its source
chain and has no alternate success path. The harness runs through
`tools/repo_health.py` immediately after the four source gates.

## Deliberate boundary

017.A aggregates existing evidence; it does not create new Material evidence.
The next runtime increment remains 016.C2: execute one exact declared target,
review and atomically activate its complete reference set, run the read-only
comparator, then add a comparison-result ingestion contract before any visual
status can become `pass`. Structural suites for the other Button variants and
Material models for the other 27 components remain separate incremental work.
