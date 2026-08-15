#pragma once
#include <QColor>
#include <QMetaType>
#include <QObject>
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
namespace QtMaterial {
enum class ThemeChangeReason {
    SetTheme,
    SetOptions,
    Rebuild,
    SeedColor,
    SystemAppearance,
    External
};
class QTMATERIAL3_THEME_RUNTIME_EXPORT ThemeManager final : public QObject {
    Q_OBJECT
public:
    static ThemeManager& instance();
    ThemeContext* defaultContext() noexcept;
    const ThemeContext* defaultContext() const noexcept;
    const Theme& theme() const noexcept;
    const ThemeOptions& options() const noexcept;
    quint64 revision() const noexcept;
    bool isApplyingTheme() const noexcept;
    bool setTheme(const Theme& theme,
                  ThemeChangeReason reason = ThemeChangeReason::SetTheme);
    bool setThemeOptions(const ThemeOptions& options);
    bool rebuildTheme(ThemeChangeReason reason = ThemeChangeReason::Rebuild);
    bool applySeedColor(const QColor& seed);
    bool applySeedColor(const QColor& seed, ThemeMode mode);
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
    ThemeOptions m_options;
    ThemeBuilder m_builder;
    ThemeContext* m_defaultContext = nullptr;
    bool m_applyingTheme = false;
};
} // namespace QtMaterial
Q_DECLARE_METATYPE(QtMaterial::ThemeChangeReason)
