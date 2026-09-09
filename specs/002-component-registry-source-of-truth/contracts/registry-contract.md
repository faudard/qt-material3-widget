# Component Registry v1 Contract

Canonical data:
`docs/components/component-registry.json`

Schema:
`docs/components/component-registry.schema.json`

Generated:
- `STATUS.md`
- `docs/component-status.md`
- `docs/components/maturity.md`

Validator layering:
1. existing `scripts/generate_component_status.py`;
2. `tools/check_component_registry.py` governance invariants.

Every component requires explicit release/reference metadata, all maturity axes,
per-axis evidence, a review date and next actions. A non-complete component must
declare a gap. A complete component requires every applicable axis at 4 and no
gaps. Declared and derived maturity must agree.

Strict authority:
`python tools/check_component_registry.py --check-generated --strict`

Support/helper headers are not automatically components.
