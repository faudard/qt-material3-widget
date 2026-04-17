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
};
} // namespace QtMaterial
