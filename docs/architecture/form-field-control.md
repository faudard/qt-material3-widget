# Form field base contract

Text fields, date fields, autocomplete fields and future picker fields should share one form-field contract.

The base class is `QtMaterial::QtMaterialFormFieldControl`.

Common properties:

- `label`
- `helperText`
- `errorText`
- `required`
- `invalid`
- `readOnly`

This prevents each input widget from inventing slightly different property names or validation behavior.

Release rule: every field-like widget should either inherit this base class or explicitly document why it does not.
