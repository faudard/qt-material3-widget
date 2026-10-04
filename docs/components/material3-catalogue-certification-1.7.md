# QtMaterial3 1.7 catalogue certification

This document defines the promotion path for the four widgets introduced by the
1.7 Missing Material 3 milestone:

- Navigation Bar;
- Side Sheet;
- Tooltip;
- Badge.

All four remain `usable` and outside release scope until both visual and native
assistive-technology evidence have been reviewed.

## Automated readiness

The focused `tst_missing_material3` suite covers:

- Navigation Bar destination state, keyboard/RTL behavior, DPR 2.0 painting and
  per-destination QAccessible List/ListItem interfaces with press/focus actions;
- Side Sheet lifecycle, Escape dismissal, explicit initial focus, modal
  Tab/Shift+Tab containment and focus restoration;
- Tooltip target attachment, focus/pointer presentation path and accessible
  description synchronization;
- Badge numeric/overflow/dot state and DPR 2.0 painting.

The visual harness registers `tst_missing_material3_visual_goldens`, backed by
`missing_material3_matrix` in light/standard, dark/standard and
light/high-contrast variants.

## Pinned visual review

The authoritative rendering profile is:

| Property | Value |
| --- | --- |
| Qt | 6.4.0 |
| Style | Fusion |
| Platform | xcb |
| Scale | 1 |
| Font DPI | 96 |

CI renders the three matrix images in two independent processes and publishes
the `missing-material3-visual-evidence` artifact.
`tools/check_missing_material3_visual_repeatability.py` rejects:

- missing cases;
- renderer/profile drift;
- different source commits;
- different image dimensions;
- different pixels or pixel hashes.

Repeatability proves determinism only. A reviewer must inspect the first-pass
PNGs for layout, clipping, theme contrast, RTL, Side Sheet edges/scrim, Tooltip
surface/text and Badge dot/overflow presentation. Approved files are copied to
`tests/visual/goldens/`, then the ledger visual record is changed to `pass`
with reviewer, date and durable evidence reference.

## Native accessibility review

Build and run `qtmaterial3_catalogue_certification` from
`examples/material3-catalogue-certification`.

Repeat the same fixture with:

| Platform | Reader |
| --- | --- |
| Windows | NVDA |
| Linux | Orca / AT-SPI |
| macOS | VoiceOver |

The canonical ledger is
`docs/components/material3-catalogue-certification-1.7.json`.

Required checks are component-specific.

### Navigation Bar

- destination traversal;
- selected/disabled state announcements;
- activation;
- focus.

### Side Sheet

- modal focus containment;
- title announcement;
- dismissal;
- focus restoration to the invoker.

### Tooltip

- focus-triggered presentation;
- announcement of supporting text;
- dismissal on focus loss;
- preservation of target focus.

### Badge

- numeric count announcement;
- overflow count announcement;
- dot/new-content announcement;
- dynamic update announcement.

A platform record can only be `pass` when every required component check is
`pass`. Bridge-specific problems stay as `fail` and remain maturity gaps.

## Validation commands

Structural validation while evidence is still being collected:

```bash
python tools/check_material3_catalogue_1_7.py
```

Final validation:

```bash
python tools/check_material3_catalogue_1_7.py --require-complete
```

The second command fails until reviewed PNGs are committed and every native AT
record is complete.

## Promotion

Check eligibility:

```bash
python tools/promote_material3_catalogue_1_7.py
```

Apply the promotion only after the command above succeeds:

```bash
python tools/promote_material3_catalogue_1_7.py --apply
```

The promotion command:

1. raises rendering and accessibility from 3/4 to 4/4;
2. clears the final gaps;
3. promotes all four components to `complete`;
4. moves them into release scope;
5. registers `missing_material3_matrix` as their stable visual family;
6. adds the three reviewed PNGs to stable release rules;
7. regenerates component status documents;
8. runs the base release contract.

If the 1.5 Enterprise gate has already been completed when 1.7 is promoted, the
new release-scoped components must satisfy the same Enterprise component
contract; the promotion helper prepares the required visual mapping for that
case.
