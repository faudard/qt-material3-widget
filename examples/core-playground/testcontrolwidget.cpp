#include "testcontrolwidget.h"
#include <QPainter>
#include "qtmaterial/specs/qtmaterialspecfactory.h"
TestControlWidget::TestControlWidget(QWidget* parent)
    : QtMaterial::QtMaterialControl(parent), m_spec()
{
    invalidateResolvedSpec();
    setMinimumSize(180, 56);
}
TestControlWidget::~TestControlWidget() = default;
void TestControlWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(m_spec.containerColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect().adjusted(4, 8, -4, -8), 20, 20);
    painter.setPen(m_spec.labelColor);
    painter.drawText(rect(), Qt::AlignCenter, QStringLiteral("Phase 2 test control"));
}
void TestControlWidget::invalidateResolvedSpec()
{
    QtMaterial::SpecFactory factory;
    m_spec = factory.filledButtonSpec(theme(), density());
}
