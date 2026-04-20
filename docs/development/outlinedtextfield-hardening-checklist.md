# Outlined Text Field Hardening Checklist

This pass focuses on keeping `QtMaterialOutlinedTextField` stable as a shared-shell input control.

## Goals in this pass

- invalidate shell layout when label, supporting text, or error state changes
- keep `QLineEdit` geometry and palette synced with shell state
- keep focused, error, and default render baselines distinct
- preserve focus forwarding and accessibility metadata

## Checklist

- [x] content changes invalidate cached shell layout
- [x] focus continues to forward to the hosted `QLineEdit`
- [x] accessibility name and description follow label/supporting/error text
- [x] focused render baseline scaffold exists
- [x] style and palette changes invalidate shell layout/palette

## Remaining work

- verify local compile/build against the integrated branch
- tighten repaint regions if shell transitions repaint too much
- consider caching separate typography variants if theme typography begins to diverge per role
