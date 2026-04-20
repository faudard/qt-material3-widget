#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"

#include <QPainter>

#include "private/qtmaterialbuttonrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialshadowrenderer.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {
QtMaterialElevatedButton::QtMaterialElevatedButton(QWidget* parent) : QtMaterialFilledButton(parent) {}
QtMaterialElevatedButton::~QtMaterialElevatedButton() = default;
ButtonSpec QtMaterialElevatedButton::resolveButtonSpec() const
{
    SpecFactory factory;
    return factory.elevatedButtonSpec(theme(), density());
}
void QtMaterialElevatedButton::paintEvent(QPaintEvent* event)
{
    ensureSpecResolved();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const QRectF visualRect = ButtonRenderHelper::containerRect(rect(), m_spec).adjusted(1, 1, -1, -1);
    const qreal radius = ButtonRenderHelper::cornerRadius(theme(), m_spec, visualRect);
    QtMaterialShadowRenderer::paintRoundedShadow(&painter, visualRect, radius, theme().colorScheme().color(ColorRole::Shadow), 6, 1);
    QtMaterialFilledButton::paintEvent(event);
}
} // namespace QtMaterial
