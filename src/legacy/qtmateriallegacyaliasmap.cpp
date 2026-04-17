#include "qtmaterial/theme/qtmateriallegacyaliasmap.h"
namespace QtMaterial {
std::optional<ColorRole> LegacyAliasMap::colorRoleForKey(const QString& key)
{
    if (key == QStringLiteral("primary1")) return ColorRole::Primary;
    if (key == QStringLiteral("primary2")) return ColorRole::PrimaryContainer;
    if (key == QStringLiteral("primary3")) return ColorRole::OnSurface;
    if (key == QStringLiteral("accent1")) return ColorRole::Secondary;
    if (key == QStringLiteral("accent2")) return ColorRole::SecondaryContainer;
    if (key == QStringLiteral("accent3")) return ColorRole::Outline;
    if (key == QStringLiteral("text")) return ColorRole::OnSurface;
    if (key == QStringLiteral("alternateText")) return ColorRole::OnPrimary;
    if (key == QStringLiteral("canvas")) return ColorRole::Surface;
    if (key == QStringLiteral("border")) return ColorRole::OutlineVariant;
    if (key == QStringLiteral("disabled")) return ColorRole::OnSurface;
    if (key == QStringLiteral("disabled2")) return ColorRole::OnSurfaceVariant;
    if (key == QStringLiteral("disabled3")) return ColorRole::SurfaceContainerHigh;
    return std::nullopt;
}
QString LegacyAliasMap::legacyKeyForColorRole(ColorRole role)
{
    switch (role) {
    case ColorRole::Primary: return QStringLiteral("primary1");
    case ColorRole::PrimaryContainer: return QStringLiteral("primary2");
    case ColorRole::Secondary: return QStringLiteral("accent1");
    case ColorRole::SecondaryContainer: return QStringLiteral("accent2");
    case ColorRole::Outline: return QStringLiteral("accent3");
    case ColorRole::OnSurface: return QStringLiteral("text");
    case ColorRole::OnPrimary: return QStringLiteral("alternateText");
    case ColorRole::Surface: return QStringLiteral("canvas");
    case ColorRole::OutlineVariant: return QStringLiteral("border");
    default: return QString();
    }
}
} // namespace QtMaterial
