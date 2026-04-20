# QtMaterialAutocompletePopup focused feature pass

This archive is a focused patch/skeleton pass for adding `QtMaterialAutocompletePopup`
into the shared text-field family.

It is intended to be applied on top of the integrated `qt-material3-widgets` branch.
It is **not** a compiled verification result.

## Goals

- Add a reusable, spec-driven autocomplete popup widget.
- Keep popup behavior separate from text editing.
- Let outlined and filled text fields host the popup through the shared shell path.
- Keep popup rendering aligned with the surface family.

## Included artifacts

- `AutocompletePopupSpec` public spec draft
- `QtMaterialAutocompletePopup` public widget draft
- private popup state/model helper draft
- test scaffolding
- render test scaffolding
- development checklist
- `tests/CMakeLists.txt.patch`
