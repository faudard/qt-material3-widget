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

    bool operator==(const ThemeOptions& other) const noexcept
    {
        return sourceColor == other.sourceColor
               && mode == other.mode
               && contrast == other.contrast
               && expressive == other.expressive
               && useMaterialColorUtilities == other.useMaterialColorUtilities;
    }

    bool operator!=(const ThemeOptions& other) const noexcept
    {
        return !(*this == other);
    }
};

} // namespace QtMaterial