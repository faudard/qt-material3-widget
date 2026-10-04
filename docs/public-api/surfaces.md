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
- `QtMaterialSideSheet`
- `QtMaterialTooltip`
- `QtMaterialBadge`

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


## Side Sheet

`QtMaterialSideSheet` provides a left- or right-anchored supporting surface with a stable
`contentWidget()` composition point. It supports modal and non-modal presentation, optional
scrim-click dismissal, Escape dismissal, title/accessibility synchronization and host-relative
geometry. The modal scrim stays below the sheet and follows host resize events through the
overlay-surface contract. Edge placement is logical API state and remains deterministic in RTL;
applications choose the semantic edge appropriate to their layout.

Modal sheets now expose `initialFocusWidget` and `restoreFocusOnClose`. While open, Tab and
Shift+Tab are contained inside the sheet's close/content controls; closing the sheet restores
the pre-open focus target when it remains valid. Non-modal sheets retain native host focus
traversal. `tst_missing_material3` certifies initial focus, modal containment, Escape dismissal
and restoration.

## Tooltip

`QtMaterialTooltip` attaches to any target `QWidget` without taking ownership of that target.
Pointer hover and keyboard focus use the same delayed presentation path. Placement can be
automatic or explicitly Above, Below, Left or Right, and the popup is clamped to the current
screen's available geometry. Tooltips never take keyboard focus; accessible description text
mirrors the visible content, and target FocusIn/FocusOut behavior provides keyboard parity.
Placement geometry is screen-relative and remains stable under RTL because no logical text order
is inverted.

## Badge

`QtMaterialBadge` renders either a small dot or a numeric count. Numeric badges support a
configurable maximum (for example `99+`) and use the theme Error/OnError roles. Badges are
non-interactive and therefore have no keyboard activation contract; their accessible description
announces either new content or the displayed notification count. The compact geometry is
direction-neutral and can be positioned by the owning layout on the appropriate leading/trailing
edge under RTL.

The Side Sheet, Tooltip and Badge enter the 1.7 catalogue as **usable** components. Focused tests
cover modal focus containment/restoration, Escape behavior, target visibility, count/dot state
and DPR 2.0 custom painting. The dedicated `missing_material3_matrix` provides pinned
light/dark/high-contrast candidates with a two-pass repeatability report. Reviewed goldens and
the component-specific NVDA/Orca/VoiceOver checks recorded in
`docs/components/material3-catalogue-certification-1.7.json` remain before promotion to the
release-scoped `complete` gate.
