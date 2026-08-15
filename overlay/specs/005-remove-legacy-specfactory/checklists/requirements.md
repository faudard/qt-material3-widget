# Requirements Checklist: Remove Legacy SpecFactory

- [x] Current active implementation was audited before defining work.
- [x] Removal is not confused with resolver redesign.
- [x] Public include/install surface is covered.
- [x] Active code/build/tests/examples are covered.
- [x] Current README and roadmap wording are covered.
- [x] Historical references remain possible.
- [x] No pre-1.0 compatibility alias is introduced.
- [x] Local stale legacy files are deleted safely with backup.
- [x] CI reintroduction guard exists.
- [ ] Real checkout is clean under the tombstone checker.
- [ ] Qt5/Qt6 build/test gates remain green.
