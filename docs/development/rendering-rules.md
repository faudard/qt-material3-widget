# Rendering Rules

This document defines the rendering discipline for `qt-material3-widgets`.

The goal is to keep the library visually consistent, performant, and maintainable while staying aligned with the architecture:

- Qt owns behavior
- theme owns design tokens
- specs own resolved metrics and colors
- effects own reusable rendering infrastructure
- widgets compose geometry and paint the Material shell

These are hard rules unless there is a strong documented reason to break them.

## 1. Core Principle

A widget may custom-paint only what Qt does not already provide correctly for Material 3.

Good custom-paint responsibilities:
- container shape
- container fill
- outline
- state layer
- ripple
- focus ring
- indicator visuals
- surface/elevation treatment

Bad custom-paint responsibilities:
- text editing engine
- keyboard activation semantics from scratch
- accessibility behavior from scratch
- text cursor logic
- input method handling
- selection model logic already owned by Qt

> Use Qt for behavior, paint for appearance.

## 2. Use Qt Primitives for Behavior

Preferred foundations:
- `QAbstractButton` for buttons and selection controls
- `QLineEdit` for single-line text input
- `QWidget` and composition for surfaces and containers
- Qt focus, tab order, and event systems for interaction semantics

> If Qt already owns the semantics correctly, keep Qt in charge.

## 3. Never Reimplement Text Editing

Text-field-like components must not implement a custom text engine.

For text fields:
- use `QLineEdit` internally for single-line input
- later use `QTextEdit` or `QPlainTextEdit` for multiline if needed

Custom painting is allowed for:
- outline
- filled container
- floating label
- supporting text
- error text
- prefix/suffix visuals
- focus/error state visuals

Do not custom-build:
- caret behavior
- text selection behavior
- clipboard semantics
- IME/input method behavior

## 4. Paint Only from Resolved Specs

Widgets must not paint directly from raw theme roles.

Bad:

```cpp
auto c = theme().colorScheme().color(ColorRole::Primary);
```

Good:

```cpp
const auto &spec = resolvedSpec();
painter.setBrush(spec.containerColor);
```

Rules:
- a widget resolves its spec
- a widget caches its spec
- paint consumes the spec only

## 5. No Magic Visual Constants in Paint Code

Widget paint code must not contain ad hoc visual constants.

Metrics must come from:
- component specs
- theme tokens
- shared private helpers

> Metrics belong in specs, not in paint functions.

## 6. Keep Paint Functions Dumb

A `paintEvent()` should mostly do the following:
1. ensure spec is resolved
2. ensure cached geometry is valid
3. configure painter
4. paint container
5. paint state layer
6. paint ripple
7. paint focus ring
8. paint content

A `paintEvent()` must not:
- decide theme policy
- generate design tokens
- perform heavy state derivation
- allocate unnecessary temporary objects repeatedly
- own animation logic

> Paint renders prepared state. It does not invent it.

## 7. Cache Geometry That Depends on Size

If geometry depends only on size, shape, and spec, cache it.

Examples:
- rounded container path
- outline path
- indicator path
- focus ring path
- ripple clip path

Recompute only when:
- widget size changes
- shape-related spec values change
- layout-affecting content changes

## 8. Cache Text Layout Inputs

Text layout work must be treated as non-trivial.

Cache or recompute only when these inputs change:
- widget text
- font
- icon
- available width
- typography-related spec values

## 9. Effects Must Be Reusable Infrastructure

Effects must live in the effects layer, not inside final widget implementations.

Effects layer owns:
- ripple animation
- focus indicator painting
- state layer painting
- shadow rendering
- shadow caching
- transition control

Widgets may:
- own an effect controller instance
- configure effect geometry
- trigger effects
- pass colors and paths

> Effects are infrastructure, not widget-local hacks.

## 10. Repaint the Smallest Reasonable Region

Widgets should not repaint more than needed. Early versions may tolerate coarse repaint for correctness, but the long-term target is tighter invalidation.

## 11. Shadow Rendering Must Be Cached

Shadows are expensive and must not be rebuilt casually.

Rules:
- no heavy shadow generation every frame in steady-state rendering
- cache by size, radius, blur, offset, and color
- invalidate only on relevant changes
- prefer shared cache infrastructure

## 12. Separate Visual Rect from Touch Target

Material 3 often uses a visual container smaller than the interactive target.

- `sizeHint()` must respect the touch target
- paint must respect the visual container
- the two must not be confused

## 13. Disabled State Must Be Spec-Driven

Disabled rendering must not be improvised per widget.

Allowed:
- disabled colors computed in specs
- a centralized disabled alpha policy applied in one place

Not allowed:
- every widget choosing its own arbitrary opacity values

## 14. Focus Rendering Must Be Keyboard-Correct

Focus indication must reflect actual focus semantics, not hover.

- keyboard focus must always be visually clear
- hover is not a replacement for focus
- focus ring must respect component shape
- focus rendering should be consistent across widget families

## 15. Animation State Must Be Outside Paint

Paint reads animation progress. Paint does not run animation logic.

Use:
- ripple controllers
- transition controllers
- explicit animated state objects

## 16. Compose Before Rebuilding Complex Behavior

When a control can be built as an existing Qt behavior primitive plus a Material shell around it, prefer that over rewriting complex behavior.

## 17. Public Headers Must Not Leak Rendering Internals

Public widget headers must not expose:
- path caches
- shadow cache keys
- animation internals
- effect engine private state
- private layout helpers

## 18. Share Paint Helpers Across Families

If multiple widgets paint the same concept, they should not each re-solve it independently.

Examples:
- button family should share content layout helpers
- selection controls should share indicator geometry helpers
- surfaces should share container/shadow paint helpers
- inputs should share label/supporting text layout helpers

> Shared visual concepts should be rendered through shared helpers.

## 19. Render Tests Are Part of the Rendering Contract

Every real widget must eventually have render regression coverage.

At minimum capture:
- default state
- hover state
- focus state
- pressed state
- disabled state
- dark mode state
- checked/error state where relevant

## 20. Optimize After Correctness, But Optimize on Purpose

The correct order is:
1. correct behavior
2. clean architecture
3. stable rendering
4. measurement
5. optimization

## Widget-Specific Rules

### Buttons
- derive behavior from `QAbstractButton`
- resolve and cache `ButtonSpec`
- cache container path on resize
- cache content layout until text/font/icon/size changes
- clip ripple to the container shape
- render focus ring consistently with the container geometry

### Selection Controls
- use the shared selection-control family base
- paint indicator visuals manually
- keep state-layer region explicit
- cache indicator geometry where practical
- separate label layout from indicator layout cleanly

### Surfaces
- use surface semantics consistently
- render container, shape, and elevation from spec
- use cached shadow rendering
- use scrim composition where relevant
- keep transition logic outside paint

### Text Fields
- host `QLineEdit`
- paint only the Material shell
- keep focus/error/label/supporting text layout separate from editor behavior
- cache label/supporting/error layout where practical
- derive colors, sizes, and outlines from `TextFieldSpec`

## Forbidden Patterns

The following patterns are banned unless explicitly justified and reviewed:
- raw theme lookups in every paint branch
- repeated hardcoded pixel metrics across widgets
- widget-local ripple implementations
- widget-local shadow algorithms
- rebuilding heavy paths every frame without need
- recalculating text elision every paint unnecessarily
- custom text entry logic replacing Qt editing widgets
- paint functions performing business logic or token mapping

## Project Mantra

> Qt owns behavior.  
> Specs own metrics.  
> Effects own reusable rendering infrastructure.  
> Widgets compose geometry and paint the Material shell.
