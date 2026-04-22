#pragma once

#include <QColor>

#include "qtmaterial/core/qtmaterialinteractionstate.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial::ButtonMotionHelper {

inline qreal targetStateLayerOpacity(const Theme& theme,
                                     const QtMaterialInteractionState& state)
{
    if (!state.isEnabled()) {
        return 0.0;
    }

    const auto& layer = theme.stateLayer();

    if (state.isPressed()) {
        return layer.pressOpacity;
    }
    if (state.isFocused()) {
        return layer.focusOpacity;
    }
    if (state.isHovered()) {
        return layer.hoverOpacity;
    }

    return 0.0;
}

template <typename SpecT>
inline void configureMotion(const Theme& theme,
                            const SpecT& spec,
                            QtMaterialTransitionController* stateLayerTransition,
                            QtMaterialRippleController* ripple)
{
    if (theme.motion().contains(spec.motionToken)) {
        const MotionStyle motion = theme.motion().style(spec.motionToken);

        if (stateLayerTransition) {
            if (motion.durationMs > 0) {
                stateLayerTransition->setDuration(motion.durationMs);
            }
            stateLayerTransition->setEasingCurve(motion.easing);
        }

        if (ripple && motion.durationMs > 0) {
            ripple->setDuration(motion.durationMs);
        }
    }

    if (ripple) {
        ripple->setBaseOpacity(theme.stateLayer().pressOpacity);
    }
}

inline void syncStateLayerTransition(const Theme& theme,
                                     const QtMaterialInteractionState& state,
                                     QtMaterialTransitionController* transition)
{
    if (!transition) {
        return;
    }

    transition->startTo(targetStateLayerOpacity(theme, state));
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