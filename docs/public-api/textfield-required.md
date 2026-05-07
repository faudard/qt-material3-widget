
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
