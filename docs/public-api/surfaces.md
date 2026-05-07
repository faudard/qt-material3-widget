# Surfaces API

Surface family:
- `QtMaterialDialog`
- `QtMaterialNavigationDrawer`
- `QtMaterialBottomSheet`
- `QtMaterialBanner`
- `QtMaterialCard`
- app bars

## Dialog

`QtMaterialDialog` provides a modal surface with a scrim, focus management and keyboard behavior.

```cpp
auto* dialog = new QtMaterial::QtMaterialDialog(parent);
dialog->setTitleText(QStringLiteral("Delete item"));
dialog->setSupportingText(QStringLiteral("This action cannot be undone."));
dialog->setBodyWidget(content);
dialog->setInitialFocusWidget(firstField);
dialog->setDefaultButton(confirmButton);
dialog->open();
```

Dialog contract:

- `Escape` rejects the dialog when `dismissOnEscape()` is enabled.
- `Tab` / `Backtab` keep focus inside the dialog.
- `Return` / `Enter` activate the configured default button.
- closing restores focus to the previously focused widget by default.
- `titleText` and `supportingText` drive accessible name and description.

See also `docs/public-api/dialog-release-readiness.md`.
