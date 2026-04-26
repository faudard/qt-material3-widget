#include "qtmaterial/theme/qtmaterialthememanager.h"

#include <QScopedValueRollback>
#include <QThread>
#include <QDebug>

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
    , m_themeFingerprint()
    , m_builder()
    , m_applyingTheme(false)
    , m_revision(0)
{
    qRegisterMetaType<QtMaterial::Theme>("QtMaterial::Theme");
    qRegisterMetaType<QtMaterial::ThemeChangeReason>("QtMaterial::ThemeChangeReason");

    m_theme = m_builder.build(m_options);
    m_themeFingerprint = stableFingerprint(m_theme);
}

const Theme& ThemeManager::theme() const noexcept
{
    return m_theme;
}

const ThemeOptions& ThemeManager::options() const noexcept
{
    return m_options;
}

quint64 ThemeManager::revision() const noexcept
{
    return m_revision;
}

bool ThemeManager::isApplyingTheme() const noexcept
{
    return m_applyingTheme;
}

bool ThemeManager::setTheme(const Theme& theme, ThemeChangeReason reason)
{
    return applyResolvedTheme(theme, theme.options(), reason);
}

bool ThemeManager::setThemeOptions(const ThemeOptions& options)
{
    if (!ensureThreadAffinity("setThemeOptions")) {
        return false;
    }
    if (m_options == options) {
        return false;
    }

    const Theme nextTheme = m_builder.build(options);
    return applyResolvedTheme(nextTheme, options, ThemeChangeReason::SetOptions);
}

bool ThemeManager::rebuildTheme(ThemeChangeReason reason)
{
    if (!ensureThreadAffinity("rebuildTheme")) {
        return false;
    }

    const Theme nextTheme = m_builder.build(m_options);
    return applyResolvedTheme(nextTheme, m_options, reason);
}

bool ThemeManager::applySeedColor(const QColor& seed)
{
    if (!ensureThreadAffinity("applySeedColor")) {
        return false;
    }
    if (m_options.sourceColor == seed) {
        return false;
    }

    ThemeOptions nextOptions = m_options;
    nextOptions.sourceColor = seed;
    const Theme nextTheme = m_builder.build(nextOptions);
    return applyResolvedTheme(nextTheme, nextOptions, ThemeChangeReason::SeedColor);
}

bool ThemeManager::applySeedColor(const QColor& seed, ThemeMode mode)
{
    if (!ensureThreadAffinity("applySeedColor")) {
        return false;
    }
    if (m_options.sourceColor == seed && m_options.mode == mode) {
        return false;
    }

    ThemeOptions nextOptions = m_options;
    nextOptions.sourceColor = seed;
    nextOptions.mode = mode;
    const Theme nextTheme = m_builder.build(nextOptions);
    return applyResolvedTheme(nextTheme, nextOptions, ThemeChangeReason::SeedColor);
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

bool ThemeManager::importThemeJson(const QByteArray& json,
                                   QString* errorString,
                                   ThemeReadMode mode)
{
    if (!ensureThreadAffinity("importThemeJson")) {
        if (errorString) {
            *errorString = QStringLiteral("ThemeManager::importThemeJson called from a non-owner thread.");
        }
        return false;
    }

    bool ok = false;
    const Theme imported = ThemeSerializer::fromJson(json, mode, &ok, errorString);
    if (!ok) {
        return false;
    }
    return applyResolvedTheme(imported, imported.options(), ThemeChangeReason::ImportJson);
}

bool ThemeManager::importThemeFromFile(const QString& filePath,
                                       QString* errorString,
                                       ThemeReadMode mode)
{
    if (!ensureThreadAffinity("importThemeFromFile")) {
        if (errorString) {
            *errorString = QStringLiteral("ThemeManager::importThemeFromFile called from a non-owner thread.");
        }
        return false;
    }

    Theme imported;
    if (!ThemeSerializer::readFromFile(filePath, &imported, mode, errorString)) {
        return false;
    }
    return applyResolvedTheme(imported, imported.options(), ThemeChangeReason::ImportFile);
}

bool ThemeManager::applyResolvedTheme(const Theme& theme,
                                      const ThemeOptions& options,
                                      ThemeChangeReason reason)
{
    if (!ensureThreadAffinity("applyResolvedTheme")) {
        return false;
    }
    if (m_applyingTheme) {
        qWarning() << "ThemeManager: ignored re-entrant theme application.";
        return false;
    }

    const QByteArray nextFingerprint = stableFingerprint(theme);
    if (nextFingerprint == m_themeFingerprint && options == m_options) {
        return false;
    }

    QScopedValueRollback<bool> applyingGuard(m_applyingTheme, true);

    m_theme = theme;
    m_options = options;
    m_themeFingerprint = nextFingerprint;
    ++m_revision;

    emit themeChanged(m_theme);
    emit themeChangedWithReason(m_theme, reason);
    emit themeRepolishRequested(m_theme, reason);

    return true;
}

bool ThemeManager::ensureThreadAffinity(const char* operation) const
{
    if (thread() == QThread::currentThread()) {
        return true;
    }

    qWarning() << "ThemeManager:" << operation
               << "must be called from the ThemeManager owner thread.";
    return false;
}

QByteArray ThemeManager::stableFingerprint(const Theme& theme) const
{
    return ThemeSerializer::toJson(theme, QJsonDocument::Compact);
}

} // namespace QtMaterial
