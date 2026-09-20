# Interaction and effects

0.7.x defines the shared behavioral contract for interactive Qt Material 3 widgets.
Component specs still decide appearance, but Core and Effects own interaction semantics,
focus visibility, motion, ripple behavior, and cached rendering policy.

## Interaction state

`QtMaterialInteractionState` is the canonical state value shared by Core and Specs.
It covers enabled/disabled, hovered, focused, pressed, checked, selected,
indeterminate, dragged, error, read-only, busy, expanded, and invalid states.

Transient states are hover, focus, press, and drag. Disabling a control clears those
states but does not erase persistent selection or checked state. Read-only controls
cannot enter pressed or dragged state.

State-layer priority is deterministic:

```text
dragged -> pressed -> focused -> hovered -> none
```

The theme's `InteractionStateTokens` may suppress hover, press, or drag feedback.

## Ripple

`QtMaterialRippleController` supports bounded and unbounded painting and pointer-
or center-origin ripples. Pointer activation uses the pointer position; keyboard
activation uses the component center.

Disabled widgets never create a ripple. Under reduced motion, the animated ripple is
suppressed and the static state layer remains the interaction feedback.

## Focus

`QtMaterialFocusIndicator::shouldShow()` combines the actual focus reason with
`InteractionStateTokens`.

With strong focus indicators enabled, focused controls keep a visible ring. Otherwise
keyboard focus reasons (Tab, Backtab, Shortcut) are visible while mouse/programmatic
focus can remain visually quiet.

Focus-ring geometry and color remain tokenized through `FocusRingTokens`.

## Reduced motion

`QtMaterialTransitionController` accepts a reduced-motion policy. When active,
transitions complete synchronously at their requested final state instead of running
an animation timer.

Re-targeting an active transition starts from the current progress and targets the
latest requested state. Cancellation never invents a final progress value.

Buttons, selection controls, Dialog, BottomSheet, and Snackbar propagate the current
theme's `AccessibilityTokens::reducedMotion` into their effect controllers.

## Transient focus

Modal Dialog and BottomSheet surfaces trap Tab/Shift+Tab while open. When dismissed,
they restore focus to the widget that owned focus before opening when that target
still exists and restoration is enabled.

Non-modal surfaces do not impose the modal focus trap.

## Device-pixel-ratio shadow caching

Shadow cache identity includes the device pixel ratio. A 1x and 2x shadow with the
same logical geometry therefore cannot alias to the same cached pixmap.

The effects benchmark suite exercises both 1x and 2x cache keys and the ripple
animated/reduced-motion paths.

## Testing

The 0.7 release gate combines:

- the common interaction-state tests;
- ripple/state-layer/focus/motion effect tests;
- Dialog and BottomSheet focus contracts;
- keyboard activation/navigation tests already owned by widget families;
- DPR-aware shadow and ripple benchmarks;
- the normal Qt 5.14.2, Qt 6, Linux, macOS, Windows and sanitizer matrix.
