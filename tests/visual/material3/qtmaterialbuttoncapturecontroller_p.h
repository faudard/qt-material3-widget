#pragma once

#include <QFont>
#include <QImage>
#include <QPointF>
#include <QRect>
#include <QString>

#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial::VisualCapturePrivate {

struct ButtonCaptureState
{
    bool enabled = true;
    bool hovered = false;
    bool focused = false;
    bool pressed = false;
    qreal stateLayerProgress = 0.0;
    QString feedbackKind;
    qreal feedbackProgress = 0.0;
    QPointF feedbackOrigin;
};

struct ButtonCaptureResult
{
    QImage image;
    QFont resolvedLabelFont;
};

class ButtonCaptureController final
{
public:
    bool capture(
        const QString& componentId,
        const ButtonCaptureState& state,
        const Theme& theme,
        const QString& label,
        const QSize& canvasSize,
        const QRect& widgetRect,
        const QColor& background,
        ButtonCaptureResult* result,
        QString* errorMessage) const;
};

} // namespace QtMaterial::VisualCapturePrivate
