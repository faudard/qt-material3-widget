#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"

#include <QPainter>

#include "private/qtmaterialbuttonrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {

QtMaterialOutlinedButton::QtMaterialOutlinedButton(QWidget* parent)
    : QtMaterialTextButton(parent)
{
}

QtMaterialOutlinedButton::~QtMaterialOutlinedButton() = default;

ButtonSpec QtMaterialOutlinedButton::resolveButtonSpec() const
{
    SpecFactory factory;
    return factory.outlinedButtonSpec(theme(), density());
}

void QtMaterialOutlinedButton::paintEvent(QPaintEvent*)
{
    ensurePolished();
    ensureSpecResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF visualRect = ButtonRenderHelper::containerRect(rect(), m_spec).adjusted(1, 1, -1, -1);
    const QPainterPath path = ButtonRenderHelper::containerPath(theme(), m_spec, visualRect);
    const qreal radius = ButtonRenderHelper::cornerRadius(theme(), m_spec, visualRect);

    painter.save();
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(isEnabled() ? m_spec.outlineColor : m_spec.disabledOutlineColor, 1.0));
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
