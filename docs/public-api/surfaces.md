# Surfaces API

Release-scoped surface widgets include:

- `QtMaterialDialog`
- `QtMaterialNavigationDrawer`
- `QtMaterialBottomSheet`
- `QtMaterialBanner`
- `QtMaterialCard`
- `QtMaterialTopAppBar`
- `QtMaterialBottomAppBar`
- `QtMaterialSnackbar`

Component maturity is tracked in `docs/components/component-registry.json`.

## Dialog

`QtMaterialDialog` provides a modal surface with scrim, focus management and keyboard behavior.

```cpp
auto* dialog = new QtMaterial::QtMaterialDialog(parent);
dialog->setTitleText(QStringLiteral("Delete item"));
dialog->setSupportingText(QStringLiteral("This action cannot be undone."));
dialog->setBodyWidget(content);
dialog->setInitialFocusWidget(firstField);
dialog->setDefaultButton(confirmButton);
dialog->open();
```

The dialog contract includes Escape dismissal when enabled, focus containment, default-button
activation, focus restoration, and accessible title/supporting text.

See [Dialog release readiness](dialog-release-readiness.md).

## Card

`QtMaterialCard` supports Elevated, Filled, and Outlined variants plus optional interactive
behavior. Interactive cards can be activated by mouse, Space, Return, or Enter.

See [Card release readiness](card-release-readiness.md).

## Snackbar

`QtMaterialSnackbar` presents transient feedback on a host widget. A `SnackbarRequest` supplies:

- message text;
- optional action text;
- short, long, or indefinite duration;
- optional dismiss button;
- optional payload.

`showSnackbar()` resolves the host geometry and starts the entry transition. `dismiss()` reports
a typed `SnackbarDismissReason`: timeout, action, manual dismissal, or consecutive replacement.

### Interaction contract

- Escape dismisses the snackbar manually.
- Triggering the action emits `actionTriggered()` and dismisses with `Action`.
- Hover/focus interaction pauses timed auto-hide when
  `pauseAutoHideOnInteraction` is enabled.
- Action and dismiss controls participate in that pause contract through child event filters.
- Leaving/focus-out resumes the remaining timeout.
- Host geometry is resynchronized during transitions and host lifecycle changes.

### Accessibility contract

Whenever request content changes, the snackbar resynchronizes its accessible description.
The description combines the message, optional action, and dismissible state. Action and dismiss
buttons expose their own accessible names/descriptions.

### 0.5.0 maturity evidence

`tst_snackbar`, `tst_snackbarhost`, and `tst_snackbarlifecycle` cover:

- show/dismiss lifecycle;
- action dismissal reason;
- Escape dismissal;
- host behavior;
- accessible request state;
- child-focus auto-hide pause/resume;
- RTL layout render smoke;
- DPR 2.0 render smoke.

Snackbar is therefore release-scoped as **usable** for 0.5.0. Reviewed visual references and a
broader transient-surface state matrix remain before `complete` maturity.

## Other surface contracts

Focused release-readiness documents are also available for:

- [Navigation Drawer](navigationdrawer-release-readiness.md)
- [Bottom Sheet](bottomsheet-release-readiness.md)
- [Banner](banner-release-readiness.md)
- [App bars](appbars-release-readiness.md)
