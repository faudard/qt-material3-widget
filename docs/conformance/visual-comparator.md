# Visual comparator

Phases 016.A-C1 define the machine-readable capture contract, implement the
private actual-only runner and provide a Qt-independent read-only comparator for
the fully certified Classic Buttons fixture. Both sources are complete, but no
declared target execution or reviewed reference set exists, so rendering is not
yet visually conformant.

The contract is generated at:

```text
references/material3/classic/compose-material3-1.4.0/visual/buttons.json
```

It is separate from the opt-in QtMaterial3 self-regression goldens under
`tests/visual/goldens/`. Those goldens answer whether this project changed
relative to itself. The Material visual contract binds future images to the
pinned Classic model and to the five evidence-backed renderer suites.

## Closed fixture and state matrix

The first visual family has one fixture:

```text
Classic / small Button / default density / label-only "Action"
5 variants × 5 states × 2 themes = 50 logical cases
```

Enabled and disabled are rendered with no feedback. Hover and focus use a
fully progressed fixed state layer. Press uses a centered ripple fixed at 50%
progress and no simultaneous fixed press layer. A direct test controller must
inject those phases; wall-clock animation and event-loop waits are forbidden.

## Deterministic raster contract

Every capture must use:

- seed `#6750A4`, standard contrast and the forced deterministic fallback color
  backend;
- `Fusion`, locale `C`, time zone `UTC`, left-to-right layout and the
  `offscreen` QPA plugin;
- device pixel ratio 1, a 160 × 80 logical canvas and a fixed 112 × 40 widget
  rectangle at `(24, 20)`;
- straight-alpha `RGBA8888`, sRGB, top-left origin and a PNG stripped of
  metadata;
- the target's exact font family at 14 points and 96 × 96 logical DPI, with
  substitution forbidden and the resolved face fingerprinted from stable font
  tables.

The runtime fingerprint is mandatory. A mismatch in runner image, operating
system, architecture, Qt build ABI, compiler, QPA plugin, style, font face,
device pixel ratio or logical DPI fails before artifacts are written. It also
records the operating-system version and resolved font style, weight and point
size.

## Isolated targets

| Target | Runner | Qt | Toolchain | Font |
|---|---|---:|---|---|
| Windows Qt 5 | `windows-2022` | 5.14.2 | MSVC 19.29 / v142 | Segoe UI |
| Windows Qt 6 | `windows-2022` | 6.7.3 | MSVC 19.29 / v142 | Segoe UI |
| Linux Qt 6 | `ubuntu-24.04` | 6.8.4 | GCC 13 | DejaVu Sans |
| macOS Qt 6 | `macos-15` | 6.8.4 | Apple Clang 16 | Helvetica |

Each target owns a distinct reference root. A Qt 5 image cannot satisfy a Qt 6
case, and an image from one operating system cannot satisfy another. The 50
logical cases therefore define 200 future reference paths.

## Locked comparison policy

The comparator requires all metrics to pass: zero differing pixels, zero
channel delta and SSIM 1.0. SSIM is fully specified as independent red, green,
blue and alpha scores over non-overlapping 8 × 8 windows. It uses population
variance/covariance, an 8-bit dynamic range, `K1=0.01` and `K2=0.03`; the final
score is the arithmetic mean of all window/channel scores. Canvas dimensions
must tile exactly.

Missing or unexpected files, dimension drift, malformed PNG data and runtime
environment drift fail before scoring artifacts are created. Masks,
cross-target substitution, runtime tolerance overrides and in-test reference
updates are forbidden.

## Actual-only runner

`qtmaterial3_material_button_capture` is built only with the visual test option.
It is not installed, exported or registered as a passing CTest. The runner:

- accepts only `--contract`, required `--target` and required `--output`;
- requires `QTMATERIAL3_VISUAL_RUNNER_IMAGE` to equal the selected pinned target;
- rejects a non-empty output directory and every output path inside a declared
  reference root;
- injects the five states directly, stops state-layer/elevation transitions and
  freezes press ripple progress at 0.5;
- renders hidden widgets through `QWidget::render`, without waits, event-loop
  pumping, shown windows or grab APIs;
- writes 50 normalized files under
  `actual/<component>/<state>/<theme>.png`, then atomically writes
  `manifest.json` only after the complete set exists.

On a matching built target, invoke it with a fresh artifact directory, for
example:

```bash
QTMATERIAL3_VISUAL_RUNNER_IMAGE=ubuntu-24.04 \
  ./qtmaterial3_material_button_capture \
  --target ubuntu-24.04.qt6.8.4.gcc-13 \
  --output /path/to/fresh/material-button-actual
```

The output conforms to
`references/material3/schema/visual-capture-manifest.schema.json`. Its SHA-256
values identify the exact contract bytes, canonical raw RGBA rows and runtime
fingerprint. The runner has no reference-write, update, tolerance or comparison
mode.

## Read-only comparator

`tools/compare_material_visual_capture.py` uses only the Python standard
library. It accepts a repository root, contract, target, actual capture and a
fresh artifact directory. The reference root is always derived from the target;
there is deliberately no reference-path option.

Before scoring, it requires:

- a target reference set explicitly marked `reviewed` and a compatible contract
  lifecycle status;
- closed actual and reference manifests bound to the exact contract SHA-256 and
  target;
- identical validated runtime fingerprints;
- exactly the declared manifest and image files, with no missing, unexpected or
  symbolic-link entries;
- metadata-free, non-interlaced 8-bit RGBA PNGs with valid chunks, CRCs,
  scanlines, dimensions and raw-pixel SHA-256 values.

Only after all 50 cases have passed preflight does it create the artifact
directory. Every failed case receives canonical `actual.png`, `diff.png` and
`metrics.json` files. The diff uses absolute RGB deltas and also exposes alpha
deltas as grayscale, with opaque output for review. `comparison-report.json` is
written last and conforms to the closed report schema.

Once a reference set is reviewed and activated, invocation is:

```bash
python tools/compare_material_visual_capture.py \
  --target ubuntu-24.04.qt6.8.4.gcc-13 \
  --actual /path/to/material-button-actual \
  --artifacts /path/to/fresh/material-button-comparison
```

The current contract intentionally refuses this command because every target is
still `pending`. There is no update, tolerance, mask or missing-reference mode.

## Repository gate

Run:

```bash
python tools/generate_material_button_visual_contract.py --check
python tools/check_material_visual_contract.py --root .
```

The checker uses only the Python standard library. It verifies schema closure,
generated freshness, exact model axes, renderer-suite evidence, pinned targets,
fixed interaction phases, canonical actual/reference inventories and the
absence of any reference file while the contract is `contract-only`. It also
audits the CMake wiring, private fixed-phase access, forbidden capture paths,
comparator metric implementation and absence of comparator bypasses. It runs
through `tools/repo_health.py` with no alternate pass mode, exception list or
tolerance environment variable.

## Current boundary

Phase 016.C1 certifies runner/comparator sources and synthetic comparator
behavior, not a runtime capture or Material pixels. No PNG or manifest is
committed, and the environment fingerprint has not been observed here. The next
increment must run all 50 cases on one exactly matching target, review the
actual-only manifest and atomically promote that set before comparison is
enabled in CI. Remaining platforms stay isolated work for 016.D.

Phase 017.A exposes that boundary in the registry-wide
[Material conformance report](material-conformance-report.md): the visual
contract gate passes, while every modeled Button remains `pending-runtime` for
visual rendering and no component is reported fully conformant.
