# Visual smoke contracts

This document defines the first release-level visual smoke tests for QtMaterial3Widgets.

These tests are intentionally lighter than pixel-perfect visual regression. Their goal is to catch severe rendering regressions early:

- widgets that crash during rendering;
- widgets that render to an empty image;
- layout paths that produce invalid dimensions;
- obvious RTL or high-DPI rendering failures.

## Scope

The visual smoke suite covers a representative page made from:

- filled, outlined, and text buttons;
- outlined text fields;
- checkbox, radio button, and switch;
- linear and circular progress indicators;
- tabs/navigation content.

Each page is rendered in:

- left-to-right layout;
- right-to-left layout;
- high-DPI rendering with device pixel ratio `2.0`.

## Non-goals

This suite does not compare against golden screenshots yet. It does not enforce exact colors, shadows, typography metrics, or pixel positions.

Those checks should come later in a dedicated visual regression system with explicit baselines per platform, Qt version, theme mode, density, and DPI scale.

## Release expectation

For a release candidate, this suite must pass together with the keyboard/accessibility and migration contract tests:

```bash
ctest --test-dir build -R "visual_smoke|keyboard_accessibility|navigation_dialog_hidpi|widget_migration" --output-on-failure
```

## Future work

The next visual QA step is to generate stable screenshot baselines for:

- light and dark mode;
- standard, medium, and high contrast;
- comfortable and compact density;
- disabled, hovered, pressed, focused, selected, error, and busy states;
- high-DPI factors `1.25`, `1.5`, and `2.0`;
- Windows, Linux, and macOS separately when required by font or style differences.
