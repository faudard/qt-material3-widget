#pragma once

#include <QObject>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

namespace QtMaterial {

class QTMATERIAL3_THEME_EXPORT ThemeManager : public QObject
{
    Q_OBJECT

public:
    static ThemeManager& instance();

    const Theme& theme() const noexcept;
    const ThemeOptions& options() const noexcept;

    void setTheme(const Theme& theme);
    void setThemeOptions(const ThemeOptions& options);
    void rebuildTheme();

signals:
    void themeChanged(const QtMaterial::Theme& theme);

private:
    explicit ThemeManager(QObject* parent = nullptr);

    ThemeOptions m_options;
    Theme m_theme;
    ThemeBuilder m_builder;
};

} // namespace QtMaterial
