#include "qtmaterial/theme/qtmaterialiconsizetokens.h"

namespace QtMaterial {

IconSizeTokens::IconSizeTokens()
{
    setSize(IconSizeRole::ExtraSmall, 16);
    setSize(IconSizeRole::Small, 18);
    setSize(IconSizeRole::Medium, 24);
    setSize(IconSizeRole::Large, 32);
    setSize(IconSizeRole::ExtraLarge, 40);
}

IconSizeTokens::~IconSizeTokens() = default;

bool IconSizeTokens::contains(IconSizeRole role) const
{
    return m_sizes.contains(role);
}

int IconSizeTokens::size(IconSizeRole role) const
{
    return m_sizes.value(role, 0);
}

void IconSizeTokens::setSize(IconSizeRole role, int logicalPx)
{
    m_sizes.insert(role, logicalPx);
}

void IconSizeTokens::clear()
{
    m_sizes.clear();
}

} // namespace QtMaterial
