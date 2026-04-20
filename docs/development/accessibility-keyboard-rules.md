# Accessibility and Keyboard Rules

This document defines the minimum accessibility and keyboard-interaction rules for `qt-material3-widgets`.

## Core rules

- Keyboard focus must be visible and distinct from hover.
- Buttons and selection controls must be reachable with Tab and activatable with Space/Enter where appropriate.
- Dialogs must support Escape to close when that matches the widget contract.
- Text fields must delegate text editing behavior to `QLineEdit` or another Qt editor primitive.
- Accessible names should come from visible labels or button text when the user has not set one explicitly.
- Accessible descriptions should prefer error text over supporting text when an error state is active.

## Family rules

### Buttons
- Derive semantics from `QAbstractButton`.
- Keep `StrongFocus` unless a widget intentionally opts out.
- Do not hide the focus ring on keyboard focus.

### Selection controls
- Must remain checkable and keyboard-toggleable.
- Checked state must be reflected consistently in interaction state and accessible semantics.

### Dialogs
- Should take focus on open.
- Should move focus to a reasonable child when one exists.
- Should support Escape close unless the widget explicitly documents a different behavior.

### Text fields
- Must expose the label as the accessible name when possible.
- Should expose supporting text or error text as the accessible description.
- Should use the hosted line edit as the focus proxy.
