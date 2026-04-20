# Input render baselines

This directory stores golden images for input-family render tests.

Current scaffolding includes:

- `outlinedtextfield_default_light.png`
- `outlinedtextfield_error_light.png`

To update baselines locally:

```bash
QTMATERIAL3_UPDATE_BASELINES=1 ctest -R outlinedtextfield_render --output-on-failure
```

Keep the render environment stable when generating baselines:

- fixed DPI / scaling
- fixed Qt style if possible
- fixed font stack where practical
- same platform or CI image used for comparison
