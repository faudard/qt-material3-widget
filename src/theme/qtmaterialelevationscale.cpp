#include "qtmaterial/theme/qtmaterialelevationscale.h"
namespace QtMaterial {
ElevationScale::ElevationScale() = default;
ElevationScale::~ElevationScale() = default;
bool ElevationScale::contains(ElevationRole role) const { return m_styles.contains(role); }
ElevationStyle ElevationScale::style(ElevationRole role) const { return m_styles.value(role, ElevationStyle{}); }
void ElevationScale::setStyle(ElevationRole role, const ElevationStyle& style) { m_styles.insert(role, style); }
} // namespace QtMaterial
