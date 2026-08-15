# Plan

1. Introduce Foundation `TokenId` and `TokenCategory`.
2. Move stable `ComponentId` into Foundation.
3. Add ThemeModel enum->TokenId mappings and complete typed role enumerations.
4. Extend ThemeTextCodec with TokenId text conversion.
5. Migrate ThemeSerializer away from duplicated token-name arrays.
6. Add stability/uniqueness/text-round-trip tests.
7. Add a checker that forbids text in the model mapping and QVariant universal lookup.
8. Run serializer compatibility fixtures and Qt5/Qt6 shared/static matrices.

The universal ID is an introspection/interchange identity, not a replacement for domain
types in component resolution.
