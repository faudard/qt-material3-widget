#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"
#include <QPainter>
namespace QtMaterial {
QtMaterialBanner::QtMaterialBanner(QWidget* parent) : QtMaterialSurface(parent) {}
QtMaterialBanner::~QtMaterialBanner() = default;
void QtMaterialBanner::setHeadlineText(const QString& text) { m_headlineText = text; update(); }
void QtMaterialBanner::setSupportingText(const QString& text) { m_supportingText = text; update(); }
void QtMaterialBanner::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), palette().alternateBase());
    painter.setPen(palette().windowText().color());
    painter.drawText(QRect(16,12,width()-32,24), Qt::AlignLeft | Qt::AlignVCenter, m_headlineText.isEmpty() ? QStringLiteral("Banner") : m_headlineText);
    painter.setPen(palette().mid().color());
    painter.drawText(QRect(16,40,width()-32,height()-52), Qt::AlignLeft | Qt::TextWordWrap, m_supportingText.isEmpty() ? QStringLiteral("Supporting text placeholder") : m_supportingText);
}
} // namespace QtMaterial
