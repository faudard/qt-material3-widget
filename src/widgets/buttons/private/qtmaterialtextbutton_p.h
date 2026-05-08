#pragma once

#include <QObject>

#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"

namespace QtMaterial {

class QtMaterialTextButtonPrivate {
public:
 explicit QtMaterialTextButtonPrivate(QObject* parent)
  : ripple(new QtMaterialRippleController(parent))
  , stateLayerTransition(new QtMaterialTransitionController(parent))
 {}

 mutable bool specDirty = true;
 mutable ButtonSpec spec;
 QtMaterialRippleController* ripple = nullptr;
 QtMaterialTransitionController* stateLayerTransition = nullptr;
};

} // namespace QtMaterial
