# Tranche 21 — QtMaterialFab

`QtMaterialFab` inherits from `QtMaterialFilledButton`, which has already had its rendering/layout caches hidden behind PIMPL in the earlier button tranche.

This cleanup handles the accessibility state still stored directly in the public header. The public API remains unchanged.
