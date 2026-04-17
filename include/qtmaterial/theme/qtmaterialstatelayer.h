#pragma once
#include <QColor>
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
class QTMATERIAL3_THEME_EXPORT StateLayer {
public:
    StateLayer();
    ~StateLayer();
    QColor color;
    qreal hoverOpacity;
    qreal focusOpacity;
    qreal pressOpacity;
    qreal dragOpacity;
};
} // namespace QtMaterial
