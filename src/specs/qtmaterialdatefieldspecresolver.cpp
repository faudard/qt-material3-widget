#include "qtmaterial/specs/qtmaterialdatefieldspecresolver.h"

#include <QStringList>

#include "qtmaterialcomponenttokenapplier_p.h"

namespace QtMaterial {

DateFieldSpec DateFieldSpecResolver::dateFieldSpec(
    const Theme& theme) const
{
    DateFieldSpec spec;
    spec.leadingIconColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.trailingIconColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.clearIconColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.placeholderColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.disabledIconColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.disabledIconColor.setAlphaF(0.38);

    applyDateFieldComponentTokens(
        theme,
        QStringList{
            QStringLiteral("input"),
            QStringLiteral("textField"),
            QStringLiteral("dateField"),
            QStringLiteral("DateField")},
        &spec);

    return spec;
}

} // namespace QtMaterial
