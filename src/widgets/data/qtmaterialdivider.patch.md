# QtMaterialDivider Patch Direction

This pass introduces a focused divider primitive for the data/display family.

## Responsibilities
- render a spec-driven horizontal or vertical divider
- support leading/trailing inset behavior
- remain cheap to paint
- stay usable inside list items, forms, cards, and app surfaces

## Suggested implementation notes
- divider should remain lightweight and stateless beyond spec/inset/orientation
- paint should use a single fill rect or line rect derived from spec
- no effect controllers are needed
- use `sizeHint()` to advertise thickness in the oriented dimension
- orientation and insets should invalidate layout and repaint

## Suggested usage
- section separators in forms
- list separators
- card content separators
- app bar / sheet section dividers

## Future path
- inset presets
- semantic divider roles if needed
- grouped list integration
