# Component Registry Governance

`docs/components/component-registry.json` is the canonical inventory for QtMaterial3 component identity and maturity.

## Schema

The current registry data format is governed by:

`docs/components/component-registry.schema.json`

Schema version: **1**.

The data file remains a JSON array in version 1. Schema versioning is independent from project/library versioning.

## Canonical fields

Each tracked component owns:
- stable component `id`;
- user-facing `name`;
- `family`;
- declared/effective `maturity`;
- `publicHeader`;
- `specType`;
- `widgetType`;
- `testTarget`;
- `galleryRoute`;
- `docsPath`;
- explicit `maturityPolicy`, `releaseScope` and `referenceCandidate` metadata;
- all ten maturity-axis values;
- a review date, gaps, next actions and non-empty evidence for every axis.

## Generated outputs

These are derived data and MUST NOT be manually maintained:
- `STATUS.md`;
- `docs/component-status.md`;
- `docs/components/maturity.md`.

Use:

```text
python scripts/generate_component_status.py
```

and check drift with:

```text
python tools/check_component_registry.py --check-generated
```

Strict release/CI validation is:

```text
python tools/check_component_registry.py --check-generated --strict
```

There is no unevaluated-axis or implicit-metadata migration mode left. Missing
metadata, an absent axis, empty evidence, a future review date, a declared
maturity inconsistent with its evaluated scores, or a non-complete component
without an explicit gap fails governance. The generated maturity matrix must
report zero unevaluated axes.

Scores below `complete` remain deliberately conservative. A score of `1` means
only basic or incomplete evidence; `2` means usable simple-case evidence.
Neither is a full Material-conformance claim. Visual, accessibility, RTL and
HiDPI evidence is recorded directly in the registry maturity axes and component tests.

## Complete maturity policy

A component declared/effectively `complete` must:
- have all maturity axes explicitly evaluated;
- use score `4` for every applicable axis;
- use `N/A` only where an axis is genuinely not applicable and evidence explains why;
- have no declared maturity gap;
- carry non-empty evidence for each axis;
- have a valid ISO review date;
- have a test target, public header, documentation and Gallery route.

Every non-complete component must keep at least one concrete gap and one next
action. Its declared maturity must equal the maturity derived from the recorded
axis values.

## Registry scope

The registry is the source of truth for **component maturity**, not yet for every installed support/helper header. Public/private header classification is handled by the dedicated header-boundary workstream.

This avoids misclassifying support types such as variant/helper headers as independent Material components.
