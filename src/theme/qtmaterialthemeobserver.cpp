#include "qtmaterial/theme/qtmaterialthemeobserver.h"

#include "qtmaterial/theme/qtmaterialthememanager.h"

namespace QtMaterial {

ThemeObserver::ThemeObserver(QObject* parent)
    : QObject(parent)
    , m_themeConnection()
    , m_themeObservationEnabled(false)
{
    setThemeObservationEnabled(true);
}

ThemeObserver::~ThemeObserver()
{
    setThemeObservationEnabled(false);
}

bool ThemeObserver::isThemeObservationEnabled() const noexcept
{
    return m_themeObservationEnabled;
}

void ThemeObserver::setThemeObservationEnabled(bool enabled)
{
    if (enabled == m_themeObservationEnabled) {
        return;
    }

    if (enabled) {
        m_themeConnection = QObject::connect(
            &ThemeManager::instance(),
            &ThemeManager::themeChanged,
            this,
            &ThemeObserver::onThemeChanged,
            Qt::UniqueConnection);
        m_themeObservationEnabled = static_cast<bool>(m_themeConnection);
        return;
    }

    if (m_themeConnection) {
        QObject::disconnect(m_themeConnection);
        m_themeConnection = QMetaObject::Connection();
    }
    m_themeObservationEnabled = false;
}

void ThemeObserver::syncTheme()
{
    onThemeChanged(ThemeManager::instance().theme());
}

} // namespace QtMaterial
