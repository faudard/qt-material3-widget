# Text Field Validation Patterns Checklist

Use this checklist when wiring shared validation support into outlined, filled, and date-field shells.

## API
- [ ] Shared validation result type exists
- [ ] Shared validator interface exists
- [ ] Validation trigger mode is explicit
- [ ] Widgets expose `validateNow()`
- [ ] Widgets expose current validation result
- [ ] Widgets emit `validationChanged(...)`

## Shell behavior
- [ ] Validation is implemented in the shared input shell, not per-widget
- [ ] Error messages override helper/supporting text
- [ ] Warning/info/success messages follow a consistent precedence rule
- [ ] Reserved supporting-line height prevents layout jump when configured
- [ ] Validation state can be cleared cleanly

## Trigger behavior
- [ ] Manual mode works
- [ ] Blur validation works
- [ ] Submit validation works
- [ ] Debounced validation is bounded and cancel-safe

## Visual behavior
- [ ] Error severity drives error outline treatment
- [ ] Non-error validation messages use their own semantic colors
- [ ] Disabled state overrides validation visuals appropriately
- [ ] Focus state and validation state compose cleanly
- [ ] Render baselines exist for neutral/warning/success/error

## Accessibility
- [ ] Validation message is reflected in accessible description or equivalent path
- [ ] Error state is keyboard-discernible
- [ ] Focus remains on the editor after validation unless explicitly redirected

## Testing
- [ ] Unit tests cover validators
- [ ] Widget tests cover triggers and state transitions
- [ ] Render tests cover neutral/warning/success/error
- [ ] Date-field validation is covered independently

## Architecture
- [ ] No widget-local validation frameworks
- [ ] No direct theme lookups in validation paint code
- [ ] Validation state maps to spec-driven shell rendering
