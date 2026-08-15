#include "qtmaterial/theme/qtmaterialthememanager.h"
#include <QDebug>
#include <QScopedValueRollback>
#include <QThread>
namespace QtMaterial {
ThemeManager& ThemeManager::instance()
{
    static ThemeManager manager;
    return manager;
}
ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent), m_options(), m_builder(),
      m_defaultContext(new ThemeContext(m_builder.build(m_options), this))
{
    qRegisterMetaType<QtMaterial::Theme>("QtMaterial::Theme");
    qRegisterMetaType<QtMaterial::ThemeChangeReason>("QtMaterial::ThemeChangeReason");
}
ThemeContext* ThemeManager::defaultContext() noexcept { return m_defaultContext; }
const ThemeContext* ThemeManager::defaultContext() const noexcept { return m_defaultContext; }
const Theme& ThemeManager::theme() const noexcept { return m_defaultContext->theme(); }
const ThemeOptions& ThemeManager::options() const noexcept { return m_options; }
quint64 ThemeManager::revision() const noexcept { return m_defaultContext->revision(); }
bool ThemeManager::isApplyingTheme() const noexcept { return m_applyingTheme; }
bool ThemeManager::setTheme(const Theme& theme, ThemeChangeReason reason)
{
    return applyResolvedTheme(theme, theme.options(), reason);
}
bool ThemeManager::setThemeOptions(const ThemeOptions& options)
{
    if (!ensureThreadAffinity("setThemeOptions") || m_options == options) return false;
    return applyResolvedTheme(m_builder.build(options), options,
                              ThemeChangeReason::SetOptions);
}
bool ThemeManager::rebuildTheme(ThemeChangeReason reason)
{
    if (!ensureThreadAffinity("rebuildTheme")) return false;
    return applyResolvedTheme(m_builder.build(m_options), m_options, reason);
}
bool ThemeManager::applySeedColor(const QColor& seed)
{
    if (!ensureThreadAffinity("applySeedColor") || m_options.sourceColor == seed)
        return false;
    ThemeOptions next = m_options;
    next.sourceColor = seed;
    return applyResolvedTheme(m_builder.build(next), next,
                              ThemeChangeReason::SeedColor);
}
bool ThemeManager::applySeedColor(const QColor& seed, ThemeMode mode)
{
    if (!ensureThreadAffinity("applySeedColor")
        || (m_options.sourceColor == seed && m_options.mode == mode))
        return false;
    ThemeOptions next = m_options;
    next.sourceColor = seed;
    next.mode = mode;
    return applyResolvedTheme(m_builder.build(next), next,
                              ThemeChangeReason::SeedColor);
}
bool ThemeManager::applyResolvedTheme(const Theme& theme,
                                      const ThemeOptions& options,
                                      ThemeChangeReason reason)
{
    if (!ensureThreadAffinity("applyResolvedTheme")) return false;
    if (m_applyingTheme) {
        qWarning() << "ThemeManager: ignored re-entrant theme application.";
        return false;
    }
    if (theme == m_defaultContext->theme() && options == m_options) return false;

    QScopedValueRollback<bool> guard(m_applyingTheme, true);
    m_options = options;
    if (!m_defaultContext->setTheme(theme)) return false;

    const Theme& applied = m_defaultContext->theme();
    emit themeChanged(applied);
    emit themeChangedWithReason(applied, reason);
    emit themeRepolishRequested(applied, reason);
    return true;
}
bool ThemeManager::ensureThreadAffinity(const char* operation) const
{
    if (thread() == QThread::currentThread()) return true;
    qWarning() << "ThemeManager:" << operation
               << "must be called from the ThemeManager owner thread.";
    return false;
}
} // namespace QtMaterial
