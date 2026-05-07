# TextField validation hardening

This patch tightens the TextField validation contract without changing the public rendering model.

## Required fields

`QtMaterialOutlinedTextField` now supports a required mode with explicit blank handling:

```cpp
field->setRequired(true);
field->setRequiredValidationMode(
    QtMaterialOutlinedTextField::RequiredValidationMode::NonBlank);
field->setRequiredText(tr("Email is required"));
```

Modes:

- `NonBlank`: empty strings and whitespace-only strings are invalid.
- `NonEmpty`: only the empty string is invalid; whitespace-only strings are accepted.

`NonBlank` is the default because it is the safer form behavior for most user-facing fields.

## Typed error messages

Validation errors can now distinguish between required, validator and input-mask failures:

```cpp
field->setValidatorErrorText(tr("Use three digits"));
field->setInputMaskErrorText(tr("Enter four digits"));
```

`effectiveErrorText()` resolves the message that should be rendered and exposed to accessibility consumers.
The priority is:

1. manual error text;
2. required error text;
3. input-mask error text;
4. validator error text;
5. fallback invalid-value text.

## Optional empty fields

Optional empty fields remain valid. This preserves the existing TextField behavior while allowing explicit required validation when needed.

## Integration tests

The patch always adds `tst_textfield_validation_hardening`.

If the earlier error-display-policy patch is present, the script also adds an accessibility/visibility integration test.
If the validation-group patch is present, the script also adds lifecycle tests for field removal/destruction.
