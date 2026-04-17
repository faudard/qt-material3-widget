#include "qtmaterial/theme/qtmaterialthemeobserver.h"

#include "qtmaterial/theme/qtmaterialthememanager.h"

namespace QtMaterial {

ThemeObserver::ThemeObserver(QObject* parent)
    : QObject(parent)
{
    QObject::connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &ThemeObserver::onThemeChanged);
}

ThemeObserver::~ThemeObserver() = default;

} // namespace QtMaterial
