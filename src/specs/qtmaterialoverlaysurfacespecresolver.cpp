#include "qtmaterial/specs/qtmaterialoverlaysurfacespecresolver.h"

#include <QtGlobal>

namespace QtMaterial {
namespace {

QColor resolvedScrim(
    const Theme& theme,
    qreal opacity)
{
    QColor color =
        theme.colorScheme().color(ColorRole::Scrim);
    color.setAlphaF(
        qBound<qreal>(
            0.0,
            color.alphaF() * opacity,
            1.0));
    return color;
}

template <typename SpecT>
void resolveRuntimeSurfaceTokens(
    const Theme& theme,
    SpecT* spec)
{
    if (!spec) {
        return;
    }

    spec->shadowColor =
        theme.colorScheme().color(ColorRole::Shadow);

    if (spec->shapeRole == ShapeRole::Full) {
        spec->cornerRadius = -1.0;
    } else if (theme.shapes().contains(spec->shapeRole)) {
        spec->cornerRadius =
            qMax<qreal>(
                0.0,
                theme.shapes().radius(spec->shapeRole));
    }

    spec->hasResolvedElevationStyle = false;
    if (theme.elevations().contains(spec->elevationRole)) {
        spec->elevationStyle =
            theme.elevations().style(spec->elevationRole);
        spec->hasResolvedElevationStyle = true;
    }

    spec->hasResolvedMotionStyle = false;
    if (theme.motion().contains(spec->motionToken)) {
        spec->motionStyle =
            theme.motion().style(spec->motionToken);
        spec->hasResolvedMotionStyle = true;
    }
}

} // namespace

NavigationDrawerSpec
OverlaySurfaceSpecResolver::navigationDrawerSpec(
    const Theme& theme) const
{
    NavigationDrawerSpec spec;
    spec.containerColor =
        theme.colorScheme().color(
            ColorRole::SurfaceContainerLow);
    spec.contentColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.scrimColor =
        resolvedScrim(theme, spec.scrimOpacity);

    resolveRuntimeSurfaceTokens(theme, &spec);
    return spec;
}

BottomSheetSpec
OverlaySurfaceSpecResolver::bottomSheetSpec(
    const Theme& theme) const
{
    BottomSheetSpec spec;
    spec.containerColor =
        theme.colorScheme().color(
            ColorRole::SurfaceContainerHigh);
    spec.contentColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.scrimColor =
        resolvedScrim(theme, spec.scrimOpacity);

    resolveRuntimeSurfaceTokens(theme, &spec);
    return spec;
}

} // namespace QtMaterial
