# Requirements Checklist: Theme Target Decomposition

- [x] Physical libraries, not documentation-only layers.
- [x] Specs gets ThemeModel only.
- [x] IO/runtime headers forbidden from Specs.
- [x] Shared-library export ownership is separated.
- [x] Legacy Theme CMake consumer target remains available.
- [x] Install/package components are covered.
- [x] MCU backend stays with ThemeModel.
- [x] Runtime->IO debt is explicit, not hidden.
- [x] Deeper ThemeModel/IO/Runtime redesign is deferred to 007-009.
- [ ] Actual shared build is green.
- [ ] Actual static build is green.
- [ ] Qt5.14.2 build is green.
- [ ] Installed consumers are green.
