# QtMaterialList Hardening Checklist

Use this checklist when turning the list scaffold into a real widget.

## API
- [ ] Add/insert/take/clear item API is stable
- [ ] Current-index API is explicit
- [ ] Activation signal path is clear
- [ ] Divider toggle API is clear

## Behavior
- [ ] Up/Down navigation works
- [ ] Home/End navigation works
- [ ] Focus moves predictably between items
- [ ] Current item updates on focus and click
- [ ] Disabled items are handled correctly

## Rendering
- [ ] Container shell is spec-driven
- [ ] Focus ring is spec-driven
- [ ] Dividers are optional and consistent
- [ ] Child items own their own row rendering
- [ ] Render baselines exist for neutral/focused/divided states

## Architecture
- [ ] List container does not re-paint child item content
- [ ] List uses public `QtMaterialListItem` behavior cleanly
- [ ] No widget-local magic metrics
- [ ] Future model-backed path remains possible

## Accessibility
- [ ] Current item is keyboard-discernible
- [ ] Activation path is keyboard-accessible
- [ ] Accessible name/description strategy is clear
