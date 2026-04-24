#pragma once

#include <QObject>
#include <QString>

#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialthemeoptions.h"

class ThemeStudioController : public QObject
{
    Q_OBJECT

public:
    explicit ThemeStudioController(QObject* parent = nullptr);

    const QtMaterial::ThemeOptions& pendingOptions() const noexcept;
    const QtMaterial::Theme& currentTheme() const noexcept;
    bool isDirty() const noexcept;
    QString currentFilePath() const noexcept;
    QString currentPresetId() const noexcept;

public slots:
    void setSeedColor(const QColor& color);
    void setMode(QtMaterial::ThemeMode mode);
    void setContrast(QtMaterial::ContrastMode contrast);
    void setExpressive(bool enabled);
    void applyPreset(const QString& presetId);

    void applyPending();
    void resetToDefaults();

    bool importJsonFile(const QString& path, QString* errorString = nullptr);
    bool exportJsonFile(const QString& path, QString* errorString = nullptr) const;
    bool importQtMaterialXmlFile(const QString& path, QString* errorString = nullptr);
    bool exportQtMaterialXmlFile(const QString& path, QString* errorString = nullptr) const;
signals:
    void pendingOptionsChanged(const QtMaterial::ThemeOptions& options);
    void themeApplied(const QtMaterial::Theme& theme);
    void themeJsonChanged(const QString& json);
    void dirtyStateChanged(bool dirty);
    void currentFilePathChanged(const QString& path);
    void currentPresetChanged(const QString& presetId);
    void errorOccurred(const QString& message);

private:
    void setDirty(bool dirty);
    void syncFromThemeManager();
    void emitThemeJson();

    QtMaterial::ThemeOptions m_pendingOptions;
    bool m_dirty {false};
    QString m_currentFilePath;
    QString m_currentPresetId;
};
