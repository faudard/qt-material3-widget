# Accessibility release contract

Accessibility is a release blocker for public widgets.

Every public widget must define:

* accessible role behavior;
* accessible name behavior;
* keyboard focus policy;
* keyboard activation/navigation behavior;
* visible focus indication;
* disabled behavior;
* state text when selected, checked, indeterminate, expanded, invalid, busy, or error;
* contrast-safe resolved colors for critical foreground/background pairs.

## Icon-only controls

Icon-only actions must have an accessible name supplied by the widget or consuming application.

```cpp
button->setAccessibleName(QStringLiteral("Open settings"));
```

A release-level accessibility test should fail for an icon-only actionable control with an empty accessible name.

## Dialogs and overlays

Modal surfaces must define focus entry, focus trapping, dismissal behavior, and restoration of focus to the invoker where practical.

## Automation alignment

The `materialState` property should agree with the accessibility state exposed by the widget. For example, a checked checkbox should expose both a checked accessible state and a `materialState` value containing `checked`.
