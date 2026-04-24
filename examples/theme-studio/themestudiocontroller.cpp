#include "themestudiocontroller.h"

#include <QColor>
#include <QJsonDocument>

#include "themepresetcatalog.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/theme/qtmaterialxmlthemeadapter.h"

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
                emitThemeJson();
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

QString ThemeStudioController::currentPresetId() const noexcept
{
    return m_currentPresetId;
}

void ThemeStudioController::setSeedColor(const QColor& color)
{
    if (!color.isValid() || m_pendingOptions.sourceColor == color) {
        return;
    }

    m_pendingOptions.sourceColor = color;
    m_currentPresetId.clear();
    emit currentPresetChanged(m_currentPresetId);
    setDirty(true);
    emit pendingOptionsChanged(m_pendingOptions);
}

void ThemeStudioController::setMode(ThemeMode mode)
{
    if (m_pendingOptions.mode == mode) {
        return;
    }

    m_pendingOptions.mode = mode;
    m_currentPresetId.clear();
    emit currentPresetChanged(m_currentPresetId);
    setDirty(true);
    emit pendingOptionsChanged(m_pendingOptions);
}

void ThemeStudioController::setContrast(ContrastMode contrast)
{
    if (m_pendingOptions.contrast == contrast) {
        return;
    }

    m_pendingOptions.contrast = contrast;
    m_currentPresetId.clear();
    emit currentPresetChanged(m_currentPresetId);
    setDirty(true);
    emit pendingOptionsChanged(m_pendingOptions);
}

void ThemeStudioController::setExpressive(bool enabled)
{
    if (m_pendingOptions.expressive == enabled) {
        return;
    }

    m_pendingOptions.expressive = enabled;
    m_currentPresetId.clear();
    emit currentPresetChanged(m_currentPresetId);
    setDirty(true);
    emit pendingOptionsChanged(m_pendingOptions);
}

void ThemeStudioController::applyPreset(const QString& presetId)
{
    if (presetId.isEmpty()) {
        return;
    }

    ThemePreset preset;
    if (!ThemePresetCatalog::findPresetById(presetId, &preset)) {
        return;
    }

    m_pendingOptions = preset.options;
    m_currentPresetId = preset.id;
    emit currentPresetChanged(m_currentPresetId);
    emit pendingOptionsChanged(m_pendingOptions);

    ThemeManager::instance().setThemeOptions(m_pendingOptions);
    emit themeApplied(ThemeManager::instance().theme());
    emitThemeJson();
    setDirty(false);
}

void ThemeStudioController::applyPending()
{
    if (m_currentPresetId.isEmpty()) {
        emit currentPresetChanged(QString());
    }

    ThemeManager::instance().setThemeOptions(m_pendingOptions);
    setDirty(false);
    emit themeApplied(ThemeManager::instance().theme());
    emitThemeJson();
}

void ThemeStudioController::resetToDefaults()
{
    const ThemePreset preset = ThemePresetCatalog::defaultPreset();
    m_pendingOptions = preset.options;
    m_currentPresetId = preset.id;
    m_currentFilePath.clear();

    ThemeManager::instance().setThemeOptions(m_pendingOptions);

    emit currentPresetChanged(m_currentPresetId);
    emit pendingOptionsChanged(m_pendingOptions);
    emit currentFilePathChanged(m_currentFilePath);
    emit themeApplied(ThemeManager::instance().theme());
    emitThemeJson();
    setDirty(false);
}

bool ThemeStudioController::importJsonFile(const QString& path, QString* errorString)
{
    if (!ThemeManager::instance().importThemeFromFile(path, errorString)) {
        emit errorOccurred(errorString ? *errorString : QStringLiteral("Import failed."));
        return false;
    }

    m_currentFilePath = path;
    m_currentPresetId.clear();
    syncFromThemeManager();

    emit currentPresetChanged(m_currentPresetId);
    emit currentFilePathChanged(m_currentFilePath);
    emit themeApplied(ThemeManager::instance().theme());
    emitThemeJson();
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
    emitThemeJson();
}

void ThemeStudioController::emitThemeJson()
{
    emit themeJsonChanged(QString::fromUtf8(
        ThemeManager::instance().exportThemeJson(QJsonDocument::Indented)));
}

bool ThemeStudioController::importQtMaterialXmlFile(const QString& path, QString* errorString)
{
    QtMaterial::Theme imported;
    if (!QtMaterial::XmlThemeAdapter::readQtMaterialXmlFile(path, &imported, errorString)) {
        emit errorOccurred(errorString ? *errorString : QStringLiteral("XML import failed."));
        return false;
    }

    QtMaterial::ThemeManager::instance().setTheme(imported);
    m_currentFilePath = path;
    m_currentPresetId.clear();
    syncFromThemeManager();

    emit currentPresetChanged(m_currentPresetId);
    emit currentFilePathChanged(m_currentFilePath);
    emit themeApplied(QtMaterial::ThemeManager::instance().theme());
    emitThemeJson();
    setDirty(false);
    return true;
}

bool ThemeStudioController::exportQtMaterialXmlFile(const QString& path, QString* errorString) const
{
    return QtMaterial::XmlThemeAdapter::writeQtMaterialXmlFile(
        QtMaterial::ThemeManager::instance().theme(),
        path,
        errorString);
}