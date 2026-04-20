# Top App Bar Hardening Checklist

`QtMaterialTopAppBar` is considered hardened when all of the following are true:

- it paints only from `TopAppBarSpec`
- navigation and action slots are spec-driven
- title layout is cached and elided correctly
- keyboard focus and Enter/Return behavior are predictable
- accessible name follows the title when unset
- light/dark render baselines exist
- elevated and non-elevated states are visually distinct
- local build and tests pass on the integrated branch
