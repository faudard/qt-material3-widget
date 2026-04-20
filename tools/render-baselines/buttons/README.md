# Button Render Baselines

This directory stores PNG baselines for button render regression tests.

Suggested files for `QtMaterialFilledTonalButton`:

- `filledtonalbutton_default_light.png`
- `filledtonalbutton_disabled_light.png`

To update baselines locally, run the render tests with:

```bash
QTMATERIAL3_UPDATE_BASELINES=1 ctest --output-on-failure -R filledtonalbutton_render
```
