# Checklist

- [x] ThemeModel has no public textual component-key API.
- [x] Specs migration is typed and fail-closed.
- [x] ThemeIO owns legacy aliases and canonical names.
- [x] Unknown extension names remain round-trippable.
- [x] ThemeManager persistence ownership is explicitly removed by the applicator.
- [x] Baseline policy is zero debt for ARCH-THEME-IO-OWNS-TEXT-KEYS after application.
- [ ] Actual serializer fixtures round-trip.
- [ ] All 37 baseline violations are gone on the real checkout.
- [ ] Qt5/Qt6 shared/static builds are green.
