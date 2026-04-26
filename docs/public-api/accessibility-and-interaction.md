# Accessibility and interaction correctness

The theming layer now carries explicit accessibility and interaction policy tokens.
These tokens are resolved with every `Theme` and are intended to be consumed by
specs/widgets before falling back to hardcoded focus, disabled, hover, or motion
values.

## Tokens

- `FocusRingTokens`: width, offset, radius adjustment, color, and opacity.
- `AccessibilityTokens`: high-contrast state, reduced-motion state, minimum text
  contrast, minimum UI contrast, and the focus ring.
- `InteractionStateTokens`: keyboard focus visibility plus hover, press, and drag
  feedback policy.

`ContrastMode::High` resolves stronger accessibility defaults:

- text contrast target: at least 7.0:1;
- UI contrast target: at least 4.5:1;
- focus ring width: at least 3 logical pixels.

## System integration

`SystemTheme` exposes high-contrast availability and state when Qt exposes native
accessibility hints. Reduced motion is represented as a theme token and explicit
application policy hook; Qt Widgets does not currently expose a universal
cross-platform reduced-motion system preference through `QStyleHints`.

## Tests

The slice adds:

- contrast-ratio utility tests;
- high-contrast token-resolution tests;
- reduced-motion token tests;
- system accessibility bridge tests;
- keyboard-focus and accessible-name smoke tests for common controls.

## Theme Studio

The Theme Studio includes an Accessibility tab with high-contrast, reduced-motion,
contrast-ratio, disabled-state, and keyboard-focus samples.
