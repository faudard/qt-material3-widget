#include "qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h"
#include <QPainter>
namespace QtMaterial {
QtMaterialNavigationDrawer::QtMaterialNavigationDrawer(QWidget* parent) : QtMaterialOverlaySurface(parent) { setMinimumWidth(280); }
QtMaterialNavigationDrawer::~QtMaterialNavigationDrawer() = default;
void QtMaterialNavigationDrawer::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), palette().window());
    painter.setPen(palette().windowText().color());
    painter.drawText(rect().adjusted(16,16,-16,-16), Qt::AlignTop | Qt::AlignLeft, QStringLiteral("Navigation drawer"));
}
} // namespace QtMaterial
