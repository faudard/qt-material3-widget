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
     * - expressive: persisted but currently not consumed by ThemeBuilder
     * - useMaterialColorUtilities: persisted but currently not consumed by ThemeBuilder
     */    
    ThemeOptions();
    ~ThemeOptions();

    QColor sourceColor;
    ThemeMode mode;
    ContrastMode contrast;
    // Experimental: persisted for forward compatibility, but not currently consumed by ThemeBuilder.
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    Q_DECL_DEPRECATED_X("ThemeBuilder does not yet implement expressive theme generation.")
#endif
    bool expressive;
    // Experimental: persisted for forward compatibility, but not currently consumed by ThemeBuilder.
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    Q_DECL_DEPRECATED_X("ThemeBuilder does not yet implement a Material Color Utilities backend.")
#endif    
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