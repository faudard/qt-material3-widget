#pragma once
#include <QFont>
#include <QHash>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
struct TypographyStyle { QFont font; qreal lineHeight = 0.0; qreal letterSpacing = 0.0; };
class QTMATERIAL3_THEME_EXPORT TypographyScale {
public:
    TypographyScale();
    ~TypographyScale();
    bool contains(TypeRole role) const;
    TypographyStyle style(TypeRole role) const;
    void setStyle(TypeRole role, const TypographyStyle& style);
private:
    QHash<TypeRole, TypographyStyle> m_styles;

public:
// Compatibility aliases/roles for Material 3 component specs.
    // Added by fix-patch12-13-colorscheme-typography-compat-v2.sh.
    QFont displayLarge;
    QFont displayMedium;
    QFont displaySmall;

    QFont headlineLarge;
    QFont headlineMedium;
    QFont headlineSmall;

    QFont titleLarge;
    QFont titleMedium;
    QFont titleSmall;

    QFont bodyLarge;
    QFont bodyMedium;
    QFont bodySmall;

    QFont labelLarge;
    QFont labelMedium;
    QFont labelSmall;
};
} // namespace QtMaterial
