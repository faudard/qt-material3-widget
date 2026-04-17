#pragma once

#include <QHash>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

class QTMATERIAL3_THEME_EXPORT ShapeScale
{
public:
    ShapeScale();
    ~ShapeScale();

    bool contains(ShapeRole role) const;
    int radius(ShapeRole role) const;
    void setRadius(ShapeRole role, int px);

private:
    QHash<ShapeRole, int> m_radii;
};

} // namespace QtMaterial
