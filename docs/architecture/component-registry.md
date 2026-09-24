# Component registry

`docs/components/component-registry.json` is the release source of truth for public component status.

`tools/component_registry.py` is the single Python domain model for registry
loading, maturity scoring and base validation. The status generator,
governance checker and release gate all consume that module rather than
reimplementing maturity policy.

The registry tracks:

- public component id
- family
- maturity
- public header
- spec type
- widget type
- test target
- gallery route
- documentation path
- explicit release and reference-candidate decisions
- ten evaluated maturity axes with evidence, gaps and next actions

Generate the human-readable status files with:

```bash
python3 scripts/generate_component_status.py
```

Validate the closed registry and all generated outputs with:

```bash
python3 tools/check_component_registry.py --check-generated --strict
```

Generated outputs:

- `STATUS.md`
- `docs/component-status.md`
- `docs/components/maturity.md`

Maturity levels:

| Level | Meaning |
|---|---|
| `complete` | API, docs, tests, accessibility, keyboard and gallery coverage are release-ready. |
| `usable` | Suitable for real use with known non-blocking gaps. |
| `partial` | Implemented but missing important release criteria. |
| `skeleton` | Structural placeholder or incomplete implementation. |
| `planned` | Not implemented yet. |

A 1.0 release should not contain P0 components marked `skeleton` or `planned`.
The Quality workflow runs complete repository health in strict mode, so implicit
registry metadata, unevaluated axes and stale generated artifacts cannot merge.
All generated status documents are derived from the same canonical JSON inventory.
