# Theme JSON schema

The theme serializer now writes `formatVersion: 2` by default.

Version 2 separates source inputs from resolved theme tokens:

- `source`: seed color, light/dark mode, contrast mode, expressive flag, and Material Color Utilities request flag.
- `resolved`: color scheme, typography scale, shape scale, elevation scale, motion tokens, state layer, density, icon sizes, and component-local overrides.
- `metadata`: generator version, library version, Qt version, and an optional timestamp.

The timestamp is intentionally optional so deterministic golden snapshots can omit it.

## Read modes

`ThemeReadMode::Lenient` accepts known v1/v2 structures and ignores forward-compatible extra fields.

`ThemeReadMode::Strict` accepts only the current schema version and rejects unknown root/source/resolved keys or missing required resolved token groups.

`ThemeReadMode::UpgradeIfPossible` accepts v1 documents and maps legacy top-level resolved blocks into the current in-memory `Theme` model.
