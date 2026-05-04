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
