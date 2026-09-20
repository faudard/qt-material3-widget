#pragma once

#include <QColor>
#include <QPainterPath>
#include <QRectF>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/foundation/qtmaterialinteractionstate.h"
#include "qtmaterial/theme/qtmaterialaccessibilitytokens.h"
#include "qtmaterial/theme/qtmaterialstatelayer.h"

class QPainter;

namespace QtMaterial {

class QTMATERIAL3_EFFECTS_EXPORT QtMaterialStateLayerPainter
{
public:
    static qreal opacityForState(
        const QtMaterialInteractionState& state,
        const StateLayer& layer,
        const InteractionStateTokens& policy) noexcept;

    static void paintRect(
        QPainter* painter,
        const QRectF& rect,
        const QColor& color,
        qreal opacity,
        qreal radius);

    static void paintPath(
        QPainter* painter,
        const QPainterPath& path,
        const QColor& color,
        qreal opacity);

    static void paintRectForState(
        QPainter* painter,
        const QRectF& rect,
        qreal radius,
        const QtMaterialInteractionState& state,
        const StateLayer& layer,
        const InteractionStateTokens& policy);

    static void paintPathForState(
        QPainter* painter,
        const QPainterPath& path,
        const QtMaterialInteractionState& state,
        const StateLayer& layer,
        const InteractionStateTokens& policy);
};

} // namespace QtMaterial
