## Text field accessories and validation

`QtMaterialOutlinedTextField` and `QtMaterialFilledTextField` share the same functional contract for common Material text field behaviour:

- prefix and suffix text through `setPrefixText()` and `setSuffixText()`;
- leading and trailing decorative icons through `setLeadingIcon()` and `setTrailingIcon()`;
- clear-text, password-visibility, and custom trailing actions through `EndActionMode`;
- manual error state through `setHasErrorState()`;
- validator-driven feedback through `ValidationFeedbackMode`.

For accessibility, icon-only or action-only affordances must expose a stable accessible name. The built-in clear action reports `Clear text`; the password action reports `Show password` or `Hide password`; custom trailing actions use the visible action text as the accessible name and the tooltip as the accessible description.

When a text field has both supporting text and an active error text, assistive technologies should receive both strings in the accessible description so the error does not hide the field guidance.
