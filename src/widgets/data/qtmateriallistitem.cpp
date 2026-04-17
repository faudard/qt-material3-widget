#include "qtmaterial/widgets/data/qtmateriallistitem.h"
#include <QPainter>
namespace QtMaterial {
QtMaterialListItem::QtMaterialListItem(QWidget* parent) : QtMaterialControl(parent) { setMinimumHeight(56); }
QtMaterialListItem::~QtMaterialListItem() = default;
QString QtMaterialListItem::headlineText() const { return m_headlineText; }
void QtMaterialListItem::setHeadlineText(const QString& text) { m_headlineText = text; update(); }
QString QtMaterialListItem::supportingText() const { return m_supportingText; }
void QtMaterialListItem::setSupportingText(const QString& text) { m_supportingText = text; update(); }
void QtMaterialListItem::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setPen(palette().windowText().color());
    painter.drawText(QRect(16,10,width()-32,20), Qt::AlignLeft | Qt::AlignVCenter, m_headlineText.isEmpty() ? QStringLiteral("List item") : m_headlineText);
    painter.setPen(palette().mid().color());
    painter.drawText(QRect(16,30,width()-32,18), Qt::AlignLeft | Qt::AlignVCenter, m_supportingText.isEmpty() ? QStringLiteral("Supporting text") : m_supportingText);
}
} // namespace QtMaterial
