#include "qtmaterial/specs/qtmaterialsurfacespecresolver.h"

#include <QStringList>

#include "qtmaterialcomponenttokenapplier_p.h"

namespace QtMaterial {

BannerSpec SurfaceSpecResolver::bannerSpec(const Theme& theme) const
{
    BannerSpec spec;
    spec.containerColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerLow);
    spec.headlineColor =
        theme.colorScheme().color(ColorRole::OnSurface);
    spec.supportingColor =
        theme.colorScheme().color(ColorRole::OnSurfaceVariant);
    spec.actionColor =
        theme.colorScheme().color(ColorRole::Primary);
    return spec;
}

CardSpec SurfaceSpecResolver::cardSpec(const Theme& theme) const
{
    CardSpec spec;
    spec.containerColor =
        theme.colorScheme().color(ColorRole::SurfaceContainerLow);
    spec.outlineColor =
        theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.contentColor =
        theme.colorScheme().color(ColorRole::OnSurface);

    applyCardComponentTokens(
        theme,
        QStringList{
            QStringLiteral("surface"),
            QStringLiteral("card"),
            QStringLiteral("Card")},
        &spec);

    return spec;
}

SnackbarSpec SurfaceSpecResolver::snackbarSpec(const Theme& theme) const
{
    SnackbarSpec spec;
    spec.containerColor =
        theme.colorScheme().color(ColorRole::InverseSurface);
    spec.textColor =
        theme.colorScheme().color(ColorRole::InverseOnSurface);
    spec.actionColor =
        theme.colorScheme().color(ColorRole::InversePrimary);
    spec.dismissIconColor =
        theme.colorScheme().color(ColorRole::InverseOnSurface);
    spec.shapeRole = ShapeRole::Small;
    spec.elevationRole = ElevationRole::Level3;
    spec.enterMotion = MotionToken::Medium2;
    spec.exitMotion = MotionToken::Short4;
    return spec;
}

} // namespace QtMaterial
