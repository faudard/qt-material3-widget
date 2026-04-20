# TextField Helper/Error Polish Checklist

## Behavior
- Supporting text is shown when there is no active error text.
- Error text takes precedence over supporting text when error state is active.
- Error color comes from `TextFieldSpec::errorColor`.
- Disabled helper/error line uses disabled supporting color policy.
- `reserveSupportingLine` defaults to `true` to reduce layout jump.
- `sizeHint()` accounts for reserved supporting-line height consistently.

## Shared-shell rules
- Supporting/error visibility logic lives in the shared shell helper.
- Outlined and filled text fields must not fork supporting-line logic.
- Elided supporting/error text is cached and invalidated on content/width/font/spec changes.
- Supporting-line layout is driven by spec metrics only.

## Accessibility
- Accessible description prefers visible error text.
- If no error is visible, accessible description uses visible supporting text.
- Label remains accessible name when no explicit accessible name is set.

## Rendering
- Supporting/error baseline is aligned across variants.
- Focused/error/default render states exist for outlined and filled variants.
- No raw theme lookups in text-field paint code.
