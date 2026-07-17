#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialappbarspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_SPECS_EXPORT AppBarSpecResolver final
{
public:
    AppBarSpec topAppBarSpec(const Theme& theme) const;
    AppBarSpec bottomAppBarSpec(const Theme& theme) const;

private:
    static AppBarSpec baseAppBarSpec(const Theme& theme);
    static void resolveRuntimeTokens(
        const Theme& theme,
        AppBarSpec* spec);
};

} // namespace QtMaterial
