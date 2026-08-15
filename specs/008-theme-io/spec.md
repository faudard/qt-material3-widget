# Feature Specification: ThemeIO

**Feature Branch**: `008-theme-io`
**Status**: Ready for application / convergence
**Milestone**: 0.5
**Created**: 2026-08-15

## Audited state

The architecture baseline currently records 37
`ARCH-THEME-IO-OWNS-TEXT-KEYS` violations. They include:
- ComponentId string conversion in ThemeModel;
- QString component-key overloads in ComponentTokenOverrides;
- textual component alias lists in dedicated spec resolvers;
- QJson/ThemeSerializer persistence APIs in ThemeManager.

ThemeSerializer itself already owns the large enum/key maps and strict v2 schema validation.

## Requirements

- **TIO-FR-001** Component override lookup in ThemeModel/Specs is typed.
- **TIO-FR-002** String↔ComponentId conversion exists only in ThemeIO.
- **TIO-FR-003** Legacy serialized aliases are accepted by ThemeIO and map to typed IDs.
- **TIO-FR-004** Serialization emits canonical component names.
- **TIO-FR-005** Unknown extension component names can round-trip opaquely.
- **TIO-FR-006** ComponentTokenOverrides exposes no QString-key lookup/mutation API.
- **TIO-FR-007** Spec resolvers use QVector<ComponentId>, never QStringList aliases.
- **TIO-FR-008** The component token applier consumes typed IDs.
- **TIO-FR-009** ThemeManager exposes no QJson/ThemeSerializer import/export API.
- **TIO-FR-010** Persistence is composed explicitly through ThemeSerializer + ThemeManager::setTheme.
- **TIO-FR-011** ThemeIO target owns ThemeTextCodec.
- **TIO-FR-012** The `ARCH-THEME-IO-OWNS-TEXT-KEYS` baseline is removed entirely.
- **TIO-FR-013** Any residual violation is a hard CI error.
- **TIO-FR-014** Migration fails closed on unknown resolver aliases.
- **TIO-FR-015** Qt5.14.2/Qt6 and shared/static gates remain mandatory.

## Non-goals

Full universal TokenId system is Spec 010. DTCG/schema-v3 work is later.
