#include "qtmaterial/core/qtmaterialproxystyle.h"

#include "qtmaterial/core/qtmaterialpaletteadapter.h"

namespace QtMaterial {

QtMaterialProxyStyle::QtMaterialProxyStyle(QStyle* baseStyle)
    : QProxyStyle(baseStyle)
    , m_palette(QProxyStyle::standardPalette())
{
}

QtMaterialProxyStyle::QtMaterialProxyStyle(const Theme& theme, QStyle* baseStyle)
    : QProxyStyle(baseStyle)
    , m_palette(QtMaterialPaletteAdapter::toPalette(theme, QProxyStyle::standardPalette()))
{
}

QtMaterialProxyStyle::~QtMaterialProxyStyle() = default;

void QtMaterialProxyStyle::setTheme(const Theme& theme)
{
    m_palette = QtMaterialPaletteAdapter::toPalette(theme, QProxyStyle::standardPalette());
}

QPalette QtMaterialProxyStyle::standardPalette() const
{
    return m_palette;
}

} // namespace QtMaterial
