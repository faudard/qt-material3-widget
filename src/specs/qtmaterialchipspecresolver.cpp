#include "qtmaterial/specs/qtmaterialchipspecresolver.h"
#include "qtmaterialcomponenttokenapplier_p.h"

#include "private/qtmaterialdensitymetrics_p.h"

#include <QtGlobal>

namespace QtMaterial {

ChipSpec ChipSpecResolver::resolve(
    ChipVariant variant,
    const Theme& theme,
    Density density) const
{
    ChipSpec spec;
    spec.variant = variant;
    spec.containerColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerLow);
    spec.selectedContainerColor =
        theme.colorScheme().color(ColorRole::SecondaryContainer);
    spec.labelColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.selectedLabelColor =
        theme.colorScheme().color(ColorRole::OnSecondaryContainer);
    spec.iconColor = theme.colorScheme().color(ColorRole::Primary);
    spec.selectedIconColor = spec.selectedLabelColor;
    spec.outlineColor = theme.colorScheme().color(ColorRole::Outline);
    spec.disabledContainerColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.disabledLabelColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledOutlineColor =
        theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnSurface);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    spec.containerHeight =
        SpecsPrivate::adjustedMetric(32, density, -4, 4);
    spec.touchTarget = QSize(48, qMax(48, spec.containerHeight));

    switch (variant) {
    case ChipVariant::Assist:
        applyChipComponentTokens(
            theme,
            QStringList{
                QStringLiteral("chip"),
                QStringLiteral("chip.assist"),
                QStringLiteral("AssistChip")},
            &spec);
        break;
    case ChipVariant::Filter:
        spec.containerColor = Qt::transparent;
        spec.selectedContainerColor =
            theme.colorScheme().color(ColorRole::SecondaryContainer);
        spec.labelColor =
            theme.colorScheme().color(ColorRole::OnSurfaceVariant);
        spec.selectedLabelColor =
            theme.colorScheme().color(ColorRole::OnSecondaryContainer);
        spec.stateLayerColor = spec.labelColor;
        applyChipComponentTokens(
            theme,
            QStringList{
                QStringLiteral("chip"),
                QStringLiteral("chip.filter"),
                QStringLiteral("FilterChip")},
            &spec);
        break;
    case ChipVariant::Input:
        spec.containerColor = Qt::transparent;
        spec.selectedContainerColor =
            theme.colorScheme().color(ColorRole::SurfaceContainerHighest);
        spec.iconColor =
            theme.colorScheme().color(ColorRole::OnSurfaceVariant);
        applyChipComponentTokens(
            theme,
            QStringList{
                QStringLiteral("chip"),
                QStringLiteral("chip.input"),
                QStringLiteral("InputChip")},
            &spec);
        break;
    case ChipVariant::Suggestion:
        spec.containerColor = Qt::transparent;
        spec.selectedContainerColor =
            theme.colorScheme().color(ColorRole::PrimaryContainer);
        spec.selectedLabelColor =
            theme.colorScheme().color(ColorRole::OnPrimaryContainer);
        spec.selectedIconColor = spec.selectedLabelColor;
        applyChipComponentTokens(
            theme,
            QStringList{
                QStringLiteral("chip"),
                QStringLiteral("chip.suggestion"),
                QStringLiteral("SuggestionChip")},
            &spec);
        break;
    }

    return spec;
}

ChipSpec ChipSpecResolver::assistChipSpec(
    const Theme& theme,
    Density density) const
{
    return resolve(ChipVariant::Assist, theme, density);
}

ChipSpec ChipSpecResolver::filterChipSpec(
    const Theme& theme,
    Density density) const
{
    return resolve(ChipVariant::Filter, theme, density);
}

ChipSpec ChipSpecResolver::inputChipSpec(
    const Theme& theme,
    Density density) const
{
    return resolve(ChipVariant::Input, theme, density);
}

ChipSpec ChipSpecResolver::suggestionChipSpec(
    const Theme& theme,
    Density density) const
{
    return resolve(ChipVariant::Suggestion, theme, density);
}

} // namespace QtMaterial
