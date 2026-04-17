#include "qtmaterial/theme/qtmaterialtypographyscale.h"
namespace QtMaterial {
TypographyScale::TypographyScale() = default;
TypographyScale::~TypographyScale() = default;
bool TypographyScale::contains(TypeRole role) const { return m_styles.contains(role); }
TypographyStyle TypographyScale::style(TypeRole role) const { return m_styles.value(role, TypographyStyle{}); }
void TypographyScale::setStyle(TypeRole role, const TypographyStyle& style) { m_styles.insert(role, style); }
} // namespace QtMaterial
