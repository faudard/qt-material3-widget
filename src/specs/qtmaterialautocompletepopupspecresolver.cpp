#include "qtmaterial/specs/qtmaterialautocompletepopupspecresolver.h"

#include <QStringList>

#include "qtmaterialcomponenttokenapplier_p.h"

namespace QtMaterial {

AutocompletePopupSpec
AutocompletePopupSpecResolver::autocompletePopupSpec(
    const Theme& theme) const
{
    AutocompletePopupSpec spec;
    spec.containerColor =
        theme.colorScheme().color(
            ColorRole::SurfaceContainerHigh);
    spec.textColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.selectedTextColor =
        theme.colorScheme().color(
            ColorRole::OnSecondaryContainer);
    spec.supportingTextColor =
        theme.colorScheme().color(
            ColorRole::OnSurfaceVariant);
    spec.selectedContainerColor =
        theme.colorScheme().color(
            ColorRole::SecondaryContainer);
    spec.stateLayerColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.dividerColor =
        theme.colorScheme().color(
            ColorRole::OutlineVariant);
    spec.focusRingColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.shadowColor =
        theme.colorScheme().color(ColorRole::Shadow);

    applyAutocompletePopupComponentTokens(
        theme,
        QStringList{
            QStringLiteral("input"),
            QStringLiteral("autocomplete"),
            QStringLiteral("autocompletePopup"),
            QStringLiteral("AutocompletePopup")},
        &spec);

    return spec;
}

} // namespace QtMaterial
