#pragma once

#include <QColor>

#include "qtmaterial/foundation/qtmaterialinteractionstate.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"

namespace QtMaterial::ButtonMotionHelper {

inline qreal targetStateLayerOpacity(
    const ButtonSpec& spec,
    const QtMaterialInteractionState& state)
{
    if (!state.isEnabled()) {
        return 0.0;
    }
    if (state.isPressed()) {
        // Press feedback is owned by the ripple controller. Keeping the fixed
        // layer at zero avoids painting the same Material state twice.
        return 0.0;
    }
    if (state.isFocused()) {
        return spec.focusStateLayerOpacity;
    }
    if (state.isHovered()) {
        return spec.hoverStateLayerOpacity;
    }
    return 0.0;
}


inline void configureTransition(
    const ButtonSpec& spec,
    QtMaterialTransitionController* transition,
    bool reducedMotion = false)
{
    if (!transition || !spec.hasResolvedMotionStyle) {
        return;
    }
    transition->applyMotionStyle(spec.motionStyle);
    transition->setReducedMotion(reducedMotion);
}

inline void configureMotion(
    const ButtonSpec& spec,
    QtMaterialTransitionController* stateLayerTransition,
    QtMaterialRippleController* ripple,
    bool reducedMotion = false)
{
    configureTransition(spec, stateLayerTransition, reducedMotion);

    if (ripple) {
        if (spec.hasResolvedMotionStyle && spec.motionStyle.durationMs > 0) {
            ripple->setDuration(spec.motionStyle.durationMs);
        }
        ripple->setBaseOpacity(spec.pressStateLayerOpacity);
        ripple->setReducedMotion(reducedMotion);
    }
}


inline void syncStateLayerTransition(
    const ButtonSpec& spec,
    const QtMaterialInteractionState& state,
    QtMaterialTransitionController* transition)
{
    if (!transition) {
        return;
    }
    transition->startTo(targetStateLayerOpacity(spec, state));
}


inline QColor blendColor(const QColor& a, const QColor& b, qreal t)
{
    QColor out;
    out.setRedF(a.redF() + (b.redF() - a.redF()) * t);
    out.setGreenF(a.greenF() + (b.greenF() - a.greenF()) * t);
    out.setBlueF(a.blueF() + (b.blueF() - a.blueF()) * t);
    out.setAlphaF(a.alphaF() + (b.alphaF() - a.alphaF()) * t);
    return out;
}

} // namespace QtMaterial::ButtonMotionHelper
