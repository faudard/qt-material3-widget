#include "qtmaterial/theme/qtmaterialdensitytokens.h"

namespace QtMaterial {

DensityTokens::DensityTokens()
{
    setValue(DensityRole::Compact, -2);
    setValue(DensityRole::Default, 0);
    setValue(DensityRole::Comfortable, 2);
}

DensityTokens::~DensityTokens() = default;

bool DensityTokens::contains(DensityRole role) const
{
    return m_values.contains(role);
}

int DensityTokens::value(DensityRole role) const
{
    return m_values.value(role, 0);
}

void DensityTokens::setValue(DensityRole role, int logicalPxDelta)
{
    m_values.insert(role, logicalPxDelta);
}

void DensityTokens::clear()
{
    m_values.clear();
}

} // namespace QtMaterial
