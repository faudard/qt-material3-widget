# Structural conformance

Phase 014.A introduces a strict static comparator for the first Material
reference vertical. Its current result is intentionally narrow:

```text
Classic / Filled Button / small / default density / label-only
5 states × 2 themes = 10 cases
```

## What is checked

The comparator reads the evidence suite, component model, product registry and
active C++ sources. It verifies:

- the `button.filled` registry entry, public widget and `ButtonSpec` binding;
- the widget → `ButtonSpecResolver::filledButtonSpec` resolution chain;
- application of the common `Button` and variant `ButtonFilled` token scopes;
- container, label, optional leading-icon, state-layer and focus-indicator slots;
- the required enabled, disabled, hovered, focused and pressed states in both
  light and dark cases;
- Filled variant identity, 40 dp container height, 24 dp label-only horizontal
  padding, 18 dp icon size, 8 dp icon spacing, full shape, Label Large typography
  and a 48 × 48 dp touch target.

The expected values come from immutable AndroidX Compose Material3 1.4.0 source
locators. The checker extracts the product values from the active header and
source chain; it has no baseline, allowlist, suppressions or bypass flag.

Run:

```bash
python tools/check_material_structural_conformance.py --root .
```

The command is part of `tools/repo_health.py`, so repository health fails when a
reference expectation, `ButtonSpec` initializer, token scope or required render
slot drifts.

## Deliberate boundary

This structural gate does not certify semantic colors, disabled alpha,
state-layer opacity, per-state elevation, motion, arbitrary icon content or pixel
rendering. Semantic renderer tokens and states are now certified by the separate
[Renderer conformance](renderer-conformance.md) gate. Runtime Qt layout and
pixels still require phases 016–017. Therefore a passing 014.A gate means
“structure OK for the named fixture”, not “Filled Button is fully
Material-conformant”. The registry-wide
[Material conformance report](material-conformance-report.md) preserves this
boundary: Filled is the only structural pass and the other modeled Button
variants remain `not-evaluated` for structure.
