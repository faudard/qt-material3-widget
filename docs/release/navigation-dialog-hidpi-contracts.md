# Navigation, dialog, RTL, and high-DPI contracts

This document defines the release-level smoke contracts for navigation widgets, dialogs, RTL layout, and high-DPI rendering.

## Tabs keyboard navigation

Tabs must support keyboard navigation through the tab bar:

- `Right` moves to the next tab in left-to-right layout.
- `Left` moves to the previous tab in left-to-right layout.
- `Home` moves to the first tab.
- `End` moves to the last tab.

The active tab must update the widget `currentIndex`. When a navigation model is bound, the selected item must remain synchronized with the current tab.

## Dialog keyboard behavior

Dialogs must close when `Escape` is pressed, unless a future API explicitly disables dismiss behavior.

When a dialog opens and contains focusable body content, initial focus should move to the first focusable child. This avoids opening a modal surface with no useful keyboard focus target.

## High-DPI smoke rendering

Core interactive widgets must render successfully into a device-pixel-ratio aware paint device.

The desktop release matrix explicitly exercises:

- 100% (`DPR 1.00`);
- 125% (`DPR 1.25`);
- 150% (`DPR 1.50`);
- 175% (`DPR 1.75`);
- 200% / Retina-class (`DPR 2.00`).

The smoke test does not compare pixels. It verifies that common widgets render into a correctly sized, non-empty paint device without crashes, invalid paint states, integer-DPR assumptions, or a hard-coded DPR of 1. Visual family baselines remain responsible for detecting clipping, one-pixel borders, focus-ring geometry, icon/text offsets, popup geometry, radii, and shadow-cache regressions.

## RTL smoke behavior

Common interactive widgets must tolerate `Qt::RightToLeft` layout direction without crashing or losing visibility. Detailed RTL semantics, such as icon mirroring and arrow-key reversal, can be validated in later visual and interaction tests.

## Test target

The corresponding test target is:

```text
tst_navigation_dialog_hidpi_contracts
```

Run it with:

```bash
ctest --test-dir build -R navigation_dialog_hidpi --output-on-failure
```
