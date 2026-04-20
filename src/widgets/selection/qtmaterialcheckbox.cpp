#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"

#include <QMouseEvent>
#include "qtmaterial/core/qtmaterialeventcompat.h"
#include <QPainter>
#include <QPainterPath>

#include "private/qtmaterialselectionrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {
QtMaterialCheckbox::QtMaterialCheckbox(QWidget* parent) : QtMaterialSelectionControl(parent), m_ripple(new QtMaterialRippleController(this)) {}
QtMaterialCheckbox::~QtMaterialCheckbox() = default;
void QtMaterialCheckbox::themeChangedEvent(const Theme& theme) { QtMaterialSelectionControl::themeChangedEvent(theme); m_specDirty = true; }
void QtMaterialCheckbox::invalidateResolvedSpec() { m_specDirty = true; }
void QtMaterialCheckbox::resolveSpecIfNeeded() const
{
    if (!m_specDirty) return;
    SpecFactory factory;
    m_spec = factory.checkboxSpec(theme(), density());
    const_cast<QtMaterialCheckbox*>(this)->setSpacing(m_spec.spacing);
    m_specDirty = false;
}
void QtMaterialCheckbox::mousePressEvent(QMouseEvent* event)
{
    if (m_ripple) {
        m_ripple->addRipple(QtMaterial::mousePosition(event));
    }
    QtMaterialSelectionControl::mousePressEvent(event);
}
void QtMaterialCheckbox::paintEvent(QPaintEvent*)
{
    resolveSpecIfNeeded();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRect box = indicatorRect();
    const QRectF stateLayerRect = SelectionRenderHelper::centeredStateLayerRect(box, m_spec.stateLayerSize);
    const qreal opacity = SelectionRenderHelper::stateLayerOpacity(theme(), interactionState());
    SelectionRenderHelper::paintCircularStateLayer(&painter, stateLayerRect, m_spec.stateLayerColor, opacity);

    QPainterPath rippleClip;
    rippleClip.addEllipse(stateLayerRect);
    if (m_ripple) {
        m_ripple->setClipPath(rippleClip);
        m_ripple->paint(&painter, m_spec.stateLayerColor);
    }

    const bool enabled = isEnabled();
    const bool checked = isChecked();
    const QColor outlineColor = checked
        ? (enabled ? m_spec.selectedContainerColor : m_spec.disabledSelectedContainerColor)
        : (enabled ? m_spec.unselectedOutlineColor : m_spec.disabledUnselectedOutlineColor);
    const QColor fillColor = checked
        ? (enabled ? m_spec.selectedContainerColor : m_spec.disabledSelectedContainerColor)
        : Qt::transparent;

    painter.save();
    painter.setPen(QPen(outlineColor, m_spec.outlineWidth));
    painter.setBrush(fillColor);
    painter.drawRoundedRect(QRectF(box), m_spec.cornerRadius, m_spec.cornerRadius);
    if (checked) {
        painter.setPen(QPen(m_spec.selectedIconColor, m_spec.checkmarkStrokeWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        const QPointF p1(box.left() + box.width() * 0.22, box.top() + box.height() * 0.55);
        const QPointF p2(box.left() + box.width() * 0.45, box.bottom() - box.height() * 0.22);
        const QPointF p3(box.right() - box.width() * 0.18, box.top() + box.height() * 0.22);
        painter.drawLine(p1, p2);
        painter.drawLine(p2, p3);
    }
    painter.restore();

    const QFont labelFont = SelectionRenderHelper::labelFont(theme(), m_spec.labelTypeRole, font());
    const QColor labelColor = enabled ? m_spec.labelColor : m_spec.disabledLabelColor;
    SelectionRenderHelper::paintLabel(&painter, labelRect(), labelAlignment(), text(), labelColor, labelFont);

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(&painter, stateLayerRect, m_spec.focusRingColor, stateLayerRect.height() / 2.0, 2.0);
    }
}
} // namespace QtMaterial
