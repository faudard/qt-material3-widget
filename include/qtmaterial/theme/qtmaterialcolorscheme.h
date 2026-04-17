#pragma once

#include <QColor>
#include <QHash>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

class QTMATERIAL3_THEME_EXPORT ColorScheme
{
public:
    ColorScheme();
    ~ColorScheme();

    bool contains(ColorRole role) const;
    QColor color(ColorRole role) const;
    void setColor(ColorRole role, const QColor& color);
    void clear();

private:
    QHash<ColorRole, QColor> m_colors;
};

} // namespace QtMaterial
