# Architecture Zero-Debt Contract v1

A repository revision is architecture-clean iff:

1. `tools/check_architecture_contracts.py --strict` returns 0.
2. `scripts/architecture/check_architecture.py` returns 0.
3. `tools/architecture_contracts_baseline.txt` has zero active entries.
4. No source/build script contains an architecture-specific suppression marker.
5. The required physical target graph matches the post-Spec-010 layering contract.

No per-file waiver and no baseline debt is permitted after 0.5 architecture convergence.
