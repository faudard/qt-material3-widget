#include "qtmaterial/core/qtmaterialthemecontextbinding.h"

#include <QEvent>
#include <QWidget>

#include "private/qtmaterialthemecontextpropagation_p.h"
#include "qtmaterial/theme/qtmaterialthemecontexthost.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

namespace QtMaterial {

QtMaterialThemeContextBinding::QtMaterialThemeContextBinding(
    QWidget* host,
    QObject* parent)
    : QObject(parent ? parent : host)
    , m_host(host)
{
    Q_ASSERT(host);
    if (m_host) {
        m_host->installEventFilter(this);
    }
    refreshEffectiveContext(false);
}

QtMaterialThemeContextBinding::~QtMaterialThemeContextBinding()
{
    if (m_host) {
        m_host->removeEventFilter(this);
    }
}

void QtMaterialThemeContextBinding::setThemeContext(ThemeContext* context)
{
    if (m_themeContext.data() == context) {
        return;
    }

    m_themeContext = context;
    refreshEffectiveContext(true);
}

ThemeContext* QtMaterialThemeContextBinding::themeContext() const noexcept
{
    return m_themeContext.data();
}

ThemeContext*
QtMaterialThemeContextBinding::effectiveThemeContext() const noexcept
{
    if (m_themeContext) {
        return m_themeContext.data();
    }

    QWidget* ancestor = m_host ? m_host->parentWidget() : nullptr;
    while (ancestor) {
        if (auto* host = qobject_cast<ThemeContextHost*>(ancestor)) {
            return host->effectiveThemeContext();
        }
        ancestor = ancestor->parentWidget();
    }

    return ThemeManager::instance().defaultContext();
}

const Theme& QtMaterialThemeContextBinding::theme() const noexcept
{
    ThemeContext* context = effectiveThemeContext();
    Q_ASSERT(context);
    return context->theme();
}

bool QtMaterialThemeContextBinding::eventFilter(
    QObject* watched,
    QEvent* event)
{
    if (watched == m_host && event) {
        const bool hierarchyChanged =
            event->type() == QEvent::ParentChange
            || event->type() == ThemeContextPropagation::eventType();
        if (hierarchyChanged) {
            refreshEffectiveContext(true);
        }
    }

    return QObject::eventFilter(watched, event);
}

void QtMaterialThemeContextBinding::handleThemeChanged(
    const Theme& theme)
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
    ThemeContext* nextContext = effectiveThemeContext();
    if (m_effectiveThemeContext.data() == nextContext) {
        return false;
    }

    QObject::disconnect(m_themeChangedConnection);
    QObject::disconnect(m_themeDestroyedConnection);

    m_effectiveThemeContext = nextContext;
    if (nextContext) {
        m_themeChangedConnection = QObject::connect(
            nextContext,
            &ThemeContext::themeChanged,
            this,
            &QtMaterialThemeContextBinding::handleThemeChanged);
        m_themeDestroyedConnection = QObject::connect(
            nextContext,
            &QObject::destroyed,
            this,
            &QtMaterialThemeContextBinding::handleThemeContextDestroyed);
    }

    if (notify) {
        emit effectiveThemeContextChanged(nextContext);
        if (nextContext) {
            emit themeChanged(nextContext->theme());
        }
        notifyDescendants();
    }

    return true;
}

void QtMaterialThemeContextBinding::notifyDescendants()
{
    ThemeContextPropagation::notifyDescendants(m_host.data());
}

} // namespace QtMaterial
