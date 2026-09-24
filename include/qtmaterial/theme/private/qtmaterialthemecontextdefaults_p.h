#pragma once
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
class ThemeContext;
class QTMATERIAL3_THEME_RUNTIME_EXPORT ThemeContextDefaults final {
public:
    ThemeContextDefaults() = delete;
    static ThemeContext* defaultContext() noexcept;
};
} // namespace QtMaterial
