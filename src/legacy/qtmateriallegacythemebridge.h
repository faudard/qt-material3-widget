#pragma once
#include <QColor>
#include <QString>
#include "qtmaterial/theme/qtmaterialtheme.h"
namespace QtMaterial {
class LegacyThemeBridge
{
public:
    explicit LegacyThemeBridge(const Theme& theme);
    bool hasLegacyKey(const QString& key) const;
    QColor resolveLegacyColor(const QString& key) const;
private:
    QColor applyDisabledAlpha(const QString& key, const QColor& color) const;
    const Theme& m_theme;
};
} // namespace QtMaterial
