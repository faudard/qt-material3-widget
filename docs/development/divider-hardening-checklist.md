# QtMaterialDivider Hardening Checklist

Use this checklist when turning the divider scaffold into a real widget.

## API
- [ ] Orientation API is explicit
- [ ] Leading/trailing inset API is explicit
- [ ] Size hints reflect orientation and thickness
- [ ] Divider is safe to use as a lightweight primitive

## Rendering
- [ ] Divider is spec-driven
- [ ] Horizontal and vertical paths both render correctly
- [ ] Insets are applied correctly in LTR and RTL contexts if applicable
- [ ] Divider remains visually stable at 1 px / 1 dp usage
- [ ] Render baselines exist for horizontal/vertical/inset states

## Architecture
- [ ] Divider does not pull in unnecessary heavy helpers
- [ ] No widget-local magic metrics
- [ ] Divider remains usable inside list/forms/cards/app bars

## Accessibility
- [ ] Divider is ignored or treated appropriately by focus navigation
- [ ] Divider does not interfere with keyboard flow
