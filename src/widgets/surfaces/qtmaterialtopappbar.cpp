#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"
#include <QPainter>
namespace QtMaterial {
QtMaterialTopAppBar::QtMaterialTopAppBar(QWidget* parent) : QtMaterialSurface(parent), m_title(QStringLiteral("Top app bar")) { setMinimumHeight(64); }
QtMaterialTopAppBar::~QtMaterialTopAppBar() = default;
void QtMaterialTopAppBar::setTitle(const QString& title) { m_title = title; update(); }
void QtMaterialTopAppBar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), palette().window());
    painter.setPen(palette().windowText().color());
    painter.drawText(rect().adjusted(16,0,-16,0), Qt::AlignLeft | Qt::AlignVCenter, m_title);
}
} // namespace QtMaterial
