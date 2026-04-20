# QtMaterialList Patch Direction

This pass introduces a list container scaffold intended to compose `QtMaterialListItem`
instances cleanly without committing yet to a model/view architecture.

## Responsibilities
- own vertical composition of `QtMaterialListItem`
- expose current-item navigation and activation semantics
- provide focus-ring/container rendering
- optionally render dividers between items
- remain compatible with a future model-backed list layer

## Suggested implementation notes
- start with `QVBoxLayout` composition
- install an event filter on each child item
- update current index on focus/click/activation
- handle Up/Down/Home/End keyboard navigation
- paint only the list container shell, not the child items themselves

## Suggested future path
Once the widget family is stable, consider:
- a model-backed list container
- virtualization for long lists
- selection modes
- trailing action handling
- grouped/subheader support
