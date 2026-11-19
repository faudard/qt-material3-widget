# 1.22 — Quality & Certification 2.0

This is the implementation/qualification workspace, **not** a claim that 1.22
has been released or that all 60 components have passed human certification.
Scope is defined in [the 1.22 roadmap](../roadmap-2.0.md).

## Authoritative state and dashboard (1.22.C)

The canonical component maturity source is
[component-registry.json](../components/component-registry.json); it currently
tracks **60 components: 43 complete, 17 usable** (snapshot 2026-10-22).
The following existing ledgers own manual native AT and visual evidence:

| Milestone | Components in release scope | Evidence |
| --- | --- | --- |
| 1.5 | Navigation Rail, Tabs, Menu, Breadcrumb, Command Palette, Split View | enterprise-accessibility-1.5.json |
| 1.7 | Navigation Bar, Side Sheet, Tooltip, Badge | material3-catalogue-certification-1.7.json |
| 1.9 | Adaptive Shell, Navigation Suite | adaptive-desktop-certification-1.9.json |
| 1.10/1.11 | Split Button, Button Group, Floating Toolbar, Loading Indicator, Segmented List | expressive-certification-1.10-1.11.json |

The expressive ledger also tracks additional integration cases; only the
ledger's explicit requiredComponents count toward the 1.22 release gate.

Run the new read-only, per-component release-health report:

    python tools/certification_report_1_22.py --format markdown
    python tools/certification_report_1_22.py --format json --output build/certification/health-1.22.json
    python tools/certification_report_1_22.py --validate
    python tools/certification_report_1_22.py --require-complete

The report derives remaining maturity-axis gaps, per-platform native AT check
counts, reviewer/date/evidence provenance, visual review states, renderer
metadata, required vs. missing committed goldens and component-level next
actions. --validate fails on malformed contracts or unsupported claims but
**does not fail on honestly pending manual reviews**; --require-complete is the
release gate and currently expected to fail. All 60 tracked components are
in scope for this final gate, including the 11 components whose historical
releaseScope flag is still false. Expressive Menu and Chip integration checks
are reported separately from the 17 standalone components.

CI runs --validate plus the Python unit suite and publishes machine-readable
JSON and human-readable Markdown as downloadable Actions artifacts. It does
not write to or manually patch STATUS.md or registry maturity. To regenerate
the canonical maturity documents, use:

    python scripts/generate_component_status.py
    python tools/check_component_registry.py --check-generated --strict

## Native assistive-technology verification (1.22.A)

QAccessible unit tests are **not** equivalent to actual screen-reader spoken
announcements. For each required ledger/platform, run a real session with
NVDA on Windows, Orca on Linux and VoiceOver on macOS. Test actual traversal,
role, selected/disabled/busy announcements, keyboard activation, focus
restoration, modal containment, RTL direction and responsive transitions, as
applicable to that ledger's component-specific check names.

Use the existing fail-closed session tooling:

    python tools/certification_center.py template --milestone 1.7 --platform windows-nvda --output build/at-session.json

Edit each result after operating the actual named screen reader. Record tester
identity, date, evidence artifact/link and reproducible failures, then run:

    python tools/certification_center.py record --session build/at-session.json --confirm-native-session
    python tools/certification_center.py status

For a ready-to-use review worklist and all outstanding session templates:

    python tools/export_certification_review_pack.py --output build/certification/review-pack

Use a fresh output directory on each export; existing session files are never
overwritten. The same pack is available in the Quality CI artifact, including
Gallery routes, exact check names and required golden paths. Its pending
checklists are not certification evidence on their own.

### Detailed native review sessions (1.22 final closure)

Quality CI now publishes 12 version-2 JSON templates and 12 matching step-by-step
Markdown guides inside `certification-health-1-22/review-pack/native-detailed`.
They cover all 76 milestone-specific scenarios (228 executions across the three
native readers), including Expressive Menu and Chip integration checks.

On a checkout containing the tested Gallery build:

    python tools/native_at_review_1_22.py prepare --output build/native-review

Start the actual screen reader, open the named Gallery route with
`qtmaterial3_gallery --route <route>` and follow the corresponding Markdown
guide. Use NVDA on Windows, Orca on a Linux accessibility-enabled graphical
session, or VoiceOver on macOS, not the offscreen Qt platform or a simulated
reader. Record the actual reader and OS versions, tested Qt build, locale,
full Git SHA of the build (`git rev-parse HEAD`), reviewer/date and a reusable
evidence location (speech log, recording or dated session report).
For each check record pass/fail and actual announced words or observed behavior
in observations. If a Gallery fixture lacks the necessary state/control,
leave the check pending and document what must be added; do not assume success.
The JSON can be edited in multiple sessions before recording the final results.

    python tools/native_at_review_1_22.py validate --session build/native-review/1.7-windows-nvda.json
    python tools/native_at_review_1_22.py record --session build/native-review/1.7-windows-nvda.json --confirm-native-session

The detailed recorder rejects incomplete observation sets, missing environment
metadata and tested builds that differ in implementation, headers, Gallery or
runtime tests. It stores the original detailed session in
`docs/components/native-at-1.22/` and updates the existing version-1 ledger
through the certification center. Commit both files in the evidence PR. A
replacement real run requires `--replace`. The original version-1 recorder
remains compatible; the detailed recorder is the recommended 1.22 workflow.
This is a human-recorded evidence contract, not a machine-verifiable guarantee
that an actual reader was used.

Repeat for all three platforms and all four milestone contracts. Do not mark
checks pass solely because a QtTest, QAccessible or headless smoke test is
green. For a failing session, retain the failure and reproducer; do not raise
the relevant registry accessibility score until every required native run
is reviewed.

## Deterministic visual references (1.22.B)

The existing 1.7 missing-M3, 1.9 adaptive desktop **30-image** matrix and
1.10/1.11 expressive ledgers list their required reference PNGs. Preserve
the pinned Qt **6.4 / Fusion / xcb** renderer provenance, use independent
repeatable captures, manually inspect the actual candidate images for
light/dark/high contrast, RTL and state coverage, and verify 1x/2x DPR where
applicable. A status of pending remains pending until the evidence exists
and has been inspected; this dashboard does not automatically turn
repeatability into approval.

To record a review after confirming **every committed** required golden was
inspected (not a screenshot attachment absent from the repo):

    python tools/certification_center.py review-visual --milestone 1.9 --status pass --reviewer "Actual reviewer" --reviewed-at YYYY-MM-DD --evidence "CI run/artifact and written review" --confirm-reviewed-all-goldens

Before sign-off, verify the 63 committed reference PNGs and the four
repeatability reports from merged PR #133:

    python tools/verify_visual_baselines_1_22.py

This verifies the recorded SHA256, dimensions, source commit, family and pinned
renderer. Quality CI enforces it as a separate integrity check. It neither
marks a review passed nor claims that high-DPI, animations or disabled-state
appearance have been manually approved.

Use the existing tools/check_*visual_repeatability.py checkers to verify
capture metadata. Candidate, approved review and checked-in golden are three
distinct states; current ledger status, not merely a PNG filename, decides
the reported certification.

## Gate and limitations

Only when actual screen-reader sessions and reviewed committed visual goldens
exist should the existing promotion workflow be considered:

    python tools/certification_center.py promote
    # Review the preflight; only then:
    python tools/certification_center.py promote --apply
    python tools/certification_report_1_22.py --require-complete

Run the installed API/public-header audits, real Qt 5/6 consumers, CTest,
sanitizers and cross-OS packaging before release sign-off. The Python-only
report validates evidence contracts; **it cannot run the native graphical
matrix, operate physical screen readers, certify DPR rendering, or substitute
for cross-platform CI/manual reviewer approval**.

A merge of this PR is tooling progress toward 1.22, not a declaration of
60/60 complete or a version tag.
