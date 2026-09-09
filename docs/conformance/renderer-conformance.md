# Renderer conformance

Phases 015.A-E activate a strict semantic comparator for every required Button
renderer vertical. They share the same deterministic fixture:

```text
Classic / Text + Filled + Filled Tonal + Elevated + Outlined Button / small / default density / label-only
5 variants × 5 states × 2 themes = 50 cases
```

## What is checked

The five renderer suites contain 530 assertions located in the immutable
AndroidX Compose Material3 1.4.0 source tree. Text, Filled, Filled Tonal and
Elevated carry ten assertions per case; Outlined carries thirteen. For each
variant, state and theme they check:

- effective container, label and icon semantic color roles and alpha;
- `OnSurface` at 10% for the Filled/Elevated disabled container and 12% for
  Filled Tonal;
- transparent enabled and disabled Text/Outlined containers;
- disabled label and icon content at 38%: `OnSurfaceVariant` for Text,
  Filled, Elevated and Outlined, and `OnSurface` for Filled Tonal;
- the variant interaction role (`OnPrimary` for Filled, `OnSecondaryContainer`
  for Filled Tonal, `Primary` for Text/Elevated and `OnSurfaceVariant` for
  Outlined) at 8% hover and 10% focus/press;
- no interaction feedback while idle or disabled;
- fixed state-layer ownership for hover/focus and ripple ownership for press;
- Filled and Filled Tonal elevation `Level1` only on hover and `Level0`
  otherwise;
- Elevated elevation `Level1` at rest/focus/press, `Level2` on hover and
  `Level0` while disabled;
- Outlined transparent containers, `OnSurfaceVariant` content, a flat `Level0`
  elevation, and an `OutlineVariant` outline at 1 dp;
- Outlined disabled content at 38% and disabled outline at 10%;
- Text `Primary` content, flat `Level0` elevation and no outline slot.

The product side is extracted from the active resolver, component-token applier,
global state tokens and renderer sources. The gate also verifies that
`paintEvent()`, the elevation selector and private render helpers do not read
theme roles or encode Material semantic values. The shared Filled-derived paint
path consumes container/content/state/elevation/focus data through `ButtonSpec`.
The distinct Outlined paint path is also certified and consumes outline color,
width, content, feedback and focus data through the same resolved spec. Normal
and hover elevation styles are separately resolved so renderers never
approximate one Material elevation level from another.
The distinct Text paint path consumes transparent/custom container, independent
label/icon, interaction and focus values through `ButtonSpec`; typed Text
overrides preserve the 38% disabled alpha and cannot inject an outline role.

Press does not paint a fixed state layer and a ripple simultaneously. Disabled
state suppresses a transitioning fixed layer, any queued ripple and the focus
ring.

Run:

```bash
python tools/check_material_renderer_conformance.py --root .
```

The command runs through `tools/repo_health.py`. It has no debt file, exception
list or alternate pass mode; a source or evidence mismatch fails repository
health.

## Deliberate boundary

This phase is static and certifies `button.text`, `button.filled`,
`button.filled-tonal`, `button.elevated` and `button.outlined` with the named
fixture. It does not certify other component families, timing curves, ripple
raster evolution, arbitrary icon content, Qt 5/Qt 6 pixels or platform
rendering. Phase 016.A defines their deterministic contract and isolated target
inventory; phase 016.B adds the statically certified actual-only runner and
phase 016.C1 adds the read-only exact [Visual comparator](visual-comparator.md).
Runtime evidence still belongs to the next target-execution increment and the
consolidated [Material conformance report](material-conformance-report.md)
keeps rendering pending until that evidence exists.
