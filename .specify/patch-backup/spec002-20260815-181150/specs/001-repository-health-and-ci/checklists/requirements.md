# Requirements Checklist: Repository Health and CI

- [x] Current malformed `quality.yml` failure mode is explicitly covered.
- [x] Architecture rules are not duplicated.
- [x] Existing architecture debt is not incorrectly treated as release-001 failure.
- [x] New/stale architecture debt still fails.
- [x] Generated component documentation drift is checked non-destructively.
- [x] Health tool has no additional Python package dependency.
- [x] Documentation remains blocking.
- [x] Strict zero-debt mode is reserved for spec 011.
- [ ] Applied checkout is green locally.
- [ ] GitHub Actions result is green.
