#include "qtmaterial/theme/qtmaterialtypographyscale.h"

namespace QtMaterial {
namespace { const TypographyStyle kDefaultTypographyStyle{}; }

TypographyScale::TypographyScale() = default;
TypographyScale::~TypographyScale() = default;

bool TypographyScale::contains(TypeRole role) const { return m_styles.contains(role); }
const TypographyStyle& TypographyScale::style(TypeRole role) const {
    auto it = m_styles.constFind(role);
    return (it != m_styles.constEnd()) ? it.value() : kDefaultTypographyStyle;
}
void TypographyScale::setStyle(TypeRole role, const TypographyStyle& style) { m_styles.insert(role, style); }

} // namespace QtMaterial
