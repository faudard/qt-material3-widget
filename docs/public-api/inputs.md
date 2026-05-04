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
