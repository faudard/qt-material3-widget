#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"
#include <QPainter>
namespace QtMaterial {
QtMaterialBottomSheet::QtMaterialBottomSheet(QWidget* parent) : QtMaterialOverlaySurface(parent) { setMinimumHeight(240); }
QtMaterialBottomSheet::~QtMaterialBottomSheet() = default;
void QtMaterialBottomSheet::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), palette().window());
    painter.setPen(palette().windowText().color());
    painter.drawText(rect().adjusted(16,16,-16,-16), Qt::AlignTop | Qt::AlignLeft, QStringLiteral("Bottom sheet"));
}
} // namespace QtMaterial
