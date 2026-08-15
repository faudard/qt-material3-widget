# Typed Token Contract v1

`TokenId` encoding v1 is 32 bits:

- high 8 bits: stable TokenCategory;
- low 24 bits: stable category-local ID.

Domain code uses `ColorRole`, `TypeRole`, etc.
TokenId is for identity/provenance/tooling/interchange.

Text conversion exists only in ThemeIO through ThemeTextCodec.

`ComponentId` is a separate Foundation identity and is never encoded as a TokenId.

Existing numeric IDs may be reserved/deprecated but never reassigned to another semantic meaning.
