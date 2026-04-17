#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include <QPainter>
#include "qtmaterial/theme/qtmaterialcolortoken.h"
namespace QtMaterial {
QtMaterialScrimWidget::QtMaterialScrimWidget(QWidget* parent) : QtMaterialWidget(parent), m_scrimColor(QColor(0, 0, 0, 160))
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
}
QtMaterialScrimWidget::~QtMaterialScrimWidget() = default;
QColor QtMaterialScrimWidget::scrimColor() const { return m_scrimColor; }
void QtMaterialScrimWidget::setScrimColor(const QColor& color) { if (m_scrimColor == color) return; m_scrimColor = color; update(); }
void QtMaterialScrimWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), m_scrimColor);
}
} // namespace QtMaterial
