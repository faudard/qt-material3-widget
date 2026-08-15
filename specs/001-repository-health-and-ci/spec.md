# Feature Specification: Repository Health and CI

**Status**: Ready / cumulative patch corrected in Spec 002 bundle  
**Milestone**: 0.5

The repository health gate provides a complete `quality.yml`, a local `tools/repo_health.py`,
architecture validation and registry/generated-document validation.

Default mode remains brownfield-aware. `--strict` is reserved for the zero-debt gate.

**Correction carried by the cumulative Spec 002 patch:** the top-level workflow-key smoke
checker accepts scalar mappings such as `name: Quality` as well as empty mappings such as `jobs:`.
