#include "qtmaterial/theme/qtmaterialthememanager.h"

namespace QtMaterial {

ThemeManager& ThemeManager::instance() {
    static ThemeManager manager;
    return manager;
}

ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent)
    , m_options()
    , m_theme()
    , m_builder()
{
    m_theme = m_builder.build(m_options);
}

const Theme& ThemeManager::theme() const noexcept { return m_theme; }
const ThemeOptions& ThemeManager::options() const noexcept { return m_options; }

void ThemeManager::setTheme(const Theme& theme) {
    m_theme = theme;
    m_options = theme.options();
    emit themeChanged(m_theme);
}

void ThemeManager::setThemeOptions(const ThemeOptions& options) {
    m_options = options;
    rebuildTheme();
}

void ThemeManager::rebuildTheme() {
    m_theme = m_builder.build(m_options);
    emit themeChanged(m_theme);
}

} // namespace QtMaterial
