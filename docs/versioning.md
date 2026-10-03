# Documentation versions

The documentation follows the library's stable-major policy.

- **latest** — documentation built from the current default branch.
- **1.0** — first stable API line. The 1.0 API-signature baseline remains the compatibility floor for the 1.x series.
- **1.1** — use the [1.0 to 1.1 migration ledger](migration/1.0-to-1.1.md) for consumer-visible changes as they land.

## Versioning policy

Tagged releases publish immutable documentation under a versioned path. The default branch publishes to latest. Links inside the documentation are relative so the same build can be hosted at either location.

Patch releases must not require migration instructions unless they correct previously documented behavior. Minor releases may add API; any consumer-visible behavioral or API migration belongs in the corresponding migration ledger. Breaking API changes require a new major version.

## Deprecation policy

A deprecated public API must identify its replacement in generated API documentation and be recorded in the migration ledger before removal. Removal is reserved for a major-version boundary.
