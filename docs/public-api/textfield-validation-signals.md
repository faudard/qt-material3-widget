# TextField validation signals

Text fields expose a lightweight signal contract for forms that need to enable or disable submit actions without polling the internal `QLineEdit`.

```cpp
connect(field, &QtMaterialOutlinedTextField::acceptableInputChanged,
        submitButton, &QPushButton::setEnabled);
```

The signal contract intentionally separates three concepts:

- `automaticValidationErrorChanged(bool)` tracks only validator/required feedback managed by the text field.
- `effectiveErrorStateChanged(bool)` tracks the rendered error state, including manual errors set with `setHasErrorState()`.
- `acceptableInputChanged(bool)` tracks whether the current value would pass the required/validator contract.

Manual error state changes do not imply that the text itself is unacceptable. This lets form-level code distinguish server-side or cross-field errors from local input validation failures.
