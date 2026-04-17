#include "qtmaterial/widgets/surfaces/qtmaterialbottomappbar.h"
#include <QPainter>
namespace QtMaterial {
QtMaterialBottomAppBar::QtMaterialBottomAppBar(QWidget* parent) : QtMaterialSurface(parent) { setMinimumHeight(80); }
QtMaterialBottomAppBar::~QtMaterialBottomAppBar() = default;
void QtMaterialBottomAppBar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), palette().window());
    painter.setPen(palette().windowText().color());
    painter.drawText(rect(), Qt::AlignCenter, QStringLiteral("Bottom app bar"));
}
} // namespace QtMaterial
