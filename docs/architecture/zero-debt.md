# Architecture Zero-Debt Policy

QtMaterial3 enforces architecture rules with a zero-debt policy: a violation is fixed in
the same change that introduces it. New baselines, per-file waivers and suppression
markers are not accepted.

## Authoritative gate

Run:

```bash
python tools/check_architecture_zero_debt.py --root .
```

The aggregate gate currently runs both architecture engines:

- `tools/check_architecture_contracts.py --strict`;
- `scripts/architecture/check_architecture.py`.

Both must report zero violations. They remain separate only because their rule sets are
not yet identical; consolidation must preserve rule coverage.

## Baseline

`tools/architecture_contracts_baseline.txt` is a historical empty tombstone. It must
contain no active entries and must not be used to accept new debt.

## Suppressions

Inline architecture suppressions, waivers, exceptions and architecture-specific
`NOLINT` markers are forbidden. Necessary architectural adaptations belong in the
reviewed architecture rules rather than in local escape hatches.

## Resolution boundary

Widgets and render helpers consume resolved specs. Theme-to-spec conversion belongs in
the designated resolver boundary; rendering, layout and motion code must not perform
ad-hoc theme resolution.
