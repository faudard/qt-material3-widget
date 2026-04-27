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

public:
// Compatibility aliases/roles for Material 3 component specs.
    // Added by fix-patch12-13-colorscheme-typography-compat-v2.sh.
    QColor primary = QColor("#6750A4");
    QColor onPrimary = QColor("#FFFFFF");
    QColor primaryContainer = QColor("#EADDFF");
    QColor onPrimaryContainer = QColor("#21005D");

    QColor secondary = QColor("#625B71");
    QColor onSecondary = QColor("#FFFFFF");
    QColor secondaryContainer = QColor("#E8DEF8");
    QColor onSecondaryContainer = QColor("#1D192B");

    QColor tertiary = QColor("#7D5260");
    QColor onTertiary = QColor("#FFFFFF");
    QColor tertiaryContainer = QColor("#FFD8E4");
    QColor onTertiaryContainer = QColor("#31111D");

    QColor error = QColor("#B3261E");
    QColor onError = QColor("#FFFFFF");
    QColor errorContainer = QColor("#F9DEDC");
    QColor onErrorContainer = QColor("#410E0B");

    QColor background = QColor("#FFFBFE");
    QColor onBackground = QColor("#1C1B1F");

    QColor surface = QColor("#FFFBFE");
    QColor onSurface = QColor("#1C1B1F");
    QColor surfaceVariant = QColor("#E7E0EC");
    QColor onSurfaceVariant = QColor("#49454F");

    QColor surfaceContainerLowest = QColor("#FFFFFF");
    QColor surfaceContainerLow = QColor("#F7F2FA");
    QColor surfaceContainer = QColor("#F3EDF7");
    QColor surfaceContainerHigh = QColor("#ECE6F0");
    QColor surfaceContainerHighest = QColor("#E6E0E9");

    QColor outline = QColor("#79747E");
    QColor outlineVariant = QColor("#CAC4D0");

    QColor inverseSurface = QColor("#313033");
    QColor inverseOnSurface = QColor("#F4EFF4");
    QColor inversePrimary = QColor("#D0BCFF");

    QColor shadow = QColor("#000000");
    QColor scrim = QColor("#000000");
};

} // namespace QtMaterial
