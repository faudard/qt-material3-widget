#pragma once

#include <QPainter>
#include <QPainterPath>
#include <QRectF>
#include <QString>

#include "qtmaterial/core/qtmaterialinteractionstate.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

class QAbstractButton;

namespace QtMaterial::ButtonRenderHelper {

QRectF containerRect(const QRectF& widgetRect, const ButtonSpec& spec);
qreal cornerRadius(const Theme& theme, const ButtonSpec& spec, const QRectF& bounds);
QPainterPath containerPath(const Theme& theme, const ButtonSpec& spec, const QRectF& bounds);
qreal stateLayerOpacity(const Theme& theme, const QtMaterialInteractionState& state);

struct ContentLayout {
    QRect iconRect;
    QRect textRect;
    QString elidedText;
    bool hasIcon = false;
};

ContentLayout layoutContent(
    const QAbstractButton* button,
    const ButtonSpec& spec,
    const QRect& containerRect,
    const QFont& font,
    const QString& text);

void paintContent(
    QPainter* painter,
    const QAbstractButton* button,
    const ButtonSpec& spec,
    const QRect& containerRect,
    const QColor& textColor,
    const QFont& font,
    const QString& text);

} // namespace QtMaterial::ButtonRenderHelper
