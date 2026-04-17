#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include <QPainter>
namespace QtMaterial {
QtMaterialFilledTextField::QtMaterialFilledTextField(QWidget* parent) : QtMaterialOutlinedTextField(parent) {}
QtMaterialFilledTextField::~QtMaterialFilledTextField() = default;
void QtMaterialFilledTextField::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect().adjusted(0,0,0,-24), palette().base());
    QtMaterialOutlinedTextField::paintEvent(event);
}
} // namespace QtMaterial
