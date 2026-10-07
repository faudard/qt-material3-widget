# Final 60/60 Certification Center

This runbook closes the current QtMaterial3 component registry without adding new widgets.

The tracked catalogue currently contains **60 components**. The certification chain remains
fail-closed: automated checks may prepare evidence and validate contracts, but they must not
invent native screen-reader observations or claim that visual references were reviewed when
they were not.

## Closure chain

The unified operator workflow covers four certification stages in order:

1. **1.5 Enterprise accessibility** — the six remaining release-scope navigation/layout
   components, promoting the historical catalogue from 43 to 49 complete components.
2. **1.7 Missing Material 3** — Navigation Bar, Side Sheet, Tooltip and Badge, promoting
   49 to 53.
3. **1.9 Adaptive/Desktop** — Navigation Suite and Adaptive Shell, promoting 53 to 55.
4. **1.10/1.11 Expressive Catalogue/Data** — Split Button, Button Group, Floating Toolbar,
   Loading Indicator and Segmented List, promoting 55 to the current **60/60** registry.

Menu and Chip Expressive modes are certified in the 1.10/1.11 native AT ledger, but they are
already complete registry components and are not promoted a second time.

## Status

Use the Certification Center for one combined view:

```bash
python tools/certification_center.py status
python tools/certification_center.py status --json
```

The command reports native AT pass/fail/pending counts for every stage and visual review state
for 1.7, 1.9 and 1.10/1.11.

A CI or release gate that requires every manual record to be complete can use:

```bash
python tools/certification_center.py status --require-complete
```

That command is expected to fail while real evidence is still pending.

## Native assistive-technology sessions

Generate a session template from the canonical ledger contract:

```bash
python tools/certification_center.py template \
  --milestone 1.10/1.11 \
  --platform windows-nvda \
  --output build/certification/expressive-windows-nvda.json
```

Supported platform/reader pairs are defined by each ledger and currently use:

- `windows-nvda` / NVDA;
- `linux-orca` / Orca;
- `macos-voiceover` / VoiceOver.

The generated template starts with every check set to `pending`. After executing the real
reader session, fill every result with `pass` or `fail` and provide:

- `reviewer`;
- `reviewedAt` in `YYYY-MM-DD`;
- `evidence` pointing to a durable log, review record or recording.

Then record it:

```bash
python tools/certification_center.py record \
  --session build/certification/expressive-windows-nvda.json \
  --confirm-native-session
```

The recorder rejects pending checks, contract drift, wrong reader/platform pairings, missing
metadata and invalid result values. Platform status is derived from the child checks; callers
cannot simply declare a platform passing.

Repeat the process for every required platform and milestone.

## Visual review

Deterministic visual generation and repeatability remain automated. Human approval remains
explicit.

For milestones with visual certification, first inspect every required committed golden, then
record the review:

```bash
python tools/certification_center.py review-visual \
  --milestone 1.10/1.11 \
  --status pass \
  --reviewer "Reviewer name" \
  --reviewed-at 2026-10-06 \
  --evidence "review/expressive-visual-001" \
  --confirm-reviewed-all-goldens
```

The command verifies that every required golden exists before accepting the review and preserves
deterministic CI provenance when manual evidence is appended.

A failed visual review should be recorded as `fail`, fixed, regenerated and reviewed again.

## Expressive production contracts

The final five standalone Expressive components have an additional executable production gate:

```bash
ctest --test-dir build --output-on-failure \
  -R "^tst_(expressive_catalogue|expressive_production_contracts)$"
```

`tst_expressive_production_contracts` covers the closure-specific contracts that were not
sufficiently represented by the initial functional test:

- disabled action behavior and accessibility metadata;
- keyboard traversal;
- RTL propagation/mirroring;
- DPR 2.0 rendering smoke;
- list selection/navigation and state contracts.

The final Expressive promoter also requires the public headers, documentation, Gallery routes
and registered CTest ownership to remain present before any registry mutation is allowed.

## Preflight and promotion

Once all real evidence is complete, preflight the entire chain without modifying files:

```bash
python tools/certification_center.py promote
```

The command runs every `--require-complete` checker and every promotion helper before allowing
mutation.

Apply the complete chain only after preflight succeeds:

```bash
python tools/certification_center.py promote --apply
```

The order is authoritative:

1. 1.5 -> 49 complete;
2. 1.7 -> 53 complete;
3. 1.9 -> 55 complete;
4. 1.10/1.11 -> **60 complete**.

The last stage promotes only the five standalone Expressive components. It raises their derived
maturity axes to 4/4 only after the automated production contracts, reviewed visual evidence
and native reader evidence are all present, then regenerates the status documents and runs the
base release checker.

## Fail-closed rule

Do not use `--confirm-native-session` or `--confirm-reviewed-all-goldens` merely to make a
gate green. Those flags are attestations that the named manual activity actually occurred.

Automated Qt accessibility tests, deterministic render repeatability and CTest coverage are
necessary evidence, but they do not replace NVDA, Orca, VoiceOver or human visual review.
