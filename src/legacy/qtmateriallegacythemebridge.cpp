#include "qtmateriallegacythemebridge.h"

#include "qtmaterial/theme/qtmateriallegacyaliasmap.h"

namespace QtMaterial {

LegacyThemeBridge::LegacyThemeBridge(const Theme& theme)
    : m_theme(theme)
{}

bool LegacyThemeBridge::hasLegacyKey(const QString& key) const {
    return LegacyAliasMap::colorRoleForKey(key).has_value();
}

QColor LegacyThemeBridge::resolveLegacyColor(const QString& key) const {
    const auto role = LegacyAliasMap::colorRoleForKey(key);
    if (!role.has_value()) {
        return QColor();
    }
    QColor color = m_theme.colorScheme().color(*role);
    return applyDisabledAlpha(key, color);
}

QColor LegacyThemeBridge::applyDisabledAlpha(const QString& key, const QColor& color) const {
    QColor result = color;
    if (key == QStringLiteral("disabled")) {
        result.setAlphaF(0.38);
    } else if (key == QStringLiteral("disabled2")) {
        result.setAlphaF(0.30);
    } else if (key == QStringLiteral("disabled3")) {
        result.setAlphaF(1.0);
    }
    return result;
}

} // namespace QtMaterial
