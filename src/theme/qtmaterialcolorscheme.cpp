#include "qtmaterial/theme/qtmaterialcolorscheme.h"

namespace QtMaterial {

ColorScheme::ColorScheme() = default;
ColorScheme::~ColorScheme() = default;

bool ColorScheme::contains(ColorRole role) const { return m_colors.contains(role); }
QColor ColorScheme::color(ColorRole role) const { return m_colors.value(role, QColor()); }
void ColorScheme::setColor(ColorRole role, const QColor& color) { m_colors.insert(role, color); }
void ColorScheme::clear() { m_colors.clear(); }

} // namespace QtMaterial
