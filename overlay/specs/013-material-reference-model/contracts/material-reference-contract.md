# Material Reference Contract v1

A Material conformance case is identified by:

```text
profile version
+ component/variant
+ interaction state
+ theme mode
```

Evidence assertions additionally identify:
- comparison domain;
- MUST_MATCH / PLATFORM_ADAPTATION / QTMATERIAL_EXTENSION;
- semantic path;
- comparison operator;
- expected value;
- tolerance when relevant;
- one or more source references.

Classic and Expressive are independent profiles.

A seeded suite defines case identity only.
An evidence-backed suite is eligible for structural conformance comparison.
