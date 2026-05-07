# Navigation Drawer release readiness

`QtMaterialNavigationDrawer` is promoted to `usable` once it provides a real destination model instead of only an overlay shell.

## Public contract

The drawer supports:

- left/right edge placement;
- open/close lifecycle with `opened`, `closed` and `openChanged`;
- optional Escape dismissal via `dismissOnEscape`;
- optional focus restoration via `restoreFocusOnClose`;
- destination insertion/removal/clear;
- current destination tracking;
- disabled destinations;
- mouse activation;
- keyboard navigation with Up, Down, Left, Right, Home and End;
- activation with Space, Return and Enter;
- destination and drawer accessibility summaries.

## Accessibility

The drawer exposes a default accessible name of `Navigation drawer` and synchronizes its accessible description with `accessibilitySummary()`.

Each destination exposes text in this shape:

```text
<label>, <supporting text>, <n> of <count>, selected, disabled
```

Only applicable states are included.

## Keyboard behavior

| Key | Behavior |
| --- | --- |
| Up / Down | Move to previous / next enabled destination |
| Left / Right | Direction-aware previous / next movement |
| Home / End | Move to first / last enabled destination |
| Space / Return / Enter | Activate current destination |
| Escape | Close drawer when `dismissOnEscape` is true |

## Release checks

```bash
cmake --build build --target tst_navigationdrawer
ctest --test-dir build -R tst_navigationdrawer --output-on-failure
python3 tools/api-check/check_navigationdrawer_release.py
```
