# QtMaterialListItem Hardening Checklist

Use this checklist when integrating `QtMaterialListItem`.

## API
- [ ] Headline text API exists
- [ ] Supporting text API exists
- [ ] Leading/trailing icon API exists
- [ ] Selection API exists
- [ ] Divider visibility API exists
- [ ] Density variant API exists

## Rendering
- [ ] Paint uses resolved `ListItemSpec`
- [ ] Layout is cached
- [ ] State-layer treatment is shared and consistent
- [ ] Focus ring is visible and shape-correct
- [ ] Divider paint is spec-driven
- [ ] Disabled colors are consistent

## Interaction
- [ ] Mouse click emits `clicked()`
- [ ] Enter/Return/Space activate the item
- [ ] Selected state is keyboard-discernible
- [ ] Accessible name follows headline when unset

## Sizing
- [ ] Touch-target-aware height
- [ ] One-line and two-line sizing are stable
- [ ] Density variant influences min height consistently

## Testing
- [ ] Unit tests for selection and activation
- [ ] Render baselines for one-line, two-line, selected states
