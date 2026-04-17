#include "qtmaterial/widgets/data/qtmaterialdivider.h"
#include <QPainter>
namespace QtMaterial {
QtMaterialDivider::QtMaterialDivider(QWidget* parent) : QWidget(parent) { setMinimumHeight(1); }
QtMaterialDivider::~QtMaterialDivider() = default;
void QtMaterialDivider::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(QRect(0, 0, width(), 1), palette().mid());
}
} // namespace QtMaterial
