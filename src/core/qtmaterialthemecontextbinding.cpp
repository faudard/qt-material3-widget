#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"
#include <QEvent>
#include <QWidget>
#include "private/qtmaterialthemecontextpropagation_p.h"
#include "qtmaterial/theme/private/qtmaterialthemecontextdefaults_p.h"
#include "qtmaterial/theme/qtmaterialthemecontexthost.h"
namespace QtMaterial {
QtMaterialThemeContextBinding::QtMaterialThemeContextBinding(QWidget* host, QObject* parent)
    : QObject(parent ? parent : host), m_host(host)
{
    Q_ASSERT(host);
    if (m_host) m_host->installEventFilter(this);
    refreshEffectiveContext(false);
}
QtMaterialThemeContextBinding::~QtMaterialThemeContextBinding()
{
    if (m_host) m_host->removeEventFilter(this);
}
void QtMaterialThemeContextBinding::setThemeContext(ThemeContext* context)
{
    if (m_themeContext.data() == context) return;
    m_themeContext = context;
    refreshEffectiveContext(true);
}
ThemeContext* QtMaterialThemeContextBinding::themeContext() const noexcept
{
    return m_themeContext.data();
}
ThemeContext* QtMaterialThemeContextBinding::effectiveThemeContext() const noexcept
{
    if (m_themeContext) return m_themeContext.data();
    QWidget* ancestor = m_host ? m_host->parentWidget() : nullptr;
    while (ancestor) {
        if (auto* host = qobject_cast<ThemeContextHost*>(ancestor)) {
            if (ThemeContext* inherited = host->effectiveThemeContext())
                return inherited;
        }
        ancestor = ancestor->parentWidget();
    }
    return ThemeContextDefaults::defaultContext();
}
const Theme& QtMaterialThemeContextBinding::theme() const noexcept
{
    ThemeContext* context = effectiveThemeContext();
    Q_ASSERT(context);
    return context->theme();
}
bool QtMaterialThemeContextBinding::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_host && event
        && (event->type() == QEvent::ParentChange
            || event->type() == ThemeContextPropagation::eventType()))
        refreshEffectiveContext(true);
    return QObject::eventFilter(watched, event);
}
void QtMaterialThemeContextBinding::handleThemeChanged(const Theme& theme)
{
    emit themeChanged(theme);
}
void QtMaterialThemeContextBinding::handleThemeContextDestroyed()
{
    m_themeContext.clear();
    m_effectiveThemeContext.clear();
    refreshEffectiveContext(true);
}
bool QtMaterialThemeContextBinding::refreshEffectiveContext(bool notify)
{
    ThemeContext* next = effectiveThemeContext();
    if (m_effectiveThemeContext.data() == next) return false;
    QObject::disconnect(m_themeChangedConnection);
    QObject::disconnect(m_themeDestroyedConnection);
    m_effectiveThemeContext = next;
    if (next) {
        m_themeChangedConnection = QObject::connect(
            next, &ThemeContext::themeChanged,
            this, &QtMaterialThemeContextBinding::handleThemeChanged);
        m_themeDestroyedConnection = QObject::connect(
            next, &QObject::destroyed,
            this, &QtMaterialThemeContextBinding::handleThemeContextDestroyed);
    }
    if (notify) {
        emit effectiveThemeContextChanged(next);
        if (next) emit themeChanged(next->theme());
        notifyDescendants();
    }
    return true;
}
void QtMaterialThemeContextBinding::notifyDescendants()
{
    ThemeContextPropagation::notifyDescendants(m_host.data());
}
} // namespace QtMaterial
