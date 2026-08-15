# Spec 001 regression checklist

- [x] `name: Quality` is detected as a top-level workflow key.
- [x] malformed job-only `quality.yml` is rejected.
- [x] empty `jobs:` is rejected.
- [x] command failure propagates.
- [x] registry checking is delegated to the Spec 002 checker when present in the cumulative patch.
