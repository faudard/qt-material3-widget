# Bottom App Bar Hardening Checklist

`QtMaterialBottomAppBar` is considered hardened when all of the following are true:

- it paints only from `BottomAppBarSpec`
- navigation, action, title, and FAB slots are spec-driven
- title layout is cached and elided correctly
- attached-FAB mode and non-FAB mode both render correctly
- keyboard focus and Enter/Return behavior are predictable
- accessible name follows the title when unset
- light/dark render baselines exist
- elevated and non-elevated states are visually distinct
- local build and tests pass on the integrated branch
