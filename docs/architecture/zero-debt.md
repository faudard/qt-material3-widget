# Architecture Zero-Debt Policy

QtMaterial3 enforces architecture rules with a zero-debt policy: a violation is
fixed in the same change that introduces it. Baselines, per-file waivers and
suppression markers are not accepted.

## Authoritative gate

Run:

```bash
python scripts/architecture/check_architecture.py --root .
```

The repository has one architecture authority:

- `scripts/architecture/check_architecture.py` is the checker engine;
- `scripts/architecture/architecture_rules.json` is the reviewed rule set;
- `cmake/QtMaterial3Architecture.cmake` exposes the same check through CMake
  and one CTest named `architecture`.

The checker covers source layering, scoped source contracts, repository-wide
forbidden markers, target graph boundaries and migrated component contracts.

## Baselines and suppressions

There is no architecture baseline. A detected violation is a failure.

Inline architecture suppressions, waivers, exceptions and architecture-specific
`NOLINT` markers are forbidden. Necessary architectural adaptations belong in
the reviewed architecture rules rather than in local escape hatches.

## Resolution boundary

Widgets and render helpers consume resolved specs. Theme-to-spec conversion
belongs in the designated resolver boundary; rendering, layout and motion code
must not perform ad-hoc theme resolution.
