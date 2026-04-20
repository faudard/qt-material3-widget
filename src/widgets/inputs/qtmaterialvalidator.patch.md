# Validation Patterns Patch Direction

This patch direction introduces shared validation concepts for text-field-like widgets.

## New shared types
- `QtMaterial::ValidationSeverity`
- `QtMaterial::ValidationTrigger`
- `QtMaterial::ValidationResult`
- `QtMaterial::Validator`
- `QtMaterial::RegexValidator`
- `QtMaterial::MinLengthValidator`
- `QtMaterial::CompositeValidator`

## Intended widget API additions

Add to `QtMaterialInputControl` or the shared text-field shell:

```cpp
ValidationTrigger validationTrigger() const noexcept;
void setValidationTrigger(ValidationTrigger trigger);

const ValidationResult& validationResult() const noexcept;
void setValidator(std::shared_ptr<QtMaterial::Validator> validator);
void validateNow();
void clearValidationState();
```

Add these signals:

```cpp
signals:
    void validationChanged(const QtMaterial::ValidationResult& result);
    void validationRequested();
```

## Behavior model

### Manual
- no automatic validation
- caller invokes `validateNow()`

### OnBlur
- validate when focus leaves the editor

### OnSubmit
- validate on Enter/Return or an explicit submit action

### OnInputDebounced
- validate after a short debounce delay when text changes

## Shared shell rules

Validation drives the visible supporting line by precedence:

1. error message if `ValidationSeverity::Error`
2. warning/info/success message if configured to show validation feedback
3. helper/supporting text otherwise

### Recommended shell-state mapping
- `Error` -> error colors and error outline treatment
- `Warning` -> warning supporting-line color, no error outline by default
- `Success` -> success supporting-line color, keep neutral outline unless explicitly designed otherwise
- `Info` -> info supporting-line color, no error outline
- `None` -> normal helper/supporting behavior

## Suggested `TextFieldSpec` growth

Add:

```cpp
QColor warningTextColor;
QColor successTextColor;
QColor infoTextColor;
bool showNonErrorValidationMessages = true;
int validationDebounceMs = 250;
```

## Recommended first adopters
- `QtMaterialOutlinedTextField`
- `QtMaterialFilledTextField`
- `QtMaterialDateField`

Do not wire validation separately in each widget.
It should live in the shared input shell.
