#pragma once

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialdialogspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_SPECS_EXPORT DialogSpecResolver final
{
public:
    DialogSpec dialogSpec(const Theme& theme) const;
};

} // namespace QtMaterial
