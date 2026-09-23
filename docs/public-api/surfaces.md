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

The dialog keeps keyboard focus inside the modal surface, supports Escape dismissal when
enabled, activates an explicit default button on Return/Enter, and restores previous focus on
close when configured.

## Card

`QtMaterialCard` supports Elevated, Filled, and Outlined variants plus optional interactive
behavior. Interactive cards can be activated by mouse, Space, Return, or Enter.

Cards are non-interactive by default. When interaction is enabled they expose mouse and
Space/Return/Enter activation plus hover, press and focus state rendering.

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

### Maturity evidence

`tst_snackbar`, `tst_snackbarhost`, and `tst_snackbarlifecycle` cover:

- show/dismiss lifecycle;
- action dismissal reason;
- Escape dismissal;
- host behavior;
- accessible request state;
- child-focus auto-hide pause/resume;
- RTL layout render smoke;
- DPR 2.0 render smoke.

`QtMaterialSnackbar` is tracked as **usable**. Reviewed visual references and a broader
transient-surface state matrix remain before `complete` maturity.

## Navigation Drawer

`QtMaterialNavigationDrawer` supports left/right placement, open/close lifecycle,
destination insertion/removal, disabled-destination skipping, directional keyboard navigation,
Space/Return/Enter activation, optional Escape dismissal, focus restoration and accessible
destination summaries.

## Bottom Sheet

`QtMaterialBottomSheet` supports modal and non-modal presentation, expanded/collapsed states,
scrim and Escape dismissal policies, drag collapse/dismiss behavior, keyboard expand/collapse,
initial-focus handling, focus restoration and a stable `contentWidget()` container.

## Banner

`QtMaterialBanner` exposes title/body text, primary and secondary actions, optional dismissal,
Escape behavior and deterministic accessibility summaries. Action controls remain keyboard
focusable.

## App bars

`QtMaterialTopAppBar` and `QtMaterialBottomAppBar` provide title text, navigation/action
controls, keyboard activation, deterministic size hints and accessible names. Bottom app bars
also expose accessible text for an attached FAB.
