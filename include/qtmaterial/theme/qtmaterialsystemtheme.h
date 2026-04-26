#pragma once

#include <QObject>
#include <QFont>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"

namespace QtMaterial {

class Theme;

/**
 * User preference for how application theme mode is resolved.
 *
 * Light and Dark force an explicit mode. FollowSystem resolves through the
 * current platform appearance when Qt exposes it, and otherwise falls back to
 * a palette-luminance heuristic.
 */
enum class ThemeModePreference {
    Light,
    Dark,
    FollowSystem
};

struct QTMATERIAL3_THEME_EXPORT SystemThemeSnapshot {
    ThemeModePreference preference = ThemeModePreference::FollowSystem;
    ThemeMode effectiveMode = ThemeMode::Light;
    ContrastMode effectiveContrast = ContrastMode::Standard;
    bool highContrast = false;
    bool hasNativeColorScheme = false;
    QString platformFontFamily;
};

/**
 * Optional bridge between platform appearance and ThemeManager.
 *
 * The class is intentionally optional: applications can use ThemeManager
 * without it, or enable this bridge when they want a Material theme to follow
 * operating-system light/dark and contrast changes.
 */
class QTMATERIAL3_THEME_EXPORT SystemTheme : public QObject {
    Q_OBJECT
public:
    static SystemTheme& instance();

    ThemeModePreference preference() const noexcept;
    void setPreference(ThemeModePreference preference);

    bool autoApplyToThemeManager() const noexcept;
    void setAutoApplyToThemeManager(bool enabled);

    bool usePlatformFont() const noexcept;
    void setUsePlatformFont(bool enabled);

    SystemThemeSnapshot snapshot() const;
    ThemeMode effectiveMode() const;
    ContrastMode effectiveContrast() const;
    bool isHighContrastEnabled() const;
    bool hasNativeColorScheme() const;
    QFont platformFont() const;

    void applyPlatformFontToTheme(Theme& theme) const;

public slots:
    void refresh();
    void applyToThemeManager();

signals:
    void systemThemeChanged();
    void effectiveModeChanged(QtMaterial::ThemeMode mode);
    void highContrastChanged(bool highContrast);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    explicit SystemTheme(QObject* parent = nullptr);
    void connectPlatformSignals();
    void emitDelta(const SystemThemeSnapshot& before, const SystemThemeSnapshot& after);

    ThemeModePreference m_preference = ThemeModePreference::FollowSystem;
    bool m_autoApplyToThemeManager = false;
    bool m_usePlatformFont = true;
    SystemThemeSnapshot m_lastSnapshot;
};

QString QTMATERIAL3_THEME_EXPORT toString(ThemeModePreference preference);
ThemeModePreference QTMATERIAL3_THEME_EXPORT themeModePreferenceFromString(
    const QString& value,
    bool* ok = nullptr);

} // namespace QtMaterial
