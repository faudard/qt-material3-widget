#include "qtmaterial/theme/qtmaterialcomponenttokens.h"

#include <algorithm>

namespace QtMaterial {

bool ComponentTokenOverride::isEmpty() const
{
    return colors.isEmpty()
        && typography.isEmpty()
        && shapes.isEmpty()
        && elevations.isEmpty()
        && motion.isEmpty()
        && density.isEmpty()
        && iconSizes.isEmpty()
        && !hasStateLayer
        && custom.isEmpty();
}

ComponentTokenOverrides::ComponentTokenOverrides() = default;
ComponentTokenOverrides::~ComponentTokenOverrides() = default;

bool ComponentTokenOverrides::contains(const QString& componentName) const
{
    return m_overrides.contains(componentName);
}

ComponentTokenOverride ComponentTokenOverrides::overrideFor(const QString& componentName) const
{
    return m_overrides.value(componentName, ComponentTokenOverride{});
}

void ComponentTokenOverrides::setOverride(const QString& componentName, const ComponentTokenOverride& overrideTokens)
{
    if (componentName.trimmed().isEmpty() || overrideTokens.isEmpty()) {
        return;
    }
    m_overrides.insert(componentName, overrideTokens);
}

void ComponentTokenOverrides::removeOverride(const QString& componentName)
{
    m_overrides.remove(componentName);
}

void ComponentTokenOverrides::clear()
{
    m_overrides.clear();
}

QStringList ComponentTokenOverrides::componentNames() const
{
    QStringList names = m_overrides.keys();
    std::sort(names.begin(), names.end());
    return names;
}

} // namespace QtMaterial
