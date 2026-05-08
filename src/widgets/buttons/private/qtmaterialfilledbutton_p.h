#pragma once

#include <QObject>

#include "private/qtmaterialbuttonlayoutcache_p.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"

namespace QtMaterial {

class QtMaterialFilledButton;

class QtMaterialFilledButtonPrivate {
public:
 explicit QtMaterialFilledButtonPrivate(QObject* parent)
  : elevationTransition(new QtMaterialTransitionController(parent))
 {}

 qreal targetElevationProgress(const QtMaterialFilledButton& button) const noexcept;
 qreal animatedElevationProgress(const QtMaterialFilledButton& button) const noexcept;
 void invalidateLayout(QtMaterialFilledButton& button);
 void ensureLayoutResolved(const QtMaterialFilledButton& button) const;

 mutable QtMaterialButtonLayoutCache layout;
 QtMaterialTransitionController* elevationTransition = nullptr;
};

} // namespace QtMaterial
