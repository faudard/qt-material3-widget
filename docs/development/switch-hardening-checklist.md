# QtMaterialSwitch hardening checklist

This pass upgrades `QtMaterialSwitch` from a correctness-first selection control to a stronger reference implementation for the switch branch.

## Goals
- keep the widget spec-driven
- cache size-dependent geometry
- keep transition state outside paint logic
- support label rendering without recomputing core geometry every frame
- keep ripple and focus behavior aligned with the rest of the selection family

## Expectations
- `paintEvent()` paints from `SwitchSpec` only
- track, handle, state-layer, and focus-ring rects are cached
- `sizeHint()` respects touch target and optional label width
- `Space` toggles the control through `QAbstractButton` semantics
- ripple is clipped to the active switch shape
- focus ring is spec-driven

## Follow-up checks
- profile repaint churn during rapid toggling
- verify transition progress updates repaint only the local widget
- tighten render baselines once the integration branch compiles cleanly
