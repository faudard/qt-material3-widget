# QtMaterialListItem Patch Direction

`QtMaterialListItem` should be a reusable, clickable row primitive.

## Intended behavior
- headline-only and two-line variants
- optional leading icon
- optional trailing icon
- selectable state
- keyboard activation with Space/Enter
- focus ring
- state layer
- optional divider
- touch-target-aware sizing

## Recommended architecture
- derive from `QtMaterialControl`
- use `ListItemSpec`
- use a shared private render helper
- keep layout cached
- emit `clicked()` and `activated()`

## Future extensions
- trailing widgets/actions
- avatar/media leading slot
- checkbox/switch trailing controls for settings rows
- list container integration
