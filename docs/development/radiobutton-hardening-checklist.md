# RadioButton Hardening Checklist

`QtMaterialRadioButton` should be treated as a real selection-family control, not a thin checkbox alias.

## Checklist

- [x] Uses `QtMaterialSelectionControl` as its behavioral base
- [x] Resolves `RadioButtonSpec` through `SpecFactory`
- [x] Uses cached indicator, dot, state-layer, focus-ring, and label geometry
- [x] Uses shared selection render helper for label font, state-layer opacity, and label painting
- [x] Clips ripple to the circular state-layer region
- [x] Supports keyboard toggle via Space
- [x] Provides spec-driven `sizeHint()` and `minimumSizeHint()`
- [x] Includes render-baseline scaffolding

## Remaining work

- [ ] Compile and reconcile against the current integrated branch
- [ ] Confirm auto-exclusive behavior policy in real usage
- [ ] Add accessibility name/state coverage if not already inherited cleanly
- [ ] Add dark-theme render baseline
