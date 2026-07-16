#pragma once

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include <QRect>
#include <QRectF>
#include <QString>

#include "qtmaterial/foundation/qtmaterialinteractionstate.h"
#include "qtmaterial/specs/qtmaterialcheckboxspec.h"
#include "qtmaterial/specs/qtmaterialradiobuttonspec.h"
#include "qtmaterial/specs/qtmaterialswitchspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/specs/qtmaterialselectionruntimespec.h"

namespace QtMaterial {
class QtMaterialRippleController;
class QtMaterialTransitionController;
} // namespace QtMaterial

namespace QtMaterial::SelectionRenderHelper {


qreal stateLayerOpacity(
    const SelectionRuntimeSpec& spec,
    const QtMaterialInteractionState& state);

QFont resolvedLabelFont(
    const QFont& fallback,
    const SelectionRuntimeSpec& spec);

void configureMotion(
    const SelectionRuntimeSpec& spec,
    QtMaterialTransitionController* transition,
    QtMaterialRippleController* ripple);


qreal stateLayerOpacity(const Theme& theme, const QtMaterialInteractionState& state);
QFont labelFont(const Theme& theme, TypeRole role, const QFont& fallback);
QRectF centeredStateLayerRect(const QRect& bounds, int stateLayerSize);
void paintCircularStateLayer(QPainter* painter, const QRectF& rect, const QColor& color, qreal opacity);
void paintLabel(QPainter* painter, const QRect& rect, Qt::Alignment alignment, const QString& text, const QColor& color, const QFont& font);
QRectF switchTrackRect(const QRect& widgetRect, const SwitchSpec& spec);
QRectF switchHandleRect(const QRectF& trackRect, const SwitchSpec& spec, qreal progress);
QPainterPath roundedTrackPath(const QRectF& trackRect);

} // namespace QtMaterial::SelectionRenderHelper
