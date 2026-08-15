# Feature Specification: Repository Health and CI

**Feature Branch**: `001-repository-health-and-ci`  
**Status**: Ready for implementation / patch supplied  
**Milestone**: 0.5 Architecture Foundation  
**Audit date**: 2026-08-15

## Confirmed current-state facts

1. `.github/workflows/quality.yml` currently starts directly with `docs:` and lacks the workflow-level `name`, `on` and `jobs` mappings.
2. `.github/workflows/ci.yml` is a complete workflow and currently exercises Qt 6 on Ubuntu, Windows and macOS.
3. `tools/check_architecture_contracts.py` already supports a brownfield baseline and a future `--strict` mode.
4. `tools/architecture_contracts_baseline.txt` currently contains 80 accepted architecture-debt entries.
5. `scripts/generate_component_status.py` already supports `--check` and `--strict`; `--check` verifies generated status files without rewriting them.
6. Documentation CI already runs Doxygen XML validation and Sphinx with `--fail-on-warning`.

## Required outcome

A pull request must have a trustworthy quality workflow whose repository-health job fails when:
- the quality workflow loses its basic workflow wrapper;
- architecture introduces a new violation;
- an architecture baseline entry becomes stale;
- generated component status/maturity documents are out of date.

Existing baselined architecture debt is permitted during spec 001. Zero debt is enforced later by spec 011.

## Requirements

- **RH-FR-001**: `quality.yml` MUST be a complete GitHub Actions workflow.
- **RH-FR-002**: Quality CI MUST run a local repository-health entry point.
- **RH-FR-003**: Repository health MUST invoke the existing architecture checker rather than duplicate architecture rules.
- **RH-FR-004**: Default repository health MUST be baseline-aware and MUST reject new/stale architecture debt.
- **RH-FR-005**: Repository health MUST verify generated component status using the generator's `--check` mode.
- **RH-FR-006**: Repository health tooling MUST have automated tests for its own workflow-structure and exit-code behavior.
- **RH-FR-007**: Documentation generation/validation MUST remain blocking.
- **RH-FR-008**: A future `--strict` mode MUST be available for spec 011 without changing the command architecture.

## Acceptance

```text
python -m unittest discover -s tests/tools -p "test_repo_health.py" -v
python tools/repo_health.py
```

Both commands return zero on the expected brownfield repository state.

`python tools/repo_health.py --strict` is intentionally expected to fail until architecture debt reaches zero.
