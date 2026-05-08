#pragma once

#include "private/qtmaterialbuttonlayoutcache_p.h"

namespace QtMaterial {

class QtMaterialOutlinedButton;

class QtMaterialOutlinedButtonPrivate {
public:
 void invalidateLayout(QtMaterialOutlinedButton& button);
 void ensureLayoutResolved(const QtMaterialOutlinedButton& button) const;

 mutable QtMaterialButtonLayoutCache layout;
};

} // namespace QtMaterial
