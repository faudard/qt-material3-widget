#pragma once

#include <QColor>
#include <QtGlobal>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

class QTMATERIAL3_THEME_EXPORT ThemeOptions
{
public:
    /*!
     * ThemeOptions is the canonical input structure used by ThemeBuilder.
     *
     * Current support level:
     * - sourceColor: consumed
     * - mode: consumed
     * - contrast: consumed (minimal post-generation adjustment pass)
     * - expressive: consumed by the Material Color Utilities backend when it is compiled in; the fallback backend may ignore it
     * - useMaterialColorUtilities: persisted but currently not consumed by ThemeBuilder
     */    
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