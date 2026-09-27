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

The 0.9 release is the API-freeze candidate. Its release checker locks the intended
public-header ownership, application-facing package components, resolver visibility,
compatibility-shim policy, and retained C++ test registration. 1.0 establishes the
first stable source/API baseline from that candidate.

The stable baseline is generated from Doxygen XML and is restricted to the canonical
`QTMATERIAL3_PUBLIC_HEADERS` install manifest. It records public/protected class
surface plus namespace-level enums, typedefs, variables and free functions from those
installed headers. Signatures also retain public-header ownership and enum initializers
rather than hashing whole headers. Private/internal source headers and PIMPL state are
therefore excluded from the stable contract.

Within the same stable major, removing or changing a baseline signature is a breaking
change and fails the gate. New declarations are additive and are allowed without
rewriting the original 1.0 compatibility baseline.

## Backend behavior

`QTMATERIAL3_USE_MCU` means "request the MCU backend." It does not guarantee
that MCU sources are present. When unavailable, the library must produce the
documented deterministic fallback colors and expose the effective backend
through its diagnostic API.
