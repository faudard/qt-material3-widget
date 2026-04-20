# QtMaterialAutocompletePopup Hardening Checklist

This checklist tracks the work needed to make `QtMaterialAutocompletePopup` production-ready.

## Behavior

- [ ] Anchors correctly to the hosted `QLineEdit`
- [ ] Repositions on anchor move/resize/show/hide
- [ ] Filters suggestions from shared text-field shell input
- [ ] Supports keyboard navigation: Up/Down/Enter/Escape/Tab policy
- [ ] Emits `completionActivated(...)` consistently
- [ ] Handles empty model and empty results gracefully

## Rendering

- [ ] Uses `AutocompletePopupSpec` only for paint metrics/colors
- [ ] Uses shared surface-style painting where practical
- [ ] Selected row treatment is spec-driven
- [ ] Dividers, supporting text, and icons are spec-driven
- [ ] Focus ring is correct for popup and selected row semantics

## Integration

- [ ] Outlined text field can host popup
- [ ] Filled text field can host popup
- [ ] Prefix/suffix content does not overlap popup alignment
- [ ] Helper/error/supporting text does not break popup geometry

## Accessibility

- [ ] Exposes a sensible accessible role/name path
- [ ] Selection changes are understandable to assistive tech
- [ ] Keyboard-only usage is complete

## Performance

- [ ] Popup geometry is not recomputed unnecessarily
- [ ] Item rendering does not allocate excessively
- [ ] Filtering path is measured for large suggestion sets
