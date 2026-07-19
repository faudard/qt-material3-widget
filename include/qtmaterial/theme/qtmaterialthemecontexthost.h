#pragma once

#include <QtPlugin>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class ThemeContext;

/**
 * Contract implemented by widgets that participate in ThemeContext
 * inheritance without requiring QtMaterialWidget as their C++ base class.
 */
class QTMATERIAL3_THEME_EXPORT ThemeContextHost
{
public:
    virtual ~ThemeContextHost() = default;

    virtual ThemeContext* themeContext() const noexcept = 0;
    virtual ThemeContext* effectiveThemeContext() const noexcept = 0;
};

} // namespace QtMaterial

Q_DECLARE_INTERFACE(
    QtMaterial::ThemeContextHost,
    "org.qtmaterial3.ThemeContextHost/1.0")
