# Requirements Checklist: ThemeModel

- [x] Identity is structural, not serialized.
- [x] All Theme subdomains are included.
- [x] Component overrides participate in identity.
- [x] Hash iteration order cannot change equality.
- [x] ThemeContext no longer needs serializer/JSON for equality.
- [x] ThemeModel forbids QObject/QWidget/runtime/IO.
- [x] Copy/move semantics are explicit.
- [x] No premature second snapshot type.
- [x] No unversioned external fingerprint format.
- [ ] Actual C++ equality implementation compiles on Qt5.14.2.
- [ ] ThemeContext regressions are green.
- [ ] Shared/static consumers remain green.
