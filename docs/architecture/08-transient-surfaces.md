# 08 — Transient Surfaces

## Overview

Transient surfaces are temporary UI layers attached to a host widget. They are used for short-lived interactions such as bottom sheets and snackbars.

In this project, transient surfaces are built from a small set of cooperating pieces:

- `QtMaterialOverlaySurface`
- `QtMaterialTransitionController`
- optional `QtMaterialScrimWidget`
- a component widget such as `QtMaterialBottomSheet` or `QtMaterialSnackbar`
- an optional host/orchestrator object such as `QtMaterialSnackbarHost`

The goal is to keep rendering, interaction, orchestration, and theming responsibilities clearly separated.

---

## Core building blocks

### `QtMaterialOverlaySurface`

`QtMaterialOverlaySurface` is the base class for surfaces that are visually attached to another widget.

Responsibilities:

- track a `hostWidget`
- resync geometry when the host moves, resizes, or is shown
- provide a protected `syncGeometryToHost()` hook for subclasses
- act as the visual root of the transient component

`QtMaterialOverlaySurface` is responsible for **placement on the host**, not for queueing, business logic, or rendering policy.

---

### `QtMaterialTransitionController`

`QtMaterialTransitionController` owns the local transition progress of a transient surface.

Responsibilities:

- expose `progress`
- animate forward and backward
- apply `MotionStyle` and `MotionToken`
- emit `progressChanged()` and `finished()`

It is the source of truth for local animation progress.

A transient widget should read its visual state from `progress` rather than duplicating animation state elsewhere.

---

### `QtMaterialScrimWidget`

`QtMaterialScrimWidget` is an optional dimming layer placed behind a transient surface.

Responsibilities:

- render a scrim color
- optionally react to theme fallback when no explicit color is set
- optionally receive clicks when used as a modal blocker

The scrim is a rendering and interaction primitive. It is not responsible for queueing or component state.

---

## Widget responsibilities

A transient **widget** is the component that renders and manages a single transient instance.

Examples:

- `QtMaterialBottomSheet`
- `QtMaterialSnackbar`

A widget is responsible for:

- local state machine
- rendering
- geometry derived from the host
- local interaction handling
- local animation usage
- spec resolution
- focus behavior
- dismiss/show behavior
- local timers when intrinsic to the component

A widget should be usable on its own for a simple case.

Examples:

- `QtMaterialBottomSheet::open()` / `close()`
- `QtMaterialSnackbar::showSnackbar()` / `dismiss()`

### Rules

A transient widget should:

- own its visual state
- own its transition progress usage
- resolve its spec from `SpecFactory`
- react to theme changes
- expose meaningful signals such as:
  - `shown`
  - `dismissed`
  - `progressChanged`
  - `stateChanged`
  - `actionTriggered`

A transient widget should **not**:

- manage a global queue
- decide cross-instance priority
- manage orchestration policies for multiple messages/surfaces
- duplicate logic that belongs to a host

---

## Host responsibilities

A transient **host** is an orchestration object that coordinates one or more transient widget requests.

Example:

- `QtMaterialSnackbarHost`

A host is responsible for:

- queueing
- replacement policy
- sequential display
- ownership or reuse of a widget instance
- convenience APIs such as `showMessage(...)`

A host should **not**:

- render
- compute component layout
- implement the component’s local state machine
- decide local focus behavior
- duplicate dismiss logic already owned by the widget

### Mental model

- the **widget** knows **how** to appear, animate, and disappear
- the **host** knows **when** and **which one** to show

---

## Recommended split by component

### Bottom sheet

`QtMaterialBottomSheet` is a transient widget.

It owns:

- state (`Closed`, `Opening`, `Open`, `Closing`)
- geometry cache
- clipping/mask behavior
- scrim coordination
- keyboard dismissal
- focus behavior
- transition-driven rendering

It does not currently require a separate host because it does not need queueing semantics.

### Snackbar

`QtMaterialSnackbar` is a transient widget.

It owns:

- rendering of a single snackbar
- action button behavior
- dismiss button behavior
- timeout behavior for one instance
- transition-driven show/dismiss animation

`QtMaterialSnackbarHost` is the host.

It owns:

- queueing
- replacing the current snackbar
- deciding which request shows next
- single-visible-instance policy

---

## Shared patterns between transient widgets

The following patterns are good candidates for reuse through private helpers:

- shape radius resolution from `ShapeScale`
- painter path helpers for rounded surfaces
- motion token application to `QtMaterialTransitionController`
- common geometry utilities for host-relative placement

These helpers should stay private until the pattern is proven stable across multiple components.

### Important

Avoid introducing a large generic transient-surface base class too early.

At this stage, `BottomSheet` and `Snackbar` share useful primitives, but they still differ in:

- scrim usage
- focus policy
- clipping behavior
- click-outside policy
- timing model
- queueing requirements

Prefer **small private helpers** over a large inheritance hierarchy.

---

## Theming model

Transient widgets should follow the standard project theming model:

1. resolve a component spec from `SpecFactory`
2. read shape, color, elevation, and motion from the resolved spec
3. use theme fallbacks only when needed
4. update on `themeChangedEvent(...)`

Examples:

- `BottomSheetSpec` provides container color, scrim color, shape role, and motion token
- `SnackbarSpec` provides container color, text/action colors, shape role, and enter/exit motion tokens

---

## Motion model

Transient widgets should not hardcode animation policy unless used as a fallback.

Preferred flow:

1. component spec selects a `MotionToken`
2. widget applies the token to `QtMaterialTransitionController`
3. widget renders from controller `progress`

This keeps motion policy theme-driven and consistent across components.

---

## Shape model

Transient widgets should prefer:

- `ShapeRole` in the component spec
- actual pixel radius resolved from `Theme::shapes()`

A local hardcoded radius is acceptable only as a defensive fallback.

This keeps component shape aligned with the global shape scale.

---

## Interaction model

Recommended defaults:

### Modal surface
- scrim visible
- scrim receives pointer input
- clicking the scrim dismisses the surface if the component supports outside-click dismissal
- the surface root does not need a click-through mask

### Non-modal surface
- no blocking scrim
- the surface root may use a mask so only the visible panel is interactive
- clicks outside the visible panel should pass to the host unless the component explicitly consumes them

The exact policy belongs to the widget, not the host.

---

## Testing strategy

Transient surfaces should be covered at three levels:

### 1. Unit tests
Examples:

- transition controller behavior
- motion token lookup
- scrim fallback behavior

### 2. Widget interaction tests
Examples:

- open/close
- keyboard dismissal
- outside click dismissal
- resize tracking
- clipping/mask behavior
- progress/state transitions

### 3. Host orchestration tests
Examples:

- FIFO queue
- replacement behavior
- dismiss current
- single-visible-instance policy

---

## Design guidelines for new transient surfaces

When introducing a new transient component:

1. start with a dedicated spec
2. implement the widget first
3. add a host only if orchestration is needed
4. reuse private transient-surface helpers where appropriate
5. add playground coverage
6. add tests before broadening behavior

### Good candidates for this model
- snackbar
- side sheet
- temporary banners
- toast-like ephemeral surfaces

---

## Summary

Use this rule of thumb:

- **Widget** = one transient surface instance, with rendering and local behavior
- **Host** = orchestration for multiple requests or policies
- **OverlaySurface** = placement on a host
- **TransitionController** = local animation progress
- **ScrimWidget** = optional modal dimming layer

This separation keeps transient components easier to evolve, test, and theme consistently.