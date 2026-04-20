# Button render baselines

This directory stores PNG baselines for button render regression tests.

Generate or update baselines by running the render tests with:

```
QTMATERIAL3_UPDATE_BASELINES=1 ctest -R filledbutton_render
```

Committed baselines should be captured with a fixed Qt version, DPI, and font environment where possible.

Expected baseline files may include:

- filledbutton_default_light.png
- filledbutton_disabled_light.png
- outlinedbutton_default_light.png
- outlinedbutton_disabled_light.png
