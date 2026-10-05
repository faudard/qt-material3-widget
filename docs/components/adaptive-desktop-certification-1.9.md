# QtMaterial3 1.9 Adaptive / Desktop certification

This runbook defines the promotion path for:

- `navigation.suite` — Navigation Suite;
- `layout.adaptive-shell` — Adaptive Shell.

Both components remain `usable` and outside release scope until deterministic
visual references and native assistive-technology evidence are complete.

## Automated behavior contract

`tst_adaptive_shell` verifies:

- Compact, Medium, Expanded, Large and ExtraLarge width boundaries;
- Navigation Bar -> Navigation Rail adaptation without losing selection;
- keyboard navigation and disabled-destination skipping;
- per-destination QAccessible state and focused item continuity across mode changes;
- automatic density resolution and application-owned density opt-out;
- supporting-pane appearance only when the main content can keep at least
  320 logical pixels;
- supporting-pane minimum width;
- LTR/RTL rail and supporting-pane placement;
- synchronized Adaptive Shell accessibility summaries.

## Pinned visual evidence

CI registers `tst_adaptive_desktop_visual_goldens`.

The canonical matrix contains 30 cases:

- five width classes;
- LTR and RTL;
- light/standard, dark/standard and light/high-contrast themes.

Representative logical widths are 520, 720, 1000, 1280 and 1640 pixels.
Every case keeps the real shell width so breakpoint behavior is tested rather
than scaled into an artificial preview.

The authoritative renderer is:

| Property | Value |
| --- | --- |
| Qt | 6.4.0 |
| Style | Fusion |
| Platform | xcb |
| Scale | 1 |
| Font DPI | 96 |

The `adaptive-desktop-visual-evidence` artifact contains two independent
render passes plus `adaptive-desktop-repeatability.json`.
`tools/check_adaptive_desktop_visual_repeatability.py` rejects missing cases,
renderer drift, different commits, dimensions, PNG bytes or pixel hashes.

Repeatability proves determinism only. A reviewer must inspect all 30 images
before copying them to `tests/visual/goldens/` and marking visual evidence
`pass`.

## Native accessibility review

Build and run `qtmaterial3_adaptive_desktop_certification` from
`examples/adaptive-desktop-certification`.

Repeat the same keyboard-only workflow with:

| Platform | Reader |
| --- | --- |
| Windows | NVDA |
| Linux | Orca / AT-SPI |
| macOS | VoiceOver |

The canonical ledger is
`docs/components/adaptive-desktop-certification-1.9.json`.

### Navigation Suite checks

- destination traversal;
- selected/disabled state announcements;
- activation;
- focused destination continuity across Bar/Rail mode switches.

### Adaptive Shell checks

- responsive composition is understandable after each resize;
- focus in main content survives resize/tiling changes;
- supporting-pane appearance/disappearance is exposed coherently;
- RTL reading/navigation order remains usable.

The fixture exposes keyboard-focusable resize controls for Compact, Medium,
Expanded, Large and ExtraLarge, an RTL toggle and an automatic-density toggle.

## Validation

Structural validation while evidence is pending:

```bash
python tools/check_adaptive_desktop_1_9.py
```

Final validation:

```bash
python tools/check_adaptive_desktop_1_9.py --require-complete
```

The final command fails until every reviewed golden exists and all three native
screen-reader records are complete.

## Promotion

Check eligibility:

```bash
python tools/promote_adaptive_desktop_1_9.py
```

Apply only after eligibility succeeds:

```bash
python tools/promote_adaptive_desktop_1_9.py --apply
```

Promotion raises rendering/accessibility to 4/4, clears final gaps, marks both
components `complete`, moves them into release scope, registers all 30 stable
goldens, regenerates component status and runs the base release contract.

The existing Enterprise visual checker accepts one representative visual prefix
per component. Navigation Suite maps to the Compact/LTR family and Adaptive
Shell maps to Expanded/LTR, while the dedicated 1.9 gate still requires all 30
reviewed references before promotion.
