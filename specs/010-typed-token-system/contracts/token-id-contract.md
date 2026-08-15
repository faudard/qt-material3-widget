# Typed Token ID Contract
- Runtime/spec APIs use typed identities; raw persistence text is not accepted.
- Category mismatch is rejected by type design or validation.
- ThemeIO maps each known textual key to at most one typed ID per namespace.
- Known IDs round-trip deterministically.
- Unknown text yields explicit diagnostics, never silent aliasing.
- Existing persistence keys remain stable unless schema-version migration changes them.
