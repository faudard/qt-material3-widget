#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include <QPainter>
namespace QtMaterial {
QtMaterialOutlinedButton::QtMaterialOutlinedButton(QWidget* parent) : QtMaterialTextButton(parent) {}
QtMaterialOutlinedButton::~QtMaterialOutlinedButton() = default;
void QtMaterialOutlinedButton::paintEvent(QPaintEvent* event)
{
    QtMaterialTextButton::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const QRectF bounds = rect().adjusted(1, 1, -1, -1);
    painter.setPen(QPen(palette().mid().color(), 1.0));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(bounds, bounds.height()/2.0, bounds.height()/2.0);
}
} // namespace QtMaterial
