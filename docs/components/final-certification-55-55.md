# Final 55/55 Certification Center

The final QtMaterial3 catalogue contains 55 tracked components. After the
deterministic visual closure work, the remaining promotion gates are explicit
human review records:

- 1.5 Enterprise accessibility: six Navigation/Desktop components;
- 1.7 Missing Material 3: four components plus the three reviewed visual matrices;
- 1.9 Adaptive/Desktop: two components plus the 30 reviewed breakpoint matrices.

`tools/certification_center.py` provides one fail-closed workflow for those
three milestones. It never invents reader output and never promotes a pending
session.

## Show the remaining work

```bash
python tools/certification_center.py status
```

Machine-readable output:

```bash
python tools/certification_center.py status --json
```

A release or local closure check can require all evidence:

```bash
python tools/certification_center.py status --require-complete
```

This exits non-zero until every required native reader check and required visual
review is complete.

## Native AT session workflow

Generate a session template from the canonical ledger contract instead of
editing the ledger by hand:

```bash
python tools/certification_center.py template \
  --milestone 1.5 \
  --platform windows-nvda \
  --output build/certification/1.5-windows-nvda.json
```

Valid platforms are the platform identifiers already present in each ledger:

- `windows-nvda`;
- `linux-orca`;
- `macos-voiceover`.

The generated template starts with every component check set to `pending`.
Run the real certification fixture with the named reader, then change every
check to exactly `pass` or `fail` and fill:

- `reviewer`;
- `reviewedAt` in `YYYY-MM-DD`;
- `evidence` with a durable review/log/recording reference;
- optional `notes`.

Record the completed session only after the real native reader session:

```bash
python tools/certification_center.py record \
  --session build/certification/1.5-windows-nvda.json \
  --confirm-native-session
```

The command rejects:

- missing or extra components/checks;
- the wrong reader for the platform;
- invalid result values;
- any remaining `pending` check;
- missing reviewer/date/evidence;
- invalid dates.

The platform status is derived from the individual checks. All checks must pass
for the platform to become `pass`; one failing check makes the platform
`fail`.

Repeat the process for the three platforms in 1.5, 1.7 and 1.9.

## Visual review workflow

The deterministic goldens are already pinned and protected by strict CI, but
repeatability is not a visual approval.

After a reviewer has inspected every required committed golden for the
milestone, record the result:

```bash
python tools/certification_center.py review-visual \
  --milestone 1.7 \
  --status pass \
  --reviewer "Reviewer name" \
  --reviewed-at 2026-10-05 \
  --evidence "review/1.7-visual-001" \
  --confirm-reviewed-all-goldens
```

Use the same command with `--milestone 1.9` for the 30 Adaptive/Desktop
goldens. A failed visual review should be recorded as `--status fail`, fixed,
re-rendered, reviewed again, and only then recorded as `pass`.

The command verifies that every golden required by the ledger exists and keeps
the deterministic CI provenance when adding the manual review reference.

## Final promotion

Preflight all evidence and all three existing promotion helpers without changing
the repository:

```bash
python tools/certification_center.py promote
```

The command first requires complete certification state, then runs all
`--require-complete` checkers and dry-runs every promoter.

Only after that succeeds, apply the full chain:

```bash
python tools/certification_center.py promote --apply
```

The order is deliberate:

1. 1.5 Enterprise: 43/49 -> 49/49;
2. 1.7 Missing Material 3: 49/55 -> 53/55;
3. 1.9 Adaptive/Desktop: 53/55 -> 55/55.

Each existing promoter remains authoritative for its registry/rules mutation.
The Certification Center only orchestrates them after all evidence is complete.

## Safety rule

Do not use `--confirm-native-session` or
`--confirm-reviewed-all-goldens` merely to make the gate green. Those flags
are attestations that the named manual review was actually performed. Automated
QAccessible tests and deterministic PNG rendering complement native reader and
visual review; they do not replace them.
