#pragma once

#include <QByteArray>
#include <QColor>
#include <QJsonDocument>
#include <QMetaType>
#include <QObject>
#include <QString>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

namespace QtMaterial {

enum class ThemeChangeReason {
    SetTheme,
    SetOptions,
    Rebuild,
    SeedColor,
    ImportJson,
    ImportFile
};

class QTMATERIAL3_THEME_EXPORT ThemeManager : public QObject {
    Q_OBJECT
public:
    static ThemeManager& instance();

    const Theme& theme() const noexcept;
    const ThemeOptions& options() const noexcept;

    quint64 revision() const noexcept;
    bool isApplyingTheme() const noexcept;

    bool setTheme(const Theme& theme, ThemeChangeReason reason = ThemeChangeReason::SetTheme);
    bool setThemeOptions(const ThemeOptions& options);
    bool rebuildTheme(ThemeChangeReason reason = ThemeChangeReason::Rebuild);
    bool applySeedColor(const QColor& seed);
    bool applySeedColor(const QColor& seed, ThemeMode mode);

    QByteArray exportThemeJson(
        QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;
    bool exportThemeToFile(const QString& filePath,
                           QString* errorString = nullptr,
                           QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;

    bool importThemeJson(const QByteArray& json,
                         QString* errorString = nullptr,
                         ThemeReadMode mode = ThemeReadMode::Lenient);
    bool importThemeFromFile(const QString& filePath,
                             QString* errorString = nullptr,
                             ThemeReadMode mode = ThemeReadMode::Lenient);

signals:
    void themeChanged(const QtMaterial::Theme& theme);
    void themeChangedWithReason(const QtMaterial::Theme& theme,
                                QtMaterial::ThemeChangeReason reason);
    void themeRepolishRequested(const QtMaterial::Theme& theme,
                                QtMaterial::ThemeChangeReason reason);

private:
    explicit ThemeManager(QObject* parent = nullptr);

    bool applyResolvedTheme(const Theme& theme,
                            const ThemeOptions& options,
                            ThemeChangeReason reason);
    bool ensureThreadAffinity(const char* operation) const;
    QByteArray stableFingerprint(const Theme& theme) const;

    ThemeOptions m_options;
    Theme m_theme;
    QByteArray m_themeFingerprint;
    ThemeBuilder m_builder;
    bool m_applyingTheme;
    quint64 m_revision;
};

} // namespace QtMaterial

Q_DECLARE_METATYPE(QtMaterial::Theme)
Q_DECLARE_METATYPE(QtMaterial::ThemeChangeReason)
