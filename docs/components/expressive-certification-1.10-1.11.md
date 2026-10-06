# QtMaterial3 1.10 / 1.11 Expressive certification

This runbook defines the production certification path for the Expressive
Catalogue and Expressive Data milestones introduced after the 1.9 Adaptive /
Desktop work.

## Promotion components

The following new registry-owned components remain `usable` until this gate is
complete:

- `button.split` — Split Button;
- `button.group` — Button Group;
- `navigation.floating-toolbar` — Floating Toolbar;
- `progress.loading-indicator` — Loading Indicator;
- `data.segmented-list` — Segmented List.

Two existing components also receive opt-in Expressive modes and are certified
by the same evidence without overwriting their existing maturity state:

- `navigation.menu`;
- `compact.chip`.

## Automated behavior contract

`tst_expressive_catalogue` covers the additive public API and core behavior,
including:

- independent Split Button primary/secondary activation;
- Button Group checked-state ownership;
- RTL-aware horizontal group/toolbar layout;
- Floating Toolbar collapse/orientation behavior;
- Loading Indicator active/idle sizing;
- Segmented List automatic Single/First/Middle/Last recomputation, including
  external item deletion;
- Expressive Menu and Chip opt-in state.

The Linux ASan/UBSan lane remains part of the release gate. In particular,
external List Item destruction must not re-enter a typed `QPointer` after the
derived object has already been torn down.

## Deterministic visual matrix

CI registers `tst_expressive_catalogue_visual_goldens`.

The `expressive_catalogue_matrix` contains all seven Expressive surfaces and
four state rows:

- default;
- variant / selected;
- disabled;
- RTL.

It is rendered in:

- light / standard contrast;
- dark / standard contrast;
- light / high contrast.

The authoritative renderer is Qt 6.4.0 / Fusion / xcb / scale 1 / 96 DPI.

The pinned Qt 6.4 lane renders the matrix twice into independent directories
and runs:

```bash
python tools/check_expressive_catalogue_visual_repeatability.py \
  build/expressive-catalogue-first \
  build/expressive-catalogue-second \
  --output build/expressive-catalogue-repeatability.json
```

The checker rejects missing cases, renderer drift, source-commit drift,
dimension drift, PNG byte drift and pixel-hash drift.

Repeatability proves determinism only. It does not constitute visual approval.

## Native assistive-technology review

The canonical ledger is:

`docs/components/expressive-certification-1.10-1.11.json`

Repeat the same keyboard-only fixture on:

| Platform | Reader |
| --- | --- |
| Windows | NVDA |
| Linux | Orca / AT-SPI |
| macOS | VoiceOver |

The ledger contains 84 native observations:

- seven surfaces;
- four component-specific checks per surface;
- three native readers.

Every observation starts as `pending`.

### Split Button

Verify primary action, secondary action, focus order and disabled state.

### Button Group

Verify traversal, checked-state announcements, activation and RTL traversal.

### Floating Toolbar

Verify traversal, collapsed-state behavior, activation and orientation changes.

### Loading Indicator

Verify busy announcement, idle announcement, reduced-motion/static state and
state-change announcement.

### Segmented List

Verify traversal, selection state, activation and meaningful segment/list
context.

### Expressive Menu

Verify traversal, Expressive state does not regress semantics, activation and
focus behavior.

### Expressive Chip

Verify checked state, activation, removable-chip behavior and focus.

A platform may only be marked `pass` when every required observation for that
reader is `pass`.

## Validation

Structural validation while evidence is pending:

```bash
python tools/check_expressive_catalogue_1_10_1_11.py
```

Final validation:

```bash
python tools/check_expressive_catalogue_1_10_1_11.py --require-complete
```

The final command remains fail-closed until:

- all three reviewed matrix PNGs exist in `tests/visual/goldens/`;
- the visual record is explicitly `pass` with reviewer/date/evidence;
- NVDA, Orca and VoiceOver platform records are all `pass`;
- every component-specific native observation is `pass`;
- each platform records reviewer/date/evidence.

No automated test or deterministic PNG replaces the native reader sessions.
