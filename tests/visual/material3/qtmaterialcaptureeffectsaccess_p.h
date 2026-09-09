#pragma once

#include <QVariantAnimation>
#include <QtGlobal>

#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"

namespace QtMaterial {

// This header belongs exclusively to the non-installed visual capture target.
class QtMaterialTransitionControllerCaptureAccess final
{
public:
    static void freeze(
        QtMaterialTransitionController& controller,
        qreal progress)
    {
        if (controller.m_animation) {
            controller.m_animation->stop();
        }
        controller.setProgress(qBound<qreal>(0.0, progress, 1.0));
    }
};

class QtMaterialRippleControllerCaptureAccess final
{
public:
    static void freeze(
        QtMaterialRippleController& controller,
        const QPointF& center,
        qreal progress)
    {
        controller.clear();

        QtMaterialRippleController::Ripple ripple;
        ripple.center = center;
        ripple.endRadius = controller.targetRadiusFor(center);
        ripple.startedMs = 0;
        ripple.fixedProgress = qBound<qreal>(0.0, progress, 1.0);
        controller.m_ripples.push_back(ripple);

        if (controller.m_timer) {
            controller.m_timer->stop();
        }
    }
};

} // namespace QtMaterial
