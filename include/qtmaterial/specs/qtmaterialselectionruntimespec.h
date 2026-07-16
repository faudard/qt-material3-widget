#pragma once

#include <QFont>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialmotiontokens.h"

namespace QtMaterial {

/**
 * Concrete runtime values shared by selection-control specifications.
 *
 * Role/token identifiers remain in the concrete specs for serialization and
 * override lookup. Widgets consume these resolved values after the resolver
 * boundary and no longer need to query Theme during layout or painting.
 */
struct QTMATERIAL3_SPECS_EXPORT SelectionRuntimeSpec
{
    QFont labelFont;
    MotionStyle motionStyle;

    qreal hoverStateLayerOpacity = 0.08;
    qreal focusStateLayerOpacity = 0.12;
    qreal pressStateLayerOpacity = 0.12;
    qreal dragStateLayerOpacity = 0.16;

    bool hasResolvedLabelFont = false;
    bool hasResolvedMotionStyle = false;
};

} // namespace QtMaterial
