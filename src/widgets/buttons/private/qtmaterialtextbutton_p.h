#pragma once

#include <QObject>

#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/effects/private/qtmaterialshapemorph_p.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

class QWidget;

namespace QtMaterial {

class QtMaterialTextButtonPrivate {
public:
 explicit QtMaterialTextButtonPrivate(QWidget* parent)
  : ripple(new QtMaterialRippleController(parent))
  , stateLayerTransition(new QtMaterialTransitionController(parent))
  , shapeTransition(new QtMaterialTransitionController(parent))
 {}

 mutable bool specDirty = true;
 mutable ButtonSpec spec;
 QtMaterialRippleController* ripple = nullptr;
 QtMaterialTransitionController* stateLayerTransition = nullptr;
 QtMaterialTransitionController* shapeTransition = nullptr;
 bool expressive = false;
 QtMaterialButtonSize expressiveSize = QtMaterialButtonSize::Small;
 QtMaterialButtonShape expressiveShape = QtMaterialButtonShape::Round;

 mutable QtMaterialShapeMorph shapeMorph;
 mutable QRectF morphBounds;
 mutable qreal morphSourceRadius = -2.0;
 mutable qreal morphTargetRadius = -2.0;
};

} // namespace QtMaterial
