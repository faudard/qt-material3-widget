# API stability policy

QtMaterial3 1.x treats the documented installed C++ surface as a stable source API.
The canonical surface is the public-header manifest used by installation and release
validation.

## Theme JSON

`formatVersion: 1` is the supported persistence format. Breaking persistence
changes require a new format version; compatible additions must preserve strict
validation and deterministic serialization guarantees documented by the schema
tests.

## Stable 1.x C++ surface

The 1.0 baseline is generated from Doxygen XML and restricted to the canonical
`QTMATERIAL3_PUBLIC_HEADERS` install manifest. It records public/protected class
surface plus namespace-level enums, typedefs, variables and free functions from those
installed headers. Signatures also retain public-header ownership and enum initializers.

Private/internal source headers, resolver implementation headers and PIMPL state are
excluded from the stable contract.

Within the 1.x line:

- removing or changing a baseline signature is a breaking source-compatibility change
  and fails the API baseline gate;
- additive public declarations are allowed without rewriting the original 1.0 baseline;
- replacement of stable API should use an explicit deprecation path before removal from
  a future major version.

Native Qt child accessors documented as extension points remain parent-owned. Callers
may configure their supported Qt behavior but must not delete or reparent those child
objects.

## ABI

Source compatibility is the stable 1.x contract. Binary compatibility is best-effort
unless a stricter ABI policy is published for a future release.

## Backend behavior

`QTMATERIAL3_USE_MCU` means "request the MCU backend." It does not guarantee
that MCU sources are present. When unavailable, the library must produce the
documented deterministic fallback colors and expose the effective backend
through its diagnostic API.
