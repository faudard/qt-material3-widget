#include "qtmaterial/specs/qtmaterialselectionspecresolver.h"

#include <QVector>

#include "qtmaterialcomponenttokenapplier_p.h"

namespace QtMaterial {

namespace {

template <typename SpecT>
void resolveSelectionRuntimeValues(const Theme& theme, SpecT* spec)
{
    if (!spec) {
        return;
    }

    const StateLayer& stateLayer = theme.stateLayer();
    spec->hoverStateLayerOpacity = stateLayer.hoverOpacity;
    spec->focusStateLayerOpacity = stateLayer.focusOpacity;
    spec->pressStateLayerOpacity = stateLayer.pressOpacity;
    spec->dragStateLayerOpacity = stateLayer.dragOpacity;

    spec->hasResolvedLabelFont = false;
    if (theme.typography().contains(spec->labelTypeRole)) {
        spec->labelFont = theme.typography().style(spec->labelTypeRole).font;
        spec->hasResolvedLabelFont = true;
    }

    spec->hasResolvedMotionStyle = false;
    if (theme.motion().contains(spec->motionToken)) {
        spec->motionStyle = theme.motion().style(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    }
}

} // namespace


int SelectionSpecResolver::spacingForDensity(
    Density density,
    int defaultSpacing) noexcept
{
    switch (density) {
    case Density::Compact:
        return 8;
    case Density::Comfortable:
        return 14;
    case Density::Default:
    default:
        return defaultSpacing;
    }
}

CheckboxSpec SelectionSpecResolver::checkboxSpec(
    const Theme& theme,
    Density density) const
{
    CheckboxSpec spec;
    spec.selectedContainerColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.selectedIconColor =
        theme.colorScheme().color(ColorRole::OnPrimary);
    spec.unselectedOutlineColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledSelectedContainerColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.disabledUnselectedOutlineColor =
        theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.labelColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.disabledLabelColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.focusRingColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.spacing = spacingForDensity(density, spec.spacing);

    applyCheckboxComponentTokens(
        theme,
        QVector<ComponentId>{ ComponentId::Checkbox },
        &spec);
    resolveSelectionRuntimeValues(theme, &spec);

    return spec;
}

RadioButtonSpec SelectionSpecResolver::radioButtonSpec(
    const Theme& theme,
    Density density) const
{
    RadioButtonSpec spec;
    spec.selectedColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.unselectedOutlineColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledColor =
        theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.labelColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.disabledLabelColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.stateLayerColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.focusRingColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.spacing = spacingForDensity(density, spec.spacing);

    applyRadioButtonComponentTokens(
        theme,
        QVector<ComponentId>{ ComponentId::RadioButton },
        &spec);
    resolveSelectionRuntimeValues(theme, &spec);

    return spec;
}

SwitchSpec SelectionSpecResolver::switchSpec(
    const Theme& theme,
    Density density) const
{
    SwitchSpec spec;
    spec.selectedTrackColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.selectedHandleColor =
        theme.colorScheme().color(ColorRole::OnPrimary);
    spec.unselectedTrackColor =
        theme.colorScheme().color(ColorRole::SurfaceVariant);
    spec.unselectedHandleColor =
        theme.colorScheme().color(ColorRole::Outline);
    spec.disabledSelectedTrackColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.disabledSelectedHandleColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.disabledUnselectedTrackColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerHigh);
    spec.disabledUnselectedHandleColor =
        theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.iconColor =
        theme.colorScheme().color(ColorRole::OnPrimary);
    spec.stateLayerColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.focusRingColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.labelColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.disabledLabelColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.spacing = spacingForDensity(density, spec.spacing);

    applySwitchComponentTokens(
        theme,
        QVector<ComponentId>{ ComponentId::Switch },
        &spec);
    resolveSelectionRuntimeValues(theme, &spec);

    return spec;
}

} // namespace QtMaterial
