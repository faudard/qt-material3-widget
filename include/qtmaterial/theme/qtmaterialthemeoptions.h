#pragma once

#include <QColor>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

class QTMATERIAL3_THEME_EXPORT ThemeOptions
{
public:
    ThemeOptions();
    ~ThemeOptions();

    QColor sourceColor;
    ThemeMode mode;
    ContrastMode contrast;
    bool expressive;
    bool useMaterialColorUtilities;
};

} // namespace QtMaterial
