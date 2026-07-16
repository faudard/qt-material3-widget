#pragma once

#include <QString>
#include <QStringList>

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/foundation/qtmaterialinteractionstate.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class Theme;

struct QTMATERIAL3_SPECS_EXPORT SpecResolutionContext
{
    const Theme* theme = nullptr;
    Density density = Density::Default;
    QtMaterialInteractionState state;
    QString component;
    QString variant;
    QString role;

    bool isValid() const noexcept { return theme != nullptr; }
};

template <typename SpecT>
struct ResolvedSpec
{
    SpecT spec;
    SpecResolutionContext context;
    QStringList warnings;

    bool isValid() const noexcept { return context.isValid(); }
};

} // namespace QtMaterial
