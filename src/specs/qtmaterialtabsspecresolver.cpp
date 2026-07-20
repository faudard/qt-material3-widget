#include "qtmaterial/specs/qtmaterialtabsspecresolver.h"
#include "qtmaterialcomponenttokenapplier_p.h"

#include <QtGlobal>

namespace QtMaterial {
namespace {

QColor roleOrDefault(
    const ColorScheme& scheme,
    ColorRole role,
    const QColor& fallback)
{
    return scheme.contains(role) ? scheme.color(role) : fallback;
}

QColor withOpacity(QColor color, qreal opacity)
{
    color.setAlphaF(qBound<qreal>(0.0, opacity, 1.0));
    return color;
}

void resolveColor(
    QColor* target,
    const ColorScheme& scheme,
    ColorRole role,
    const QColor& fallback)
{
    if (target && !target->isValid()) {
        *target = roleOrDefault(scheme, role, fallback);
    }
}

} // namespace

TabsSpec TabsSpecResolver::resolve(
    const Theme& theme,
    const TabsSpec& authored) const
{
    TabsSpec resolved = authored;
    if (!resolved.useGlobalTheme) {
        return resolved;
    }

    const ColorScheme& scheme = theme.colorScheme();
    const QColor primary = roleOrDefault(
        scheme,
        ColorRole::Primary,
        QColor(QStringLiteral("#6750A4")));
    const QColor surface = roleOrDefault(
        scheme,
        ColorRole::Surface,
        QColor(QStringLiteral("#FFFBFE")));
    const QColor onSurface = roleOrDefault(
        scheme,
        ColorRole::OnSurface,
        QColor(QStringLiteral("#1C1B1F")));
    const QColor onSurfaceVariant = roleOrDefault(
        scheme,
        ColorRole::OnSurfaceVariant,
        QColor(QStringLiteral("#49454F")));
    const QColor error = roleOrDefault(
        scheme,
        ColorRole::Error,
        QColor(QStringLiteral("#B3261E")));
    const QColor onError = roleOrDefault(
        scheme,
        ColorRole::OnError,
        QColor(Qt::white));

    resolveColor(
        &resolved.containerColor,
        scheme,
        ColorRole::Surface,
        surface);

    if (!resolved.activeLabelColor.isValid()) {
        resolved.activeLabelColor =
            resolved.variant == TabsVariant::Primary
            ? primary
            : onSurface;
    }

    resolveColor(
        &resolved.inactiveLabelColor,
        scheme,
        ColorRole::OnSurfaceVariant,
        onSurfaceVariant);
    resolveColor(
        &resolved.activeIndicatorColor,
        scheme,
        ColorRole::Primary,
        primary);
    resolveColor(
        &resolved.stateLayerColor,
        scheme,
        ColorRole::Primary,
        primary);

    if (!resolved.hoverStateLayerColor.isValid()) {
        resolved.hoverStateLayerColor = resolved.stateLayerColor;
    }
    if (!resolved.focusedStateLayerColor.isValid()) {
        resolved.focusedStateLayerColor = resolved.stateLayerColor;
    }
    if (!resolved.pressedStateLayerColor.isValid()) {
        resolved.pressedStateLayerColor = resolved.stateLayerColor;
    }

    resolveColor(
        &resolved.focusRingColor,
        scheme,
        ColorRole::Primary,
        primary);

    if (!resolved.disabledLabelColor.isValid()) {
        resolved.disabledLabelColor =
            withOpacity(onSurfaceVariant, resolved.disabledOpacity);
    }

    resolveColor(
        &resolved.badgeColor,
        scheme,
        ColorRole::Error,
        error);
    resolveColor(
        &resolved.badgeLabelColor,
        scheme,
        ColorRole::OnError,
        onError);
    resolveColor(
        &resolved.overflowButtonColor,
        scheme,
        ColorRole::OnSurfaceVariant,
        onSurfaceVariant);

    const StateLayer& stateLayer = theme.stateLayer();
    if (resolved.hoverOpacity < 0.0) {
        resolved.hoverOpacity = stateLayer.hoverOpacity;
    }
    if (resolved.focusOpacity < 0.0) {
        resolved.focusOpacity = stateLayer.focusOpacity;
    }
    if (resolved.pressedOpacity < 0.0) {
        resolved.pressedOpacity = stateLayer.pressOpacity;
    }

    applyTabsComponentTokens(
        theme,
        QStringList{
            QStringLiteral("navigation"),
            QStringLiteral("tabs"),
            QStringLiteral("Tabs")},
        &resolved);

    return resolved;
}

} // namespace QtMaterial
