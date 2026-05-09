#pragma once
#include <QtGlobal>
#include <QColor>

#include "private/qtmaterialbuttonlayoutcache_p.h"

namespace QtMaterial {

class QtMaterialOutlinedButton;

class QtMaterialOutlinedButtonPrivate {
public:
 void invalidateLayout(QtMaterialOutlinedButton& button);
 void ensureLayoutResolved(const QtMaterialOutlinedButton& button) const;
    qreal resolvedOutlineStrokeWidth(const QtMaterialOutlinedButton& button) const;
    QColor resolvedOutlineColor(const QtMaterialOutlinedButton& button) const;
    bool shouldPaintOutline(const QtMaterialOutlinedButton& button) const;

 mutable QtMaterialButtonLayoutCache layout;
};

} // namespace QtMaterial
