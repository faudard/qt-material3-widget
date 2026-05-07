# TextField validation commit helpers

This patch adds two form-oriented helpers to `QtMaterialOutlinedTextField`.

## API

```cpp
bool validateInput();
void resetValidationFeedback();

Q_SIGNAL void validationRequested(bool acceptable);
Q_SIGNAL void validationFeedbackReset();
```

## Behavior

`validateInput()` forces validation as if the field had been committed, makes automatic validation errors visible, refreshes layout/accessibility state, emits `validationRequested(bool)`, and returns whether the field is currently acceptable.

`resetValidationFeedback()` clears automatic validation feedback and commit visibility. It does not clear manually forced errors set through `setHasErrorState(true)`.

This is intended for form submit flows:

```cpp
const bool ok =
    emailField->validateInput() &&
    passwordField->validateInput();

if (!ok) {
    return;
}
```

The helper is especially useful with `ErrorDisplayMode::AfterCommit`, where validation errors are real but not shown until explicit submit/commit.
