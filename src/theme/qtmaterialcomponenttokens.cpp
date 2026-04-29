#include "qtmaterial/theme/qtmaterialcomponenttokens.h"

#include <algorithm>

namespace QtMaterial {
namespace {

struct ComponentNamePair {
    ComponentId id;
    const char* name;
};

constexpr ComponentNamePair kComponentNames[] = {
    { ComponentId::Button, "Button" },
    { ComponentId::IconButton, "IconButton" },
    { ComponentId::FloatingActionButton, "FloatingActionButton" },
    { ComponentId::Checkbox, "Checkbox" },
    { ComponentId::RadioButton, "RadioButton" },
    { ComponentId::Switch, "Switch" },
    { ComponentId::Dialog, "Dialog" },
    { ComponentId::NavigationDrawer, "NavigationDrawer" },
    { ComponentId::BottomSheet, "BottomSheet" },
    { ComponentId::Banner, "Banner" },
    { ComponentId::Card, "Card" },
    { ComponentId::TopAppBar, "TopAppBar" },
    { ComponentId::BottomAppBar, "BottomAppBar" },
    { ComponentId::TextField, "TextField" },
    { ComponentId::AutoCompletePopup, "AutoCompletePopup" },
    { ComponentId::DateField, "DateField" },
    { ComponentId::List, "List" },
    { ComponentId::ListItem, "ListItem" },
    { ComponentId::Divider, "Divider" },
    { ComponentId::Tabs, "Tabs" },
    { ComponentId::Snackbar, "Snackbar" },
    { ComponentId::ProgressIndicator, "ProgressIndicator" },
    { ComponentId::Custom, "Custom" },
};

QString normalizedComponentName(QString name)
{
    name = name.trimmed();
    return name;
}

} // namespace

QString componentIdToString(ComponentId id)
{
    for (const auto& pair : kComponentNames) {
        if (pair.id == id) {
            return QString::fromLatin1(pair.name);
        }
    }
    return QStringLiteral("Custom");
}

bool componentIdFromString(const QString& name, ComponentId* outId)
{
    const QString normalized = normalizedComponentName(name);
    for (const auto& pair : kComponentNames) {
        if (normalized.compare(QLatin1String(pair.name), Qt::CaseInsensitive) == 0) {
            if (outId) {
                *outId = pair.id;
            }
            return true;
        }
    }
    return false;
}

bool ComponentTokenOverride::isEmpty() const
{
    return colors.isEmpty() && typography.isEmpty() && shapes.isEmpty()
        && elevations.isEmpty() && motion.isEmpty() && density.isEmpty()
        && iconSizes.isEmpty() && !hasStateLayer && custom.isEmpty();
}

ComponentTokenOverrides::ComponentTokenOverrides() = default;
ComponentTokenOverrides::~ComponentTokenOverrides() = default;

bool ComponentTokenOverrides::contains(const QString& componentName) const
{
    return m_overrides.contains(normalizedComponentName(componentName));
}

bool ComponentTokenOverrides::contains(ComponentId componentId) const
{
    return contains(componentIdToString(componentId));
}

ComponentTokenOverride ComponentTokenOverrides::overrideFor(const QString& componentName) const
{
    return m_overrides.value(normalizedComponentName(componentName), ComponentTokenOverride{});
}

ComponentTokenOverride ComponentTokenOverrides::overrideFor(ComponentId componentId) const
{
    return overrideFor(componentIdToString(componentId));
}

void ComponentTokenOverrides::setOverride(const QString& componentName, const ComponentTokenOverride& overrideTokens)
{
    const QString normalized = normalizedComponentName(componentName);
    if (normalized.isEmpty() || overrideTokens.isEmpty()) {
        return;
    }
    m_overrides.insert(normalized, overrideTokens);
}

void ComponentTokenOverrides::setOverride(ComponentId componentId, const ComponentTokenOverride& overrideTokens)
{
    setOverride(componentIdToString(componentId), overrideTokens);
}

void ComponentTokenOverrides::removeOverride(const QString& componentName)
{
    m_overrides.remove(normalizedComponentName(componentName));
}

void ComponentTokenOverrides::removeOverride(ComponentId componentId)
{
    removeOverride(componentIdToString(componentId));
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
