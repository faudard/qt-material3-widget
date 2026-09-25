#include "qtmaterial/theme/private/qtmaterialthemecontextdefaults_p.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
namespace QtMaterial {
ThemeContext* ThemeContextDefaults::defaultContext() noexcept
{
    return ThemeManager::instance().defaultContext();
}
} // namespace QtMaterial
