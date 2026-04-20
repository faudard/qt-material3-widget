#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "private/qtmaterialselectionrenderhelper_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/core/qtmaterialeventcompat.h"


namespace QtMaterial {
QtMaterialRadioButton::QtMaterialRadioButton(QWidget* parent) : QtMaterialSelectionControl(parent), m_ripple(new QtMaterialRippleController(this))
{
    setAutoExclusive(true);
}
QtMaterialRadioButton::~QtMaterialRadioButton() = default;
void QtMaterialRadioButton::themeChangedEvent(const Theme& theme) { QtMaterialSelectionControl::themeChangedEvent(theme); m_specDirty = true; }
void QtMaterialRadioButton::invalidateResolvedSpec() { m_specDirty = true; }
void QtMaterialRadioButton::resolveSpecIfNeeded() const
{
    if (!m_specDirty) return;
    SpecFactory factory;
    m_spec = factory.radioButtonSpec(theme(), density());
    const_cast<QtMaterialRadioButton*>(this)->setSpacing(m_spec.spacing);
    m_specDirty = false;
}
void QtMaterialRadioButton::mousePressEvent(QMouseEvent* event)
{
    if (m_ripple) {
        m_ripple->addRipple(QtMaterial::mousePosition(event));
    }
    QtMaterialSelectionControl::mousePressEvent(event);
}
void QtMaterialRadioButton::paintEvent(QPaintEvent*)
{
    resolveSpecIfNeeded();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRect indicator = indicatorRect();
    const QRectF stateLayerRect = SelectionRenderHelper::centeredStateLayerRect(indicator, m_spec.stateLayerSize);
    const qreal opacity = SelectionRenderHelper::stateLayerOpacity(theme(), interactionState());
    SelectionRenderHelper::paintCircularStateLayer(&painter, stateLayerRect, m_spec.stateLayerColor, opacity);

    QPainterPath rippleClip;
    rippleClip.addEllipse(stateLayerRect);
    if (m_ripple) {
        m_ripple->setClipPath(rippleClip);
        m_ripple->paint(&painter, m_spec.stateLayerColor);
    }

    const bool enabled = isEnabled();
    const QColor ringColor = isChecked()
        ? (enabled ? m_spec.selectedColor : m_spec.disabledColor)
        : (enabled ? m_spec.unselectedOutlineColor : m_spec.disabledColor);

    const QRectF outer(indicator);
    painter.save();
    painter.setPen(QPen(ringColor, m_spec.outlineWidth));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(outer);
    if (isChecked()) {
        const qreal dotInset = (outer.width() - m_spec.dotSize) / 2.0;
        painter.setPen(Qt::NoPen);
        painter.setBrush(ringColor);
        painter.drawEllipse(outer.adjusted(dotInset, dotInset, -dotInset, -dotInset));
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
