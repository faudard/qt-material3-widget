#pragma once

#include <QString>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialthemeoptions.h"

namespace QtMaterial {

enum class ThemeColorBackend {
    Fallback,
    MaterialColorUtilities
};

struct QTMATERIAL3_THEME_EXPORT ThemeColorBackendStatus {
    ThemeColorBackend requestedBackend = ThemeColorBackend::Fallback;
    ThemeColorBackend effectiveBackend = ThemeColorBackend::Fallback;
    bool materialColorUtilitiesRequested = false;
    bool materialColorUtilitiesCompiledIn = false;
    bool fallbackUsed = true;
    QString diagnostic;
};

QTMATERIAL3_THEME_EXPORT QString themeColorBackendToString(ThemeColorBackend backend);
QTMATERIAL3_THEME_EXPORT ThemeColorBackend compiledThemeColorBackend() noexcept;
QTMATERIAL3_THEME_EXPORT bool isMaterialColorUtilitiesCompiledIn() noexcept;
QTMATERIAL3_THEME_EXPORT double contrastModeToMaterialColorUtilitiesLevel(ContrastMode contrast) noexcept;
QTMATERIAL3_THEME_EXPORT ThemeColorBackendStatus resolveThemeColorBackend(const ThemeOptions& options);

} // namespace QtMaterial
