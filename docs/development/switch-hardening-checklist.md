# QtMaterialSwitch hardening checklist (pass 2)

This pass tightens `QtMaterialSwitch` beyond the first hardening step.

## Added in this pass

- cached ripple clip path
- cached focus-ring path
- cached label font
- `contentChangedEvent()` invalidation path
- left/right keyboard semantics for explicit on/off behavior
- better accessibility metadata sync for label and checked state

## Remaining follow-up

- profile repaint region during animated toggles
- validate RTL arrow-key behavior on a real build
- move focus-ring width to spec if switch family needs finer control
- add snapshot baselines for checked, unchecked, disabled, and dark mode
