# Feature Specification: Visual Comparator

**Feature Branch**: `016-visual-comparator`
**Status**: Phase 016.C1 source complete / runtime capture and references unavailable
**Milestone**: Material Conformance Harness
**Created**: 2026-09-02

## Scope

Phases 016.A-B define the deterministic visual contract for all five required
Classic Button variants, five required states and light/dark themes and provide
its private actual-only Qt capture executable. The runner validates its target,
injects fixed interaction phases without time, renders normalized images and
emits a closed fingerprinted manifest. Phase 016.C1 adds a Qt-independent,
read-only comparator with closed reference/report manifests, strict normalized
PNG decoding, exact metrics and deterministic failure artifacts. No reference
image is accepted and no visual pass is claimed yet.

## Requirements

- **VC-FR-001** Derive component and state axes from the required ordered axes
  in `classic.buttons`.
- **VC-FR-002** Cover the exact 5 × 5 × 2 Cartesian product with 50 canonical
  logical cases.
- **VC-FR-003** Bind each variant to its separate evidence-backed renderer
  suite.
- **VC-FR-004** Fix the small/default-density/label-only `Action` fixture.
- **VC-FR-005** Inject enabled, disabled, hover, focus and press directly at
  deterministic phases, without wall-clock time or event-loop waits.
- **VC-FR-006** Fix seed, contrast, color backend, style, locale, time zone,
  layout direction, QPA platform and device pixel ratio.
- **VC-FR-007** Normalize canvas, widget rectangle, background role, pixel
  format, alpha, color space, origin, encoder and metadata policy.
- **VC-FR-008** Require exact 14-point font selection at 96 logical DPI and
  fingerprint the resolved face from its font tables; reject substitution.
- **VC-FR-009** Pin distinct Windows Qt 5, Windows Qt 6, Linux Qt 6 and macOS
  Qt 6 targets.
- **VC-FR-010** Give every target a separate reference root and forbid
  cross-target substitution.
- **VC-FR-011** Declare zero pixel/channel difference and SSIM 1.0, with every
  metric required.
- **VC-FR-012** Fail on missing, unexpected, dimension-mismatched or
  environment-mismatched references once comparison is active.
- **VC-FR-013** Forbid masks, runtime tolerance overrides and in-test reference
  updates.
- **VC-FR-014** Keep phase 016.A explicitly `contract-only` and reject premature
  reference files.
- **VC-FR-015** Validate a closed JSON Schema and deterministic generated
  content with Python's standard library only.
- **VC-FR-016** Exercise mutations for target pinning, font substitution,
  tolerance, state phases, Cartesian coverage, target substitution and schema
  closure.
- **VC-FR-017** Run the contract checker through repository health and CI with
  no exception or alternate pass mode.
- **VC-FR-018** Build capture as a private standalone executable, never as an
  installed/exported API and never as a passing CTest before comparison exists.
- **VC-FR-019** Inject widget interaction state and freeze state-layer,
  elevation and ripple phases through non-installed capture access only.
- **VC-FR-020** Render hidden widgets with `QWidget::render`; forbid waits,
  event-loop pumping, shown windows and grab-based capture.
- **VC-FR-021** Refuse an unknown runtime, non-empty output, or output inside a
  reference root before writing any artifact.
- **VC-FR-022** Emit exactly 50 canonical actual PNG paths and a manifest whose
  contract, runtime and raw normalized pixels are SHA-256 identified.
- **VC-FR-023** Keep the visual contract `contract-only` until a complete,
  reviewed target reference inventory exists.
- **VC-FR-024** Represent every target reference set explicitly as `pending` or
  `reviewed`; require lifecycle status and reviewed-set count to agree.
- **VC-FR-025** Decode only metadata-free, non-interlaced, 8-bit RGBA PNGs and
  verify CRC, dimensions, scanline structure and raw-pixel hashes.
- **VC-FR-026** Define SSIM as the arithmetic mean of independent RGBA channel
  scores over non-overlapping 8 × 8 windows, using population moments,
  `L=255`, `K1=0.01` and `K2=0.03`.
- **VC-FR-027** Validate exact actual/reference manifests, runtime
  fingerprints and complete file inventories before creating artifacts.
- **VC-FR-028** Emit normalized actual, opaque absolute-difference and JSON
  metric artifacts for failed cases, followed by a closed comparison report.
- **VC-FR-029** Derive the reference root from the selected contract target;
  forbid reference-path arguments, writes, updates, masks and tolerances.

## Current boundary

The 016.B runner and 016.C1 comparator sources are statically certified. The
comparator engine and diagnostics pass synthetic tests, but the runner has not
been compiled or executed in this environment. No actual/reference manifest or
PNG is committed, all four reference sets remain `pending`, and no visual pass
is claimed. A declared-target capture and human review remain mandatory before
single-target comparison can be activated. The existing opt-in theme
self-regression harness remains separate, and scope does not broaden beyond
Buttons.
