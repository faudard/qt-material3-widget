#pragma once

#include <QPainterPath>
#include <QRect>

class QPainter;

namespace QtMaterial {
struct ListSpec;
class Theme;
}

class QtMaterialListRenderHelper
{
public:
    static qreal cornerRadius(const QtMaterial::ListSpec& spec, const QtMaterial::Theme& theme);
    static QPainterPath containerPath(const QRect& rect, qreal radius);
    static void paintContainer(QPainter* painter,
                               const QRect& rect,
                               const QtMaterial::ListSpec& spec,
                               const QtMaterial::Theme& theme);
    static void paintDividers(QPainter* painter,
                              const QList<QRect>& itemRects,
                              const QtMaterial::ListSpec& spec);
};
