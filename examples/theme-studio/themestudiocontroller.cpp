#include "themestudiocontroller.h"

#include <QColor>

#include "qtmaterial/theme/qtmaterialthememanager.h"

using namespace QtMaterial;

ThemeStudioController::ThemeStudioController(QObject* parent)
    : QObject(parent)
{
    syncFromThemeManager();

    connect(&ThemeManager::instance(),
            &ThemeManager::themeChanged,
            this,
            [this](const Theme& theme) {
                m_pendingOptions = ThemeManager::instance().options();
                emit pendingOptionsChanged(m_pendingOptions);
                emit themeApplied(theme);
            });
}

const ThemeOptions& ThemeStudioController::pendingOptions() const noexcept
{
    return m_pendingOptions;
}

const Theme& ThemeStudioController::currentTheme() const noexcept
{
    return ThemeManager::instance().theme();
}

bool ThemeStudioController::isDirty() const noexcept
{
    return m_dirty;
}

QString ThemeStudioController::currentFilePath() const noexcept
{
    return m_currentFilePath;
}

void ThemeStudioController::setSeedColor(const QColor& color)
{
    if (!color.isValid() || m_pendingOptions.sourceColor == color) {
        return;
    }

    m_pendingOptions.sourceColor = color;
    setDirty(true);
    emit pendingOptionsChanged(m_pendingOptions);
}

void ThemeStudioController::setMode(ThemeMode mode)
{
    if (m_pendingOptions.mode == mode) {
        return;
    }

    m_pendingOptions.mode = mode;
    setDirty(true);
    emit pendingOptionsChanged(m_pendingOptions);
}

void ThemeStudioController::setContrast(ContrastMode contrast)
{
    if (m_pendingOptions.contrast == contrast) {
        return;
    }

    m_pendingOptions.contrast = contrast;
    setDirty(true);
    emit pendingOptionsChanged(m_pendingOptions);
}

void ThemeStudioController::setExpressive(bool enabled)
{
    if (m_pendingOptions.expressive == enabled) {
        return;
    }

    m_pendingOptions.expressive = enabled;
    setDirty(true);
    emit pendingOptionsChanged(m_pendingOptions);
}

void ThemeStudioController::applyPending()
{
    ThemeManager::instance().setThemeOptions(m_pendingOptions);
    setDirty(false);
    emit themeApplied(ThemeManager::instance().theme());
}

void ThemeStudioController::resetToDefaults()
{
    m_pendingOptions = ThemeOptions {};
    ThemeManager::instance().setThemeOptions(m_pendingOptions);
    m_currentFilePath.clear();

    emit pendingOptionsChanged(m_pendingOptions);
    emit currentFilePathChanged(m_currentFilePath);
    emit themeApplied(ThemeManager::instance().theme());
    setDirty(false);
}

bool ThemeStudioController::importJsonFile(const QString& path, QString* errorString)
{
    if (!ThemeManager::instance().importThemeFromFile(path, errorString)) {
        emit errorOccurred(errorString ? *errorString : QStringLiteral("Import failed."));
        return false;
    }

    m_currentFilePath = path;
    syncFromThemeManager();

    emit currentFilePathChanged(m_currentFilePath);
    emit themeApplied(ThemeManager::instance().theme());
    setDirty(false);
    return true;
}

bool ThemeStudioController::exportJsonFile(const QString& path, QString* errorString) const
{
    return ThemeManager::instance().exportThemeToFile(path, errorString);
}

void ThemeStudioController::setDirty(bool dirty)
{
    if (m_dirty == dirty) {
        return;
    }

    m_dirty = dirty;
    emit dirtyStateChanged(m_dirty);
}

void ThemeStudioController::syncFromThemeManager()
{
    m_pendingOptions = ThemeManager::instance().options();
    emit pendingOptionsChanged(m_pendingOptions);
}
