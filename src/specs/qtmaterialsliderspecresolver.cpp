#include "qtmaterial/specs/qtmaterialsliderspecresolver.h"

namespace QtMaterial {
namespace {

QColor withOpacity(QColor color, qreal opacity)
{
    color.setAlphaF(
        qBound<qreal>(
            0.0,
            color.alphaF() * opacity,
            1.0));
    return color;
}

void applyDensity(SliderSpec* spec, Density density)
{
    if (!spec) {
        return;
    }

    switch (density) {
    case Density::Compact:
        spec->touchTarget = QSize(40, 40);
        spec->handleDiameter = 16;
        spec->stateLayerSize = 32;
        break;
    case Density::Comfortable:
        spec->touchTarget = QSize(44, 44);
        spec->handleDiameter = 18;
        spec->stateLayerSize = 36;
        break;
    case Density::Default:
    default:
        break;
    }
}

} // namespace

SliderSpec SliderSpecResolver::sliderSpec(
    const Theme& theme,
    Density density) const
{
    SliderSpec spec;
    spec.activeTrackColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.inactiveTrackColor =
        theme.colorScheme().color(
            ColorRole::SurfaceContainerHighest);
    spec.handleColor =
        theme.colorScheme().color(ColorRole::Primary);

    const QColor onSurface =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.disabledActiveTrackColor =
        withOpacity(onSurface, 0.38);
    spec.disabledInactiveTrackColor =
        withOpacity(onSurface, 0.12);
    spec.disabledHandleColor =
        withOpacity(onSurface, 0.38);

    spec.stateLayerColor =
        theme.colorScheme().color(ColorRole::Primary);
    spec.focusRingColor =
        theme.colorScheme().color(ColorRole::Primary);

    applyDensity(&spec, density);
    return spec;
}

} // namespace QtMaterial
