#pragma once

#include <QPalette>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_CORE_EXPORT QtMaterialPaletteAdapter {
public:
    static QPalette toPalette(const Theme& theme, const QPalette& base = QPalette());
    static QColor roleOrFallback(const Theme& theme, ColorRole role, const QColor& fallback);
};

} // namespace QtMaterial
