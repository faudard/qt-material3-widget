# Feature Specification: Component Registry Source of Truth

**Feature Branch**: `002-component-registry-source-of-truth`  
**Created**: 2026-08-15  
**Status**: Strict static closure complete / external CI execution pending
**Milestone**: 0.5 Architecture Foundation

## Confirmed current state

The repository already has a strong foundation:
- `docs/components/component-registry.json` is the source consumed by the status generator;
- `docs/component-status.md` states that it is generated and must not be edited directly;
- the generator validates IDs, required fields, maturity values/policies, headers, docs paths,
  release-scope test targets, maturity axes and selected button-specific completion rules;
- generated outputs are `STATUS.md`, `docs/component-status.md`, and `docs/components/maturity.md`.

The missing layer is **registry governance**: a versioned schema and global invariants that make
component identity/maturity safe to use as the canonical project inventory.

## Requirements

- **REG-FR-001**: Publish a versioned JSON Schema for the current registry format.
- **REG-FR-002**: Preserve the current JSON-array data format in schema v1; do not churn 900+ lines merely to add an envelope.
- **REG-FR-003**: Reuse the existing generator/validator as the authority for path/test/maturity mechanics.
- **REG-FR-004**: Component IDs, public headers, widget types and Gallery routes MUST be unique.
- **REG-FR-005**: A `complete` component MUST have every maturity axis evaluated at 4 or explicitly `N/A`.
- **REG-FR-006**: A `complete` component MUST have non-empty evidence per axis, an ISO review date, and no gaps.
- **REG-FR-007**: A `referenceCandidate` MUST be `complete`; schema v1 permits at most one reference candidate per family.
- **REG-FR-008**: `maturityPolicy`, `releaseScope` and `referenceCandidate` MUST be explicit for every component.
- **REG-FR-009**: Generated status documents MUST be checked deterministically via the existing generator `--check`.
- **REG-FR-010**: Repository health MUST invoke the registry governance checker.
- **REG-FR-011**: Registry tooling MUST require only the Python standard library.
- **REG-FR-012**: Registry scope is component maturity; support/helper public headers remain the responsibility of the public/private-header boundary spec.
- **REG-FR-013**: Every component MUST evaluate all ten maturity axes and carry non-empty evidence for each axis.
- **REG-FR-014**: Every non-complete component MUST declare a gap and a next action; declared and derived maturity MUST agree.
- **REG-FR-015**: Quality CI MUST execute repository health with explicit strict mode and reject workflow drift back to normal mode.

## Acceptance

```text
python -m unittest discover -s tests/tools -p "test_*.py" -v
python tools/check_component_registry.py --check-generated --strict
python tools/repo_health.py --strict
```

The current registry contains 32 closed entries and zero unevaluated axes. The
24 non-Button assessments retain conservative scores and explicit gaps; this
governance closure does not promote their Material-conformance status.
