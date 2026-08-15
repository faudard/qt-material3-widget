# Checklist

- [x] TokenId is Foundation and Qt-free.
- [x] TokenId encoding is explicit/versioned.
- [x] Category values are pinned.
- [x] Domain enum order is not persistence identity.
- [x] ComponentId moved to Foundation.
- [x] ComponentId and TokenId are not conflated.
- [x] ThemeModel mapping contains no token strings.
- [x] ThemeIO owns token names.
- [x] Serializer migration is fail-closed.
- [x] No QVariant(TokenId) universal lookup API.
- [x] Representative raw IDs are compile-time pinned.
- [ ] Existing JSON fixtures are byte/semantic compatible as intended.
- [ ] Qt5/Qt6 shared/static gates are green.
