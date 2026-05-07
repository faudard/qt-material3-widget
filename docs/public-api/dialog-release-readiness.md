# Dialog release readiness

`QtMaterialDialog` is considered `usable` when it behaves like a real modal surface, not only as a painted container.

## Public contract

The dialog now exposes:

```cpp
dialog->setTitleText("Delete item");
dialog->setSupportingText("This action cannot be undone.");
dialog->setBodyWidget(content);
dialog->setInitialFocusWidget(primaryInput);
dialog->setDefaultButton(confirmButton);
dialog->setDismissOnEscape(true);
dialog->setRestoreFocusOnClose(true);
```

## Keyboard behavior

- `Escape` rejects and closes the dialog when `dismissOnEscape` is enabled.
- `Tab` and `Backtab` keep focus inside the dialog.
- `Return` / `Enter` activate the configured default button.
- Opening the dialog focuses the configured initial focus widget or the first focusable child.
- Closing the dialog restores focus to the previously focused widget by default.

## Accessibility behavior

- `titleText` maps to `accessibleName`.
- `supportingText` maps to `accessibleDescription`.
- `accessibilitySummary()` combines title and supporting text for summaries, logs and future screen-reader announcements.

## Release status

`surface.dialog` can be promoted from `partial` to `usable` after:

```bash
cmake --build build --target tst_dialog
ctest --test-dir build -R tst_dialog --output-on-failure
```
