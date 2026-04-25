#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolorscheme.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

class QColor;

namespace QtMaterial {

class QTMATERIAL3_THEME_EXPORT ThemeBuilder {
public:
    ThemeBuilder();
    ~ThemeBuilder();

    Theme build(const ThemeOptions& options) const;

    Theme buildLightFromSeed(const QColor& seed) const;
    Theme buildDarkFromSeed(const QColor& seed) const;

    ColorScheme buildLightSchemeFromSeed(const QColor& seed) const;
    ColorScheme buildDarkSchemeFromSeed(const QColor& seed) const;

private:
    static Theme buildBaseTheme();
    static const Theme& baseTheme();

    Theme buildFallbackTheme(const ThemeOptions& options) const;
    ColorScheme buildFallbackLightScheme(const QColor& seed) const;
    ColorScheme buildFallbackDarkScheme(const QColor& seed) const;

    void applyContrastAdjustments(Theme& theme, ContrastMode contrast) const;
    QColor adjustColorTowards(const QColor& base, const QColor& target, qreal amount) const;

    void applyDefaultTypography(Theme& theme) const;
    void applyDefaultShapes(Theme& theme) const;
    void applyDefaultElevations(Theme& theme) const;
    void applyDefaultMotion(Theme& theme) const;
    void applyDefaultStateLayer(Theme& theme) const;
    void applyDefaultDensity(Theme& theme) const;
    void applyDefaultIconSizes(Theme& theme) const;
};

} // namespace QtMaterial
