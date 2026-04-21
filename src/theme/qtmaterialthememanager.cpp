#include "qtmaterial/theme/qtmaterialthememanager.h"

#include "qtmaterial/theme/qtmaterialthemeserializer.h"

namespace QtMaterial {

ThemeManager& ThemeManager::instance()
{
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

const Theme& ThemeManager::theme() const noexcept
{
    return m_theme;
}

const ThemeOptions& ThemeManager::options() const noexcept
{
    return m_options;
}

void ThemeManager::setTheme(const Theme& theme)
{
    m_theme = theme;
    m_options = theme.options();
    emit themeChanged(m_theme);
}

void ThemeManager::setThemeOptions(const ThemeOptions& options)
{
    m_options = options;
    rebuildTheme();
}

void ThemeManager::rebuildTheme()
{
    m_theme = m_builder.build(m_options);
    emit themeChanged(m_theme);
}

void ThemeManager::applySeedColor(const QColor& seed)
{
    m_options.sourceColor = seed;
    rebuildTheme();
}

void ThemeManager::applySeedColor(const QColor& seed, ThemeMode mode)
{
    m_options.sourceColor = seed;
    m_options.mode = mode;
    rebuildTheme();
}

QByteArray ThemeManager::exportThemeJson(QJsonDocument::JsonFormat format) const
{
    return ThemeSerializer::toJson(m_theme, format);
}

bool ThemeManager::exportThemeToFile(const QString& filePath,
                                     QString* errorString,
                                     QJsonDocument::JsonFormat format) const
{
    return ThemeSerializer::writeToFile(m_theme, filePath, errorString, format);
}

bool ThemeManager::importThemeJson(const QByteArray& json, QString* errorString)
{
    bool ok = false;
    const Theme imported = ThemeSerializer::fromJson(json, &ok, errorString);
    if (!ok) {
        return false;
    }

    setTheme(imported);
    return true;
}

bool ThemeManager::importThemeFromFile(const QString& filePath, QString* errorString)
{
    Theme imported;
    if (!ThemeSerializer::readFromFile(filePath, &imported, errorString)) {
        return false;
    }

    setTheme(imported);
    return true;
}

} // namespace QtMaterial
