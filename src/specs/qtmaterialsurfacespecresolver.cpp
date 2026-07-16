#include "qtmaterial/specs/qtmaterialsurfacespecresolver.h"

#include <QStringList>

#include "qtmaterialcomponenttokenapplier_p.h"

namespace QtMaterial {

BannerSpec SurfaceSpecResolver::bannerSpec(
    const Theme& theme) const
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
    spec.outlineColor =
        theme.colorScheme().color(ColorRole::OutlineVariant);
    spec.focusRingColor =
        theme.colorScheme().color(ColorRole::Primary);

    if (theme.typography().contains(spec.headlineTypeRole)) {
        spec.headlineFont =
            theme.typography().style(spec.headlineTypeRole).font;
        spec.hasResolvedHeadlineFont = true;
    }
    if (theme.typography().contains(spec.supportingTypeRole)) {
        spec.supportingFont =
            theme.typography().style(spec.supportingTypeRole).font;
        spec.hasResolvedSupportingFont = true;
    }
    if (theme.typography().contains(spec.actionTypeRole)) {
        spec.actionFont =
            theme.typography().style(spec.actionTypeRole).font;
        spec.hasResolvedActionFont = true;
    }
    if (theme.shapes().contains(spec.shapeRole)) {
        spec.cornerRadius =
            qMax<qreal>(0.0, theme.shapes().radius(spec.shapeRole));
    }

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

SnackbarSpec SurfaceSpecResolver::snackbarSpec(
    const Theme& theme) const
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
    spec.shadowColor =
        theme.colorScheme().color(ColorRole::Shadow);
    spec.shapeRole = ShapeRole::Small;
    spec.elevationRole = ElevationRole::Level3;
    spec.enterMotion = MotionToken::Medium2;
    spec.exitMotion = MotionToken::Short4;

    if (theme.typography().contains(spec.textTypeRole)) {
        spec.textFont =
            theme.typography().style(spec.textTypeRole).font;
        spec.hasResolvedTextFont = true;
    }
    if (theme.typography().contains(spec.actionTypeRole)) {
        spec.actionFont =
            theme.typography().style(spec.actionTypeRole).font;
        spec.hasResolvedActionFont = true;
    }
    if (theme.shapes().contains(spec.shapeRole)) {
        spec.cornerRadius =
            qMax<qreal>(0.0, theme.shapes().radius(spec.shapeRole));
    }
    if (theme.elevations().contains(spec.elevationRole)) {
        spec.elevationStyle =
            theme.elevations().style(spec.elevationRole);
        spec.hasResolvedElevationStyle = true;
    }
    if (theme.motion().contains(spec.enterMotion)) {
        spec.enterMotionStyle =
            theme.motion().style(spec.enterMotion);
        spec.hasResolvedEnterMotion = true;
    }
    if (theme.motion().contains(spec.exitMotion)) {
        spec.exitMotionStyle =
            theme.motion().style(spec.exitMotion);
        spec.hasResolvedExitMotion = true;
    }

    return spec;
}

} // namespace QtMaterial
