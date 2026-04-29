# Async and progress state

Components that can represent pending work should share one state contract.

The shared value type is `QtMaterial::QtMaterialAsyncState`.

It supports:

- `busy`
- `indeterminate`
- determinate `progress` in `[0, 1]`
- `statusText`

Target consumers:

- loading buttons
- progress indicators
- busy dialogs
- form submit controls
- snackbar pending actions

Release rule: user-visible busy states need accessible status text when no visible text explains the operation.
