#pragma once

#include <QtGlobal>
#include <utility>

#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"
#include "qtmaterial/specs/qtmaterialmenuspecresolver.h"
#include "qtmaterial/specs/qtmaterialtabsspecresolver.h"

namespace QtMaterial {
namespace NavigationSpecResolution {

template <typename... Args>
inline auto menuSpec(
    const QtMaterialThemeContextBinding* binding,
    Args&&... args)
    -> decltype(MenuSpecResolver().menuSpec(
        binding->theme(),
        std::forward<Args>(args)...))
{
    Q_ASSERT(binding);
    return MenuSpecResolver().menuSpec(
        binding->theme(),
        std::forward<Args>(args)...);
}

template <typename... Args>
inline auto tabsSpec(
    const QtMaterialThemeContextBinding* binding,
    Args&&... args)
    -> decltype(TabsSpecResolver().resolve(
        binding->theme(),
        std::forward<Args>(args)...))
{
    Q_ASSERT(binding);
    return TabsSpecResolver().resolve(
        binding->theme(),
        std::forward<Args>(args)...);
}

} // namespace NavigationSpecResolution
} // namespace QtMaterial
