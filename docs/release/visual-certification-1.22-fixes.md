# 1.22 — Visual Certification & Rendering Fixes

This follow-up to #128 is based on the assisted review of 36 candidate PNGs
from source commit `8624f86c3aee183cb9239436d16063997837910d`. The historical
captures were reproducible, but repeatability is not visual acceptance.

| Finding | Correction / follow-up |
| --- | --- |
| M3-01 | Provide deterministic non-null destination icons, including the label-hidden variant. |
| M3-02 | Use a themed 420×240 host for the 360-wide side sheet, so the scrim is visible outside the rounded sheet. Reinspect modal/nonmodal corners; do not assume this was a production defect. |
| AD-01 | Include painted insets in button preferred width; verify full label against resolved metrics. |
| AD-02 | Mirror Compact Navigation Suite geometry once for paint, indicator, mouse hit testing and accessibility; ensure RTL keyboard traversal agrees. |
| EX-01 | Use logical QBoxLayout direction (Qt already mirrors); allocate the full group preferred width in the visual fixture. |
| EX-02 | Supply deterministic non-null toolbar action icons. |
| EX-03 | Use effective menu enabled state for item colors, checkmarks and state layers; mirror list item slots, text and divider. |
| HC-01 | Fallback contrast adjustment chooses the contrasting endpoint per actual foreground/background pair; accents can adjust the background to reach 7:1 in High. |

## Candidate refresh and review

The existing pinned Qt 6.4.0 / Fusion / xcb / 96 DPI CI generates independent
first/second passes. Capture export steps run even when an earlier strict
comparison against old references fails, provided the build succeeded.
Strict comparison itself remains blocking; no tolerance is increased.

Download the three evidence artifacts (missing-material3, adaptive-desktop,
expressive-catalogue) from this PR's CI run. Check repeatability JSON, then
review the 36 corrected candidates at native resolution. Review any other
changed family/navigation goldens too, because contrast and RTL fixes apply
to shared components. Do not overwrite approved PNGs merely to make CI green.

The two changed color-token JSON snapshots are deterministic data expectations,
not visual approvals. Verify them against the actual C++ test run. Only after
review, commit the matching PNGs and repeatability provenance, rerun strict CI,
and record the real human review using `tools/certification_center.py review-visual`.
The visual ledger stays pending in this PR; native screen-reader evidence and
separate DPR 2.0 and motion checks remain required for certification.


## Refresh after visual acceptance

The pinned CI comparisons intentionally fail against superseded PNGs. A rerun
without changing the checked-in references cannot fix those failures.
Download the four evidence artifacts from the **same** successful capture
run (the overall run may be red due to strict comparisons only):

    gh run download <RUN_ID> --dir build/review --pattern '*-visual-evidence'

The reports specify the actual rendered source SHA (for pull requests this can
be the synthetic merge SHA rather than the branch head). Obtain it from a
repeatability JSON file, fetch it into the local Git checkout if required, and
perform a read-only preflight:

    git fetch origin <RENDERED_SOURCE_SHA>
    python tools/refresh_visual_baselines_1_22.py --artifacts build/review --run-id <RUN_ID> --source-commit <RENDERED_SOURCE_SHA>

The importer reuses all four independent repeatability checkers and rejects
mixed commits, inconsistent reports or differing production/test sources.
After reviewing every changed/new image, repeat with
`--apply --confirm-reviewed`, review the resulting Git diff and commit the
PNGs and provenance JSON. This does not promote component maturity or mark
the certification ledgers as passed. Re-run strict CI on the updated branch.

Previous runs 44292887900 and 44347279314 are historical candidates, not
approved baselines. The latter shows a translucent Side Sheet scrim and the
correct Adaptive RTL order, but the Month segment still exhibited elision.
The follow-up changes fractional text metrics to avoid rounding a preferred
label width down before painting. Requalify the new render; do not import
the historical candidates after this source change.
