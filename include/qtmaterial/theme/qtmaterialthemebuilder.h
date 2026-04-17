#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_THEME_EXPORT ThemeBuilder
{
public:
    ThemeBuilder();
    ~ThemeBuilder();

    Theme build(const ThemeOptions& options) const;
    Theme buildLightFromSeed(const QColor& seed) const;
    Theme buildDarkFromSeed(const QColor& seed) const;

private:
    Theme buildFallbackTheme(const ThemeOptions& options) const;
    void applyDefaultTypography(Theme& theme) const;
    void applyDefaultShapes(Theme& theme) const;
    void applyDefaultElevations(Theme& theme) const;
    void applyDefaultMotion(Theme& theme) const;
    void applyDefaultStateLayer(Theme& theme) const;
    void applyFallbackLightScheme(Theme& theme, const QColor& seed) const;
    void applyFallbackDarkScheme(Theme& theme, const QColor& seed) const;
};

} // namespace QtMaterial
