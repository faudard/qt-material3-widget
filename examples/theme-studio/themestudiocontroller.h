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

public slots:
    void setSeedColor(const QColor& color);
    void setMode(QtMaterial::ThemeMode mode);
    void setContrast(QtMaterial::ContrastMode contrast);
    void setExpressive(bool enabled);

    void applyPending();
    void resetToDefaults();

    bool importJsonFile(const QString& path, QString* errorString = nullptr);
    bool exportJsonFile(const QString& path, QString* errorString = nullptr) const;

signals:
    void pendingOptionsChanged(const QtMaterial::ThemeOptions& options);
    void themeApplied(const QtMaterial::Theme& theme);
    void dirtyStateChanged(bool dirty);
    void currentFilePathChanged(const QString& path);
    void errorOccurred(const QString& message);

private:
    void setDirty(bool dirty);
    void syncFromThemeManager();

    QtMaterial::ThemeOptions m_pendingOptions;
    bool m_dirty {false};
    QString m_currentFilePath;
};
