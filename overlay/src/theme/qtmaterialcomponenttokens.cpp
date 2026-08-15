#include "qtmaterial/theme/qtmaterialcomponenttokens.h"

#include <algorithm>

namespace QtMaterial {
namespace {

bool equalTypographyStyle(const TypographyStyle& a, const TypographyStyle& b)
{
    return a.font == b.font
        && a.lineHeight == b.lineHeight
        && a.letterSpacing == b.letterSpacing;
}

bool equalElevationStyle(const ElevationStyle& a, const ElevationStyle& b)
{
    return a.shadowBlur == b.shadowBlur
        && a.shadowYOffset == b.shadowYOffset
        && a.tonalOverlayOpacity == b.tonalOverlayOpacity;
}

bool equalMotionStyle(const MotionStyle& a, const MotionStyle& b)
{
    return a.durationMs == b.durationMs && a.easing == b.easing;
}

bool equalStateLayer(const StateLayer& a, const StateLayer& b)
{
    return a.color == b.color
        && a.hoverOpacity == b.hoverOpacity
        && a.focusOpacity == b.focusOpacity
        && a.pressOpacity == b.pressOpacity
        && a.dragOpacity == b.dragOpacity;
}

bool equalOverride(const ComponentTokenOverride& a,
                   const ComponentTokenOverride& b)
{
    if (a.colors != b.colors
        || a.shapes != b.shapes
        || a.density != b.density
        || a.iconSizes != b.iconSizes
        || a.custom != b.custom
        || a.hasStateLayer != b.hasStateLayer) {
        return false;
    }

    if (a.hasStateLayer && !equalStateLayer(a.stateLayer, b.stateLayer)) {
        return false;
    }

    if (a.typography.size() != b.typography.size()
        || a.elevations.size() != b.elevations.size()
        || a.motion.size() != b.motion.size()) {
        return false;
    }

    for (auto it = a.typography.constBegin(); it != a.typography.constEnd(); ++it) {
        if (!b.typography.contains(it.key())
            || !equalTypographyStyle(it.value(), b.typography.value(it.key()))) {
            return false;
        }
    }
    for (auto it = a.elevations.constBegin(); it != a.elevations.constEnd(); ++it) {
        if (!b.elevations.contains(it.key())
            || !equalElevationStyle(it.value(), b.elevations.value(it.key()))) {
            return false;
        }
    }
    for (auto it = a.motion.constBegin(); it != a.motion.constEnd(); ++it) {
        if (!b.motion.contains(it.key())
            || !equalMotionStyle(it.value(), b.motion.value(it.key()))) {
            return false;
        }
    }
    return true;
}

template <typename Key>
bool equalOverrideMap(const QHash<Key, ComponentTokenOverride>& a,
                      const QHash<Key, ComponentTokenOverride>& b)
{
    if (a.size() != b.size()) {
        return false;
    }
    for (auto it = a.constBegin(); it != a.constEnd(); ++it) {
        if (!b.contains(it.key())
            || !equalOverride(it.value(), b.value(it.key()))) {
            return false;
        }
    }
    return true;
}

} // namespace

bool ComponentTokenOverride::isEmpty() const
{
    return colors.isEmpty() && typography.isEmpty() && shapes.isEmpty()
        && elevations.isEmpty() && motion.isEmpty() && density.isEmpty()
        && iconSizes.isEmpty() && !hasStateLayer && custom.isEmpty();
}

ComponentTokenOverrides::ComponentTokenOverrides() = default;
ComponentTokenOverrides::~ComponentTokenOverrides() = default;

bool ComponentTokenOverrides::contains(ComponentId componentId) const
{
    return m_overrides.contains(componentId);
}

ComponentTokenOverride
ComponentTokenOverrides::overrideFor(ComponentId componentId) const
{
    return m_overrides.value(componentId, ComponentTokenOverride{});
}

void ComponentTokenOverrides::setOverride(
    ComponentId componentId,
    const ComponentTokenOverride& overrideTokens)
{
    if (overrideTokens.isEmpty()) {
        m_overrides.remove(componentId);
        return;
    }
    m_overrides.insert(componentId, overrideTokens);
}

void ComponentTokenOverrides::removeOverride(ComponentId componentId)
{
    m_overrides.remove(componentId);
}

void ComponentTokenOverrides::clear()
{
    m_overrides.clear();
    m_extensionOverrides.clear();
}

QList<ComponentId> ComponentTokenOverrides::componentIds() const
{
    QList<ComponentId> ids = m_overrides.keys();
    std::sort(ids.begin(), ids.end(), [](ComponentId a, ComponentId b) {
        return static_cast<quint32>(a) < static_cast<quint32>(b);
    });
    return ids;
}

bool ComponentTokenOverrides::semanticallyEquals(
    const ComponentTokenOverrides& other) const
{
    return equalOverrideMap(m_overrides, other.m_overrides)
        && equalOverrideMap(m_extensionOverrides, other.m_extensionOverrides);
}

} // namespace QtMaterial
