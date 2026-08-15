#include "qtmaterial/specs/qtmaterialdialogspecresolver.h"

#include <QVector>

#include "qtmaterialcomponenttokenapplier_p.h"

namespace QtMaterial {

DialogSpec DialogSpecResolver::dialogSpec(const Theme& theme) const
{
    DialogSpec spec;
    spec.containerColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.headlineColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.bodyColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.scrimColor =
        theme.colorScheme().color(ColorRole::Scrim);
    spec.shadowColor =
        theme.colorScheme().color(ColorRole::Shadow);

    applyDialogComponentTokens(
        theme,
        QVector<ComponentId>{ ComponentId::Surface, ComponentId::Dialog },
        &spec);

    return spec;
}

} // namespace QtMaterial
