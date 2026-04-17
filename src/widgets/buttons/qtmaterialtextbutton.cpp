#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
#include <QMouseEvent>
#include <QPainter>
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/core/qtmaterialeventcompat.h"
namespace QtMaterial {
QtMaterialTextButton::QtMaterialTextButton(QWidget* parent) : QtMaterialAbstractButton(parent), m_ripple(new QtMaterialRippleController(this))
{
    setMinimumHeight(40);
}
QtMaterialTextButton::~QtMaterialTextButton() = default;
void QtMaterialTextButton::themeChangedEvent(const Theme& theme) { QtMaterialAbstractButton::themeChangedEvent(theme); m_specDirty = true; }
void QtMaterialTextButton::invalidateResolvedSpec() { m_specDirty = true; }
void QtMaterialTextButton::resolveSpecIfNeeded() const
{
    if (!m_specDirty) return;
    SpecFactory factory;
    m_spec = factory.textButtonSpec(theme(), density());
    m_specDirty = false;
}
QSize QtMaterialTextButton::sizeHint() const
{
    const_cast<QtMaterialTextButton*>(this)->resolveSpecIfNeeded();
    return QSize(qMax(88, fontMetrics().horizontalAdvance(text()) + 2 * m_spec.horizontalPadding), m_spec.touchTarget.height());
}
void QtMaterialTextButton::mousePressEvent(QMouseEvent* event)
{
    if (m_ripple)
    {
        m_ripple->addRipple(QtMaterial::mousePosition(event));
    }
    QtMaterialAbstractButton::mousePressEvent(event);
}
void QtMaterialTextButton::paintEvent(QPaintEvent*)
{
    resolveSpecIfNeeded();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const QRectF bounds = rect().adjusted(1, 1, -1, -1);
    const qreal radius = bounds.height() / 2.0;
    if (interactionState().isPressed()) {
        QtMaterialStateLayerPainter::paintRect(&painter, bounds, m_spec.stateLayerColor, 0.10, radius);
    } else if (interactionState().isHovered()) {
        QtMaterialStateLayerPainter::paintRect(&painter, bounds, m_spec.stateLayerColor, 0.08, radius);
    }
    if (m_ripple) {
        m_ripple->setClipRect(bounds, radius);
        m_ripple->paint(&painter, m_spec.stateLayerColor);
    }
    painter.setPen(isEnabled() ? m_spec.labelColor : m_spec.disabledLabelColor);
    painter.drawText(rect(), Qt::AlignCenter, text());
    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(&painter, bounds, m_spec.stateLayerColor, radius, 2.0);
    }
}
} // namespace QtMaterial
