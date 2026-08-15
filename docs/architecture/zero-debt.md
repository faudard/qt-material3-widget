# Architecture Zero-Debt Policy

QtMaterial3 0.5 changes architecture enforcement from **baseline-managed debt** to
**zero-debt enforcement**.

## Authority

A change is architecture-clean only when all of the following pass:

```text
tools/check_architecture_contracts.py --strict
scripts/architecture/check_architecture.py
baseline active entries == 0
architecture suppression markers == 0
```

`tools/check_architecture_zero_debt.py` runs that aggregate contract.

## Baseline retirement

`tools/architecture_contracts_baseline.txt` remains as a historical tombstone with comments
only. It may not contain active keys.

The former `--update-baseline` workflow is not part of normal CMake/CI anymore. New debt is
fixed in the same change that introduces it.

## No hidden waivers

Inline markers such as `ARCH-IGNORE`, architecture waivers/exceptions or architecture
specific `NOLINT` markers are forbidden. A genuinely necessary architectural adaptation
must be encoded as a reviewed rule/boundary in the architecture configuration, not as a
per-file escape hatch.

## Two checkers, one gate

The repository currently has two architecture engines with partially overlapping scopes.
Spec 011 deliberately runs both. Consolidating them is allowed later only after rule parity
is demonstrated.

## Resolved-spec migration

The principal remaining legacy family is widget/render code reading Theme after resolution.

The required direction is:

```text
Theme + authored state
        ↓
dedicated resolver boundary
        ↓
ResolvedSpec / concrete runtime values
        ↓
widget layout / motion / rendering
```

A render helper must not receive `Theme` merely to resolve shape radius, state-layer opacity,
font, motion or elevation. Those values belong in the resolved spec.

ThemeContext plumbing is runtime infrastructure; Theme-to-Spec conversion must live only in
the designated resolution boundary.

## Gate discipline

During convergence use:

```powershell
python tools/architecture_zero_debt_inventory.py
python tools/check_architecture_contracts.py --strict
```

When the inventory reaches zero:

```powershell
python tools/check_architecture_zero_debt.py
```

Only then is Spec 011 sealed.
