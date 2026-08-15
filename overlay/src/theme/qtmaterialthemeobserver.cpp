#include "qtmaterial/theme/qtmaterialthemeobserver.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/theme/qtmaterialthemecontextdefaults.h"
namespace QtMaterial {
ThemeObserver::ThemeObserver(QObject* parent) : QObject(parent)
{
    refreshConnection(false);
}
ThemeObserver::~ThemeObserver()
{
    QObject::disconnect(m_themeConnection);
    QObject::disconnect(m_destroyedConnection);
}
void ThemeObserver::setThemeContext(ThemeContext* context)
{
    if (m_themeContext.data() == context) return;
    m_themeContext = context;
    refreshConnection(true);
}
ThemeContext* ThemeObserver::themeContext() const noexcept { return m_themeContext.data(); }
ThemeContext* ThemeObserver::effectiveThemeContext() const noexcept
{
    return m_themeContext ? m_themeContext.data()
                          : ThemeContextDefaults::defaultContext();
}
bool ThemeObserver::isThemeObservationEnabled() const noexcept
{
    return m_themeObservationEnabled;
}
void ThemeObserver::setThemeObservationEnabled(bool enabled)
{
    if (enabled == m_themeObservationEnabled) return;
    m_themeObservationEnabled = enabled;
    refreshConnection(false);
    emit themeObservationEnabledChanged(enabled);
}
void ThemeObserver::syncTheme()
{
    if (ThemeContext* context = effectiveThemeContext())
        onThemeChanged(context->theme());
}
void ThemeObserver::handleContextDestroyed()
{
    m_themeContext.clear();
    m_effectiveThemeContext.clear();
    refreshConnection(true);
}
bool ThemeObserver::refreshConnection(bool notify)
{
    ThemeContext* next = effectiveThemeContext();
    QObject::disconnect(m_themeConnection);
    QObject::disconnect(m_destroyedConnection);
    m_themeConnection = {};
    m_destroyedConnection = {};
    const bool changed = m_effectiveThemeContext.data() != next;
    m_effectiveThemeContext = next;
    if (m_themeObservationEnabled && next) {
        m_themeConnection = QObject::connect(
            next, &ThemeContext::themeChanged,
            this, &ThemeObserver::onThemeChanged);
        m_destroyedConnection = QObject::connect(
            next, &QObject::destroyed,
            this, &ThemeObserver::handleContextDestroyed);
    }
    if (notify && changed) {
        emit effectiveThemeContextChanged(next);
        if (next) onThemeChanged(next->theme());
    }
    return changed;
}
} // namespace QtMaterial
