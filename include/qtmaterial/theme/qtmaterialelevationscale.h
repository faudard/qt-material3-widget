#pragma once
#include <QHash>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct ElevationStyle { int shadowBlur = 0; int shadowYOffset = 0; qreal tonalOverlayOpacity = 0.0; };
class QTMATERIAL3_THEME_EXPORT ElevationScale {
public:
    ElevationScale();
    ~ElevationScale();
    bool contains(ElevationRole role) const;
    ElevationStyle style(ElevationRole role) const;
    void setStyle(ElevationRole role, const ElevationStyle& style);
private:
    QHash<ElevationRole, ElevationStyle> m_styles;
};
} // namespace QtMaterial
