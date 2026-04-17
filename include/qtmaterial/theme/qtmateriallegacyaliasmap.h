#pragma once
#include <optional>
#include <QString>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
class QTMATERIAL3_LEGACY_EXPORT LegacyAliasMap
{
public:
    static std::optional<ColorRole> colorRoleForKey(const QString& key);
    static QString legacyKeyForColorRole(ColorRole role);
};
} // namespace QtMaterial
