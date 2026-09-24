# API stability policy

QtMaterial3 remains pre-1.0. Public C++ APIs may still change until the 1.0
surface is frozen, while the Theme JSON persistence contract is already
versioned independently.

## Theme JSON

`formatVersion: 1` is the supported persistence format. Breaking persistence
changes require a new format version; compatible additions must preserve strict
validation and deterministic serialization guarantees documented by the schema
tests.

## Pre-1.0 C++ surface

Before 1.0 the project may remove or rename public C++ APIs directly when doing
so reduces accidental surface area. Compatibility shims are intentionally not
kept for unpublished APIs.

The 0.9 workstream performs the final public-header and package-component audit.

## Backend behavior

`QTMATERIAL3_USE_MCU` means "request the MCU backend." It does not guarantee
that MCU sources are present. When unavailable, the library must produce the
documented deterministic fallback colors and expose the effective backend
through its diagnostic API.
