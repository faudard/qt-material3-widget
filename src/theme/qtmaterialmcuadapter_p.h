#pragma once

#include <QColor>
#include <QString>

#include "qtmaterial/theme/qtmaterialcolorscheme.h"
#include "qtmaterial/theme/qtmaterialthemeoptions.h"

namespace QtMaterial::detail {

struct McuColorSchemeResult {
    bool ok = false;
    ColorScheme colorScheme;
    QString diagnostic;
};

bool mcuAdapterCompiledIn() noexcept;
McuColorSchemeResult generateMcuColorScheme(const ThemeOptions& options);
McuColorSchemeResult generateMcuColorScheme(const QColor& seed,
                                            ThemeMode mode,
                                            ContrastMode contrast,
                                            bool expressive);

} // namespace QtMaterial::detail
