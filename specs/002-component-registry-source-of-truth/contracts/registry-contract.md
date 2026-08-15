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

A complete component requires all applicable maturity axes at 4, explicit evidence,
a review date and no gaps.

Support/helper headers are not automatically components.
