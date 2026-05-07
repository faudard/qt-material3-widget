# TextField error display policy

This patch separates raw error state from visible error state.

`hasErrorState()` still reports whether the field currently has a manual or automatic validation error.
`isEffectiveErrorVisible()` reports whether that error should be rendered and exposed through the TextField accessibility description.

## API

```cpp
enum class ErrorDisplayMode {
    Always,
    WhenTouched,
    WhenModified,
    AfterCommit,
};

ErrorDisplayMode errorDisplayMode() const noexcept;
void setErrorDisplayMode(ErrorDisplayMode mode);
void showValidationError();
void resetValidationErrorVisibility();
bool isEffectiveErrorVisible() const noexcept;

Q_SIGNAL void effectiveErrorVisibleChanged(bool visible);
```

## Behaviour

- `Always`: automatic validation errors are displayed as soon as they are detected. This is the default and preserves existing behaviour.
- `WhenTouched`: automatic validation errors are hidden until the user has interacted with the field.
- `WhenModified`: automatic validation errors are hidden until the field is dirty.
- `AfterCommit`: automatic validation errors are shown after commit/submit semantics.

Manual errors set with `setHasErrorState(true)` are always visible. Manual errors are considered explicit application state, for example a server-side validation error after form submit.

`showValidationError()` is intended for form submit flows. It forces the current validation error to become visible even if the selected display policy would otherwise hide it.

`resetValidationErrorVisibility()` clears that forced visibility and returns to the selected policy.
