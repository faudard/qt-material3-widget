# Visual smoke contracts

Visual smoke tests are the lightweight rendering guardrail for QtMaterial3.
They complement, rather than replace, the maintained visual-regression harness.

Their goal is to catch severe rendering regressions early:

- widgets that crash during rendering;
- widgets that render to an empty image;
- layout paths that produce invalid dimensions;
- obvious RTL or high-DPI rendering failures.

## Scope

`tst_visual_smoke_contracts` renders representative controls including buttons,
text fields, selection controls, progress indicators, and tabs/navigation
content. Coverage includes left-to-right, right-to-left, and DPR 2.0 rendering.

The related `tst_navigation_dialog_hidpi_contracts` suite exercises navigation,
dialogs, accessibility, RTL, and high-DPI integration paths.

## Relationship to visual regression

Pixel/golden regression is maintained separately by
`tst_theme_visual_regression`.

That harness covers deterministic token-board snapshots and representative
component-grid rendering. Strict pixel comparison remains opt-in because native
font and rasterization output varies across platforms.

See [Visual regression testing](../public-api/visual-regression.md) for golden
update and strict-comparison commands.

## Release expectation

For a release candidate, run the smoke, accessibility, and visual-regression
contracts together:

```bash
ctest --test-dir build --output-on-failure \
  -R "tst_visual_smoke_contracts|tst_keyboard_accessibility|tst_navigation_dialog_hidpi_contracts|tst_theme_visual_regression"
```

The smoke suites validate catastrophic rendering/layout failures; the visual
regression suite protects intentional visual baselines.
