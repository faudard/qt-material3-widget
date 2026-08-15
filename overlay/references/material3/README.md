# QtMaterial3 Material References

This directory contains **versioned conformance reference metadata**, not QtMaterial3
golden screenshots.

Profiles are separate:

```text
classic/<reference-version>/
expressive/<reference-version>/
```

Classic and Expressive data must never overwrite each other.

## Source precedence

1. Material Design guideline/spec pages define normative design intent.
2. A pinned AndroidX Material3 implementation is the executable reference used to produce
   comparable measurements/renders.
3. AndroidX release notes establish artifact version/release provenance.

If guideline intent and executable AndroidX behavior differ, the conformance work must
record that difference explicitly. It must not silently change a MUST_MATCH expectation.

## Case status

`seeded` means the case identity/matrix is defined but evidence assertions are not complete.

`evidence-backed` means every case has source-backed machine-readable assertions and may be
used for structural conformance scoring.

Spec 013 seeds 50 Classic button cases. Specs 014-016 add evidence and rendering comparison.
