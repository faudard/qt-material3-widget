# Material Reference Contract v1

A conformance identity is:

```text
profile version + component variant + interaction state + theme mode
```

A component model additionally defines:

- required variants and their product-registry IDs;
- structural slots and cardinality;
- required states;
- semantic token roles and exact `ResolvedSpec` paths;
- expected behavior;
- explicit Qt runtime-support fields.

Every classified item uses exactly one of:

```text
MUST_MATCH
PLATFORM_ADAPTATION
QTMATERIAL_EXTENSION
```

Adaptations and extensions require rationale. Evidence assertions also require a
domain, semantic path, operator, expected value, tolerance when applicable, one
or more profile source references, and immutable file/line evidence. At least one
evidence locator must use a revision-pinned profile source.

Classic and Expressive profiles are independent. A seeded suite contains no
assertions and uses `full-model` coverage. An evidence-backed suite contains
located evidence in every assertion of every case. A `vertical-slice` is a
separate catalog entry and cannot partially promote the full-model seed.
