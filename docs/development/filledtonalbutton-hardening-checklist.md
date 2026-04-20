# QtMaterialFilledTonalButton Hardening Checklist

- Derive from `QtMaterialFilledButton`
- Override only spec resolution
- Reuse cached layout/path behavior from the filled-button path
- Verify tonal color roles come from `filledTonalButtonSpec()`
- Verify keyboard activation
- Verify touch-target height in `sizeHint()`
- Add default and disabled render baselines
