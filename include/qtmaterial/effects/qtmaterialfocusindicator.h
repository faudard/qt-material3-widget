#pragma once

#include <QColor>
#include <QPainterPath>
#include <QRectF>
#include <Qt>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/foundation/qtmaterialinteractionstate.h"
#include "qtmaterial/theme/qtmaterialaccessibilitytokens.h"

class QPainter;

namespace QtMaterial {

class QTMATERIAL3_EFFECTS_EXPORT QtMaterialFocusIndicator
{
public:
    static bool shouldShow(
        const QtMaterialInteractionState& state,
        Qt::FocusReason reason,
        const InteractionStateTokens& policy) noexcept;

    static void paintRectFocusRing(
        QPainter* painter,
        const QRectF& rect,
        const QColor& color,
        qreal radius,
        qreal width);

    static void paintPathFocusRing(
        QPainter* painter,
        const QPainterPath& path,
        const QColor& color,
        qreal width);

    static void paintRectFocusRing(
        QPainter* painter,
        const QRectF& rect,
        qreal radius,
        const FocusRingTokens& tokens);

    static void paintPathFocusRing(
        QPainter* painter,
        const QPainterPath& path,
        const FocusRingTokens& tokens);
};

} // namespace QtMaterial
