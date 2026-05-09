#pragma once

#include <QPainterPath>
#include <QRect>
#include <QString>
#include <QtGlobal>

namespace QtMaterial {

struct QtMaterialButtonLayoutCache {
 bool dirty = true;

 QRect visualRect;
 QPainterPath containerPath;
 qreal cornerRadius = 0.0;

 QRect iconRect;
 QRect textRect;
 QString elidedText;
 bool hasIcon = false;
};

} // namespace QtMaterial
