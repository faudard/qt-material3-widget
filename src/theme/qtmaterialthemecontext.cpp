#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include <QDebug>
#include <QThread>
namespace QtMaterial {
ThemeContext::ThemeContext(QObject* parent) : ThemeContext(Theme(), parent) {}
ThemeContext::ThemeContext(const Theme& theme, QObject* parent)
    : QObject(parent), m_theme(theme) {}
const Theme& ThemeContext::theme() const noexcept { return m_theme; }
quint64 ThemeContext::revision() const noexcept { return m_revision; }
bool ThemeContext::setTheme(const Theme& theme)
{
    if (theme == m_theme) {
        return true;
    }

    if (!ensureThreadAffinity("setTheme") || theme == m_theme) return false;
    m_theme = theme;
    ++m_revision;
    emit themeChanged(m_theme);
    emit revisionChanged(m_revision);
    return true;
}
bool ThemeContext::ensureThreadAffinity(const char* operation) const
{
    if (thread() == QThread::currentThread()) return true;
    qWarning() << "ThemeContext:" << operation
               << "must be called from the ThemeContext owner thread.";
    return false;
}
} // namespace QtMaterial
