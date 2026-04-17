#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"
#include <QPainter>
namespace QtMaterial {
QtMaterialCard::QtMaterialCard(QWidget* parent) : QtMaterialSurface(parent) { setMinimumSize(160, 100); }
QtMaterialCard::~QtMaterialCard() = default;
void QtMaterialCard::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const QRectF r = rect().adjusted(1,1,-1,-1);
    painter.setBrush(palette().base());
    painter.setPen(QPen(palette().mid().color(), 1.0));
    painter.drawRoundedRect(r, 16.0, 16.0);
    painter.setPen(palette().windowText().color());
    painter.drawText(r.adjusted(16,16,-16,-16), Qt::AlignLeft | Qt::AlignTop, QStringLiteral("Card"));
}
} // namespace QtMaterial
