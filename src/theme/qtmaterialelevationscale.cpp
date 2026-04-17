#include "qtmaterial/theme/qtmaterialelevationscale.h"

namespace QtMaterial {
namespace { const ElevationStyle kDefaultElevationStyle{}; }

ElevationScale::ElevationScale() = default;
ElevationScale::~ElevationScale() = default;

bool ElevationScale::contains(ElevationRole role) const { return m_styles.contains(role); }
const ElevationStyle& ElevationScale::style(ElevationRole role) const {
    auto it = m_styles.constFind(role);
    return (it != m_styles.constEnd()) ? it.value() : kDefaultElevationStyle;
}
void ElevationScale::setStyle(ElevationRole role, const ElevationStyle& style) { m_styles.insert(role, style); }

} // namespace QtMaterial
