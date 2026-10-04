# Visual regression testing

QtMaterial3 keeps one visual self-regression harness for resolved theme tokens
and representative component rendering.

## Coverage

The harness exercises:

- light and dark themes;
- standard and high contrast;
- seed-color changes;
- focus/accessibility token changes;
- representative component rendering;
- Selection family state matrices across default, selected, disabled and RTL states;
- Input field state matrices across empty, value, disabled and RTL states;
- composite Input matrices for Search View, Date Picker and Date Range Picker;
- Navigation matrices for Tabs, Navigation Rail, Menu, Breadcrumb and Command Palette;
- focused fixtures for all six Navigation/Desktop components and a SplitView state matrix;
- Slider/Range Slider matrices covering horizontal, vertical, disabled and RTL states;
- Desktop-data matrices for Table, Tree View, Pagination and Split View;
- Theme Studio screenshot output.

Token-board snapshots use deterministic painting and are the safest regression
baseline. Component-grid snapshots use real Qt widgets and therefore remain
opt-in for strict pixel comparison across platforms.

## Run

```bash
ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Strict comparison:

```bash
QTMATERIAL3_VISUAL_STRICT=1 \
ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Intentional golden update:

```bash
QTMATERIAL3_UPDATE_VISUAL_GOLDENS=1 \
ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Optional tolerance:

```bash
QTMATERIAL3_VISUAL_STRICT=1 QTMATERIAL3_VISUAL_MAX_DIFF=25 \
ctest --test-dir build --output-on-failure -R tst_theme_visual_regression
```

Artifacts are written to `visual-artifacts` next to the test executable unless
`QTMATERIAL3_VISUAL_ARTIFACTS_DIR` overrides the directory.

`QTMATERIAL3_VISUAL_GOLDENS_DIR` redirects baseline lookup and candidate output. CI uses
separate output directories so candidate generation preserves reviewed source PNGs.

Golden changes should be reviewed when theme generation, component specs,
shape/elevation/typography defaults, or accessibility/focus behavior changes.

## Candidate family goldens

Selection, Input, Navigation and Desktop Productivity state matrices use a two-stage baseline workflow.

A matrix without a checked-in PNG is a **candidate golden**. Smoke rendering still runs on
every test job, while the Ubuntu/Fusion CI job regenerates the candidate PNGs and publishes
them in the `family-visual-candidate-goldens` artifact. Missing candidate baselines do not
break strict release validation.

After visual review, commit the approved PNG under `tests/visual/goldens/`. From that point,
`QTMATERIAL3_VISUAL_STRICT=1` automatically compares it pixel-for-pixel through the same
test case. Once a family baseline is considered release-critical, also add its path to
`base.stable_release.visual_goldens` in `tools/release_rules.json`.

The candidate set currently contains three controlled theme variants for each matrix:
light/standard contrast, dark/standard contrast, and light/high contrast. This intentionally
avoids a full Cartesian explosion while covering the highest-value theme/state combinations.

## Navigation/Desktop repeatability

The `tst_navigation_visual_goldens` CTest entry selects 27 cases: primary and desktop
Navigation matrices, the SplitView matrix, and focused fixtures for all six components,
each in the three controlled theme variants. The pinned Qt 6.4.0 / Fusion / xcb lane
compares committed references and renders two independent passes at scale 1 and font DPI 96.
It publishes `navigation-desktop-visual-evidence` with PNGs, manifests, comparison artifacts
and the report from `tools/check_navigation_visual_repeatability.py`.

Both passes must have identical image bytes, pixel hashes, dimensions and source commits,
with the expected runtime renderer profile. This catches nondeterministic captures; the
report's `visualReview` remains `pending` until a human reviews the candidates. Promotion
still requires approved source PNGs and release rules. The
[certification guide](navigation-desktop-certification.md) records behavioral coverage and
the separate platform accessibility review.

Linux CI starts Openbox through `scripts/ci/run-with-openbox.py`. It waits for matching
`_NET_SUPPORTING_WM_CHECK` properties on the root and supporting window before starting
CTest, with a bounded timeout and startup diagnostics. This prevents the first real
focus fixture from racing window-manager initialization. Qt exposure and focus assertions
remain required. The full-family candidate job uses the same pinned xcb, scale and font
settings as the Navigation captures.


## 1.x family matrix policy

The 1.x visual gate grows by **family**, not by taking the Cartesian product of every theme,
state, direction, and scale factor for every widget.

Approved family buckets are:

- Buttons
- Selection
- Inputs
- Navigation
- Surfaces
- Data
- Progress
- Compact

Each important component must eventually have deterministic evidence for its relevant subset
of Light, Dark, High Contrast, enabled/disabled, hover, focus, pressed, selected/checked,
error, LTR and RTL behavior. State coverage is chosen per family: for example, checked is
mandatory for Selection but meaningless for a plain surface, while error is mandatory for
validated Inputs but not for Breadcrumb.

DPI is split deliberately between two layers. The pixel-golden layer uses a small stable
reference set (normally 1x plus a reviewed 2x/Retina-class case where rasterization matters).
The desktop integration contract separately exercises 100%, 125%, 150%, 175%, and 200%.
This catches fractional-scale assumptions without multiplying every family golden by five.

Before promoting a candidate family image to a release gate, review it for:

- one-pixel borders and dividers;
- corner radii and focus-ring geometry;
- icon rasterization and alignment;
- text baselines, offsets, and clipping;
- menus, popups, and overlay geometry;
- elevation/shadow cache behavior;
- LTR/RTL mirroring where applicable.

A component registry maturity score must only be raised after the corresponding deterministic
evidence is checked in. Candidate artifacts alone do not satisfy the visual-completion gate.
