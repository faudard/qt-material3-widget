#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"

#include <QtGlobal>
#include <QVector>

#include "qtmaterialcomponenttokenapplier_p.h"

namespace QtMaterial {
namespace {

constexpr qreal kDisabledButtonContainerOpacity = 0.10;
constexpr qreal kFilledTonalDisabledContainerOpacity = 0.12;
constexpr qreal kDisabledButtonContentOpacity = 0.38;
constexpr qreal kSmallOutlinedButtonOutlineWidth = 1.0;

QColor withOpacity(QColor color, qreal opacity)
{
    color.setAlphaF(qBound<qreal>(0.0, opacity, 1.0));
    return color;
}

} // namespace

ButtonSpec ButtonSpecResolver::baseButtonSpec(
    Density density) noexcept
{
    ButtonSpec spec;
    spec.containerColor = Qt::transparent;
    spec.outlineColor = Qt::transparent;
    spec.disabledOutlineColor = Qt::transparent;
    spec.containerHeight = buttonHeightForDensity(density);
    return spec;
}

int ButtonSpecResolver::buttonHeightForDensity(Density density) noexcept
{
    switch (density) {
    case Density::Compact:
        return 36;
    case Density::Comfortable:
        return 44;
    case Density::Default:
    default:
        return 40;
    }
}

ButtonSpec ButtonSpecResolver::resolve(
    ButtonVariant variant,
    const Theme& theme,
    Density density) const
{
    switch (variant) {
    case ButtonVariant::Text:
        return textButtonSpec(theme, density);
    case ButtonVariant::Filled:
        return filledButtonSpec(theme, density);
    case ButtonVariant::FilledTonal:
        return filledTonalButtonSpec(theme, density);
    case ButtonVariant::Outlined:
        return outlinedButtonSpec(theme, density);
    case ButtonVariant::Elevated:
        return elevatedButtonSpec(theme, density);
    }

    return textButtonSpec(theme, density);
}

ButtonSpec ButtonSpecResolver::textButtonSpec(
    const Theme& theme,
    Density density) const
{
    ButtonSpec spec = baseButtonSpec(density);
    spec.containerColor = Qt::transparent;
    spec.labelColor = theme.colorScheme().color(ColorRole::Primary);
    spec.iconColor = spec.labelColor;
    spec.disabledContainerColor = Qt::transparent;
    spec.disabledLabelColor = withOpacity(
        theme.colorScheme().color(ColorRole::OnSurfaceVariant),
        kDisabledButtonContentOpacity);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::Primary);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);

    spec.elevationRole = ElevationRole::Level0;
    spec.hoverElevationRole = ElevationRole::Level0;
    spec.restingElevationProgress = 0.0;
    spec.hoverElevationProgress = 0.0;
    spec.focusElevationProgress = 0.0;
    spec.pressElevationProgress = 0.0;
    spec.disabledElevationProgress = 0.0;

    applyButtonComponentTokens(
        theme,
        QVector<ComponentId>{ ComponentId::Button, ComponentId::ButtonText },
        &spec);
    return spec;
}

ButtonSpec ButtonSpecResolver::filledButtonSpec(
    const Theme& theme,
    Density density) const
{
    ButtonSpec spec = baseButtonSpec(density);
    spec.containerColor = theme.colorScheme().color(ColorRole::Primary);
    spec.labelColor = theme.colorScheme().color(ColorRole::OnPrimary);
    spec.iconColor = spec.labelColor;
    spec.disabledContainerColor = withOpacity(
        theme.colorScheme().color(ColorRole::OnSurface),
        kDisabledButtonContainerOpacity);
    spec.disabledLabelColor = withOpacity(
        theme.colorScheme().color(ColorRole::OnSurfaceVariant),
        kDisabledButtonContentOpacity);
    spec.stateLayerColor = theme.colorScheme().color(ColorRole::OnPrimary);
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);

    spec.elevationRole = ElevationRole::Level0;
    spec.hoverElevationRole = ElevationRole::Level1;
    spec.restingElevationProgress = 0.0;
    spec.hoverElevationProgress = 1.0;
    spec.focusElevationProgress = 0.0;
    spec.pressElevationProgress = 0.0;
    spec.disabledElevationProgress = 0.0;

    applyButtonComponentTokens(
        theme,
        QVector<ComponentId>{ ComponentId::Button, ComponentId::ButtonFilled },
        &spec);
    return spec;
}

ButtonSpec ButtonSpecResolver::filledTonalButtonSpec(
    const Theme& theme,
    Density density) const
{
    ButtonSpec spec = baseButtonSpec(density);
    spec.containerColor =
        theme.colorScheme().color(ColorRole::SecondaryContainer);
    spec.labelColor =
        theme.colorScheme().color(ColorRole::OnSecondaryContainer);
    spec.iconColor = spec.labelColor;
    spec.disabledContainerColor = withOpacity(
        theme.colorScheme().color(ColorRole::OnSurface),
        kFilledTonalDisabledContainerOpacity);
    spec.disabledLabelColor = withOpacity(
        theme.colorScheme().color(ColorRole::OnSurface),
        kDisabledButtonContentOpacity);
    spec.stateLayerColor = spec.labelColor;
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Secondary);

    spec.elevationRole = ElevationRole::Level0;
    spec.hoverElevationRole = ElevationRole::Level1;
    spec.restingElevationProgress = 0.0;
    spec.hoverElevationProgress = 1.0;
    spec.focusElevationProgress = 0.0;
    spec.pressElevationProgress = 0.0;
    spec.disabledElevationProgress = 0.0;

    applyButtonComponentTokens(
        theme,
        QVector<ComponentId>{ ComponentId::Button, ComponentId::ButtonFilledTonal },
        &spec);
    return spec;
}

ButtonSpec ButtonSpecResolver::outlinedButtonSpec(
    const Theme& theme,
    Density density) const
{
    ButtonSpec spec = baseButtonSpec(density);
    spec.containerColor = Qt::transparent;
    spec.labelColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.iconColor = spec.labelColor;
    spec.disabledContainerColor = Qt::transparent;
    spec.disabledLabelColor = withOpacity(
        theme.colorScheme().color(ColorRole::OnSurfaceVariant),
        kDisabledButtonContentOpacity);
    spec.stateLayerColor = spec.labelColor;
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    spec.outlineColor =
        theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.disabledOutlineColor = withOpacity(
        theme.colorScheme().color(ColorRole::OutlineVariant),
        kDisabledButtonContainerOpacity);
    spec.outlineWidth = kSmallOutlinedButtonOutlineWidth;

    spec.elevationRole = ElevationRole::Level0;
    spec.hoverElevationRole = ElevationRole::Level0;
    spec.restingElevationProgress = 0.0;
    spec.hoverElevationProgress = 0.0;
    spec.focusElevationProgress = 0.0;
    spec.pressElevationProgress = 0.0;
    spec.disabledElevationProgress = 0.0;

    applyButtonComponentTokens(
        theme,
        QVector<ComponentId>{ ComponentId::Button, ComponentId::ButtonOutlined },
        &spec);
    return spec;
}

ButtonSpec ButtonSpecResolver::elevatedButtonSpec(
    const Theme& theme,
    Density density) const
{
    ButtonSpec spec = baseButtonSpec(density);
    spec.containerColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerLow);
    spec.labelColor = theme.colorScheme().color(ColorRole::Primary);
    spec.iconColor = spec.labelColor;
    spec.disabledContainerColor = withOpacity(
        theme.colorScheme().color(ColorRole::OnSurface),
        kDisabledButtonContainerOpacity);
    spec.disabledLabelColor = withOpacity(
        theme.colorScheme().color(ColorRole::OnSurfaceVariant),
        kDisabledButtonContentOpacity);
    spec.stateLayerColor = spec.labelColor;
    spec.focusRingColor = theme.colorScheme().color(ColorRole::Primary);
    spec.elevationRole = ElevationRole::Level1;
    spec.hoverElevationRole = ElevationRole::Level2;
    spec.restingElevationProgress = 0.0;
    spec.hoverElevationProgress = 1.0;
    spec.focusElevationProgress = 0.0;
    spec.pressElevationProgress = 0.0;
    spec.disabledElevationProgress = 0.0;

    applyButtonComponentTokens(
        theme,
        QVector<ComponentId>{ ComponentId::Button, ComponentId::ButtonElevated },
        &spec);
    return spec;
}

} // namespace QtMaterial
