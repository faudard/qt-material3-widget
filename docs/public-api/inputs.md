# Inputs API

Input family:
- `QtMaterialOutlinedTextField`
- `QtMaterialFilledTextField`
- `QtMaterialAutocompletePopup`
- `QtMaterialDateField`

### TextField RTL contract

TextField layout and painting must honor `QWidget::layoutDirection()` consistently.
For right-to-left widgets, leading accessories are placed on the visual right side,
trailing accessories are placed on the visual left side, and label/supporting/error
text is painted with right alignment inside its reserved text rect.


### Character counter

`QtMaterialOutlinedTextField` and `QtMaterialFilledTextField` expose a lightweight character counter API:

```cpp
field->setMaxLength(40);
field->setCharacterCounterEnabled(true);
```

When enabled, the counter is rendered as a right-aligned supporting-line label. The supporting/error text rectangle is reduced before painting so the counter does not overlap helper or error copy. `maxLength()` delegates to the internal `QLineEdit`, so overly long input is still constrained by Qt itself.

### Required TextField

`QtMaterialOutlinedTextField` and `QtMaterialFilledTextField` can mark a field as
required through `setRequired(bool)` / `isRequired()`.

Required validation is treated as automatic validation. It is evaluated by
`ValidatorOnEdit` and `ValidatorOnCommit`, but it does not override manual error
mode. Empty text is considered valid when `isRequired()` is false, which preserves
the existing optional-field behaviour.

The required error message defaults to `Required` and can be customized with
`setRequiredText(QString)`. When the field is required, empty, and currently in
an automatic error state, the required message becomes the effective error text
for rendering and accessibility.

```cpp
QtMaterial::QtMaterialOutlinedTextField field;
field.setLabelText(QStringLiteral("Email"));
field.setRequired(true);
field.setRequiredText(QStringLiteral("Email is required."));
field.setValidationFeedbackMode(
    QtMaterial::QtMaterialOutlinedTextField::ValidationFeedbackMode::ValidatorOnEdit);
```

### Read-only TextField

`QtMaterialOutlinedTextField` and `QtMaterialFilledTextField` expose a read-only
state through `setReadOnly(bool)` / `isReadOnly()`.

A read-only TextField stays enabled: it can still receive focus, expose its text
to accessibility tools, and allow selection/copy through the embedded
`QLineEdit`. It differs from the disabled state, which communicates that the
control is unavailable.

The clear-text end action is hidden in read-only mode because it would mutate
the field value. The password visibility toggle remains available because it
does not change the stored text.

```cpp
QtMaterial::QtMaterialOutlinedTextField field;
field.setText(QStringLiteral("Generated identifier"));
field.setReadOnly(true);
```

## TextField touched state

`QtMaterialOutlinedTextField` and `QtMaterialFilledTextField` expose a lightweight
`touched` flag for form interaction tracking:

- `isTouched()`
- `setTouched(bool)`
- `resetTouched()`
- `touchedChanged(bool)`

The flag is intentionally separate from `isModified()`. Programmatic `setText()`
does not mark the field as touched. The field becomes touched when the embedded
line edit emits `textEdited()` or when it receives `FocusOut` through the wrapper's
event filter. This is useful for forms that want to postpone visible validation
until the user has interacted with a field at least once.

<!-- textfield-release-readiness:start -->
## Text fields release status

`QtMaterialOutlinedTextField` and `QtMaterialFilledTextField` are registry-level `usable` components once the TextField patch series has been applied.

The usable contract covers:

- label, placeholder, supporting text and error text
- prefix/suffix text and leading/trailing icons
- clear action, password visibility action and custom trailing action
- required validation, validator feedback, input mask feedback and manual errors
- dirty, touched and explicit validation commit flows
- accessibility descriptions synchronized with visible validation feedback
- grouped form validation and validation summaries

See `docs/public-api/textfield-release-readiness.md` for the checklist used to promote the components.
<!-- textfield-release-readiness:end -->

## Date field

`QtMaterialDateField` extends the outlined text field with date parsing, date
formatting, clear/calendar affordances, minimum/maximum date validation and an
accessibility summary for the selected date and range.

