# TextField release readiness

This page documents the checklist used to promote the TextField family from `partial` to `usable`.

## Components

- `QtMaterialOutlinedTextField`
- `QtMaterialFilledTextField`

Both components use `TextFieldSpec` and share the same public interaction and validation contract.

## Public interaction contract

A usable TextField supports:

- label, placeholder, text and selected text operations
- supporting text and error text
- prefix and suffix text
- leading and trailing icons
- clear text action
- password visibility toggle
- custom trailing action
- read-only state
- dirty/modified state
- touched/interacted state
- right-to-left layout and mirrored accessories

## Validation contract

A usable TextField supports:

- `ValidationFeedbackMode::ManualOnly`
- `ValidationFeedbackMode::ValidatorOnEdit`
- `ValidationFeedbackMode::ValidatorOnCommit`
- required validation
- `RequiredValidationMode::NonEmpty`
- `RequiredValidationMode::NonBlank`
- validator-specific error text
- input-mask-specific error text
- manual error state
- effective error state
- visible error policy
- explicit validation commit through `validateInput()`
- reset of automatic validation feedback through `resetValidationFeedback()`

## Accessibility contract

A usable TextField keeps its accessible state synchronized with visible UI feedback:

- the label maps to the accessible name
- supporting text maps to the accessible description when no visible error is active
- visible error text is exposed in the accessible description
- hidden validation feedback is not announced prematurely
- clear, password and custom trailing actions have accessible names
- icon-only actions remain keyboard reachable

## Group validation contract

`QtMaterialTextFieldValidationGroup` is considered part of the form validation layer:

- fields can be added and removed
- destroyed fields are ignored
- `validateAll()` commits all field validation
- `resetValidationFeedback()` clears automatic feedback
- `focusFirstInvalidField()` focuses the first invalid field in insertion order
- validation summaries expose field labels and field-specific messages

## Release gate

Before keeping the registry entries as `usable`, run:

```bash
python3 tools/api-check/check_textfield_release.py
cmake --build build --target tst_outlinedtextfield tst_filledtextfield
ctest --test-dir build -R 'tst_.*textfield.*' --output-on-failure
```

The broad `ctest` pattern is intentional: it should catch the public widget tests, helper/layout tests, validation tests, accessibility tests and form-validation tests from the TextField patch series.
