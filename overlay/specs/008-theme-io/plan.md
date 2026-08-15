# Plan

1. Make ComponentTokenOverrides first-party lookup typed.
2. Add ThemeTextCodec to ThemeIO.
3. Preserve unknown serialized extension names behind a private opaque map.
4. Migrate resolver alias lists mechanically with a fail-closed tool.
5. Change component token applier to QVector<ComponentId>.
6. Patch ThemeSerializer to translate canonical/legacy names through ThemeTextCodec.
7. Remove ThemeManager persistence methods and serializer/QJson includes.
8. Delete the entire ThemeIO textual-key architecture baseline slice.
9. Build/test; any residual architecture violation is treated as unfinished migration.

Spec 010 later generalizes stable typed IDs beyond component override slots.
