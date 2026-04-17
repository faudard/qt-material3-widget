#include "qtmaterial/theme/qtmaterialshapescale.h"
namespace QtMaterial {
ShapeScale::ShapeScale() = default;
ShapeScale::~ShapeScale() = default;
bool ShapeScale::contains(ShapeRole role) const { return m_radii.contains(role); }
int ShapeScale::radius(ShapeRole role) const { return m_radii.value(role, 0); }
void ShapeScale::setRadius(ShapeRole role, int px) { m_radii.insert(role, px); }
} // namespace QtMaterial
