#pragma once
#include <QFont>
#include <QObject>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialthemeoptions.h"
namespace QtMaterial {
class Theme;
struct QTMATERIAL3_THEME_RUNTIME_EXPORT SystemThemeSnapshot {
    ThemePreference preference = ThemePreference::FollowSystem;
    ThemeMode effectiveMode = ThemeMode::Light;
    ContrastMode effectiveContrast = ContrastMode::Standard;
    bool highContrast = false;
    bool hasNativeColorScheme = false;
    QString platformFontFamily;
};
class QTMATERIAL3_THEME_RUNTIME_EXPORT SystemTheme final : public QObject {
    Q_OBJECT
public:
    static SystemTheme& instance();

    // Pure policy helpers used by applications and deterministic tests. The
    // platform probe supplies systemMode/highContrast; these helpers resolve
    // the user-facing preference without depending on the host CI machine.
    static ThemeMode resolveMode(
        ThemePreference preference,
        ThemeMode systemMode) noexcept;
    static ContrastMode resolveContrast(bool highContrast) noexcept;
    ThemePreference preference() const noexcept;
    void setPreference(ThemePreference preference);
    bool autoApplyToThemeManager() const noexcept;
    void setAutoApplyToThemeManager(bool enabled);
    bool usePlatformFont() const noexcept;
    void setUsePlatformFont(bool enabled);
    SystemThemeSnapshot snapshot() const;
    ThemeMode effectiveMode() const;
    ContrastMode effectiveContrast() const;
    bool isHighContrastEnabled() const;
    bool isHighContrastAvailable() const;
    bool isReducedMotionEnabled() const;
    bool hasNativeColorScheme() const;
    QFont platformFont() const;
    void applyPlatformFontToTheme(Theme& theme) const;
    void applyAccessibilityPreferencesToTheme(Theme& theme) const;
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
    void emitDelta(const SystemThemeSnapshot& before,
                   const SystemThemeSnapshot& after);
    ThemePreference m_preference = ThemePreference::FollowSystem;
    bool m_autoApplyToThemeManager = false;
    bool m_usePlatformFont = true;
    SystemThemeSnapshot m_lastSnapshot;
};
} // namespace QtMaterial
