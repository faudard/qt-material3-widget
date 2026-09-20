#include "themestudiocontroller.h"

#include <QColor>
#include <QJsonDocument>

#include "themepresetcatalog.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"
#include "qtmaterial/theme/qtmaterialsystemtheme.h"
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
    setPreference(
        mode == ThemeMode::Dark ? ThemePreference::Dark : ThemePreference::Light);
}

void ThemeStudioController::setPreference(ThemePreference preference)
{
    ThemeMode resolvedMode = m_pendingOptions.mode;
    switch (preference) {
    case ThemePreference::Light:
        resolvedMode = ThemeMode::Light;
        break;
    case ThemePreference::Dark:
        resolvedMode = ThemeMode::Dark;
        break;
    case ThemePreference::FollowSystem:
        resolvedMode = SystemTheme::instance().effectiveMode();
        break;
    }

    if (m_pendingOptions.preference == preference
        && m_pendingOptions.mode == resolvedMode) {
        return;
    }

    m_pendingOptions.preference = preference;
    m_pendingOptions.mode = resolvedMode;
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
    const ThemeVariant variant =
        enabled ? ThemeVariant::Expressive : ThemeVariant::TonalSpot;

    if (m_pendingOptions.variant == variant) {
        return;
    }

    m_pendingOptions.variant = variant;
    m_currentPresetId.clear();
    emit currentPresetChanged(m_currentPresetId);
    setDirty(true);
    emit pendingOptionsChanged(m_pendingOptions);
}

void ThemeStudioController::setBackendPolicy(ColorBackendPolicy policy)
{
    if (m_pendingOptions.backendPolicy == policy) {
        return;
    }

    m_pendingOptions.backendPolicy = policy;
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

    if (m_pendingOptions.preference == ThemePreference::FollowSystem) {
        m_pendingOptions.mode = SystemTheme::instance().effectiveMode();
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
    Theme imported;
    if (!ThemeSerializer::readFromFile(
            path, &imported, ThemeReadMode::Strict, errorString)) {
        emit errorOccurred(errorString ? *errorString : QStringLiteral("Import failed."));
        return false;
    }

    ThemeManager::instance().setTheme(imported, ThemeChangeReason::External);
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

bool ThemeStudioController::applyJson(const QByteArray& json, QString* errorString)
{
    bool ok = false;
    QString localError;
    const Theme imported = ThemeSerializer::fromJson(
        json, ThemeReadMode::Strict, &ok, &localError);
    if (!ok) {
        if (errorString) {
            *errorString = localError;
        }
        return false;
    }

    ThemeManager::instance().setTheme(imported, ThemeChangeReason::External);
    m_currentFilePath.clear();
    m_currentPresetId.clear();
    syncFromThemeManager();

    emit currentPresetChanged(m_currentPresetId);
    emit currentFilePathChanged(m_currentFilePath);
    emit themeApplied(ThemeManager::instance().theme());
    emitThemeJson();
    setDirty(false);
    if (errorString) {
        errorString->clear();
    }
    return true;
}

bool ThemeStudioController::validateJson(
    const QByteArray& json,
    QString* errorString) const
{
    return ThemeSerializer::validateJson(
        json, ThemeReadMode::Strict, errorString);
}

bool ThemeStudioController::exportJsonFile(const QString& path, QString* errorString) const
{
    return ThemeSerializer::writeToFile(ThemeManager::instance().theme(), path, errorString);
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
        ThemeSerializer::toJson(ThemeManager::instance().theme(), QJsonDocument::Indented)));
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
