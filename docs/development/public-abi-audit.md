# Public API and ABI guidance

Public widget headers under `include/qtmaterial/widgets` should expose API,
not implementation state. Resolved specs, rendering/layout caches, child widget
internals, effects/controllers and private navigation state belong in
implementation or private headers.

## Validation

The old `check_public_widget_abi.py` CMake hook was retired because its backing
script was no longer present. The maintained automated contract is the unified
API/header-surface gate:

```bash
python3 tools/check_api_surface.py --root . --scope source
```

It validates the reviewed public/private manifest, public include boundaries,
explicit install contract and CTest self-contained-header coverage. Installed
packages are checked with:

```bash
python3 tools/check_api_surface.py --root . --scope installed --prefix <prefix>
```

This is a source/API-surface guardrail, not a promise of automated binary ABI
compatibility. Binary compatibility changes still require explicit review.
