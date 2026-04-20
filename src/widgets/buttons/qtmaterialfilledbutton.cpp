#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

#include <QPainter>

#include "private/qtmaterialbuttonrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialFilledButton::QtMaterialFilledButton(QWidget* parent)
    : QtMaterialTextButton(parent)
{
}

QtMaterialFilledButton::~QtMaterialFilledButton() = default;

ButtonSpec QtMaterialFilledButton::resolveButtonSpec() const
{
    SpecFactory factory;
    return factory.filledButtonSpec(theme(), density());
}

void QtMaterialFilledButton::paintEvent(QPaintEvent*)
{
    ensurePolished();
    ensureSpecResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF visualRect = ButtonRenderHelper::containerRect(rect(), m_spec).adjusted(1, 1, -1, -1);
    const QPainterPath path = ButtonRenderHelper::containerPath(theme(), m_spec, visualRect);
    const qreal radius = ButtonRenderHelper::cornerRadius(theme(), m_spec, visualRect);

    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(isEnabled() ? m_spec.containerColor : m_spec.disabledContainerColor);
    painter.drawPath(path);
    painter.restore();

    const qreal layerOpacity = ButtonRenderHelper::stateLayerOpacity(theme(), interactionState());
    if (layerOpacity > 0.0) {
        QtMaterialStateLayerPainter::paintPath(&painter, path, m_spec.stateLayerColor, layerOpacity);
    }

    if (m_ripple) {
        m_ripple->setClipPath(path);
        m_ripple->paint(&painter, m_spec.stateLayerColor);
    }

    QFont resolvedFont = font();
    if (theme().typography().contains(m_spec.labelTypeRole)) {
        resolvedFont = theme().typography().style(m_spec.labelTypeRole).font;
    }
    ButtonRenderHelper::paintContent(&painter, this, m_spec, visualRect.toAlignedRect(), isEnabled() ? m_spec.labelColor : m_spec.disabledLabelColor, resolvedFont, text());

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(&painter, visualRect, m_spec.stateLayerColor, radius, 2.0);
    }
}

} // namespace QtMaterial
