#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"
#include <QMouseEvent>
#include <QPainter>
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/core/qtmaterialeventcompat.h"

namespace QtMaterial {
QtMaterialIconButton::QtMaterialIconButton(QWidget* parent) : QtMaterialAbstractButton(parent), m_ripple(new QtMaterialRippleController(this))
{
    setCheckable(true);
}
QtMaterialIconButton::~QtMaterialIconButton() = default;
QIcon QtMaterialIconButton::icon() const { return m_icon; }
void QtMaterialIconButton::setIcon(const QIcon& icon) { m_icon = icon; update(); }
void QtMaterialIconButton::themeChangedEvent(const Theme& theme) { QtMaterialAbstractButton::themeChangedEvent(theme); m_specDirty = true; }
void QtMaterialIconButton::invalidateResolvedSpec() { m_specDirty = true; }
void QtMaterialIconButton::resolveSpecIfNeeded() const
{
    if (!m_specDirty) return;
    SpecFactory factory;
    m_spec = factory.iconButtonSpec(theme(), density());
    m_specDirty = false;
}
QSize QtMaterialIconButton::sizeHint() const
{
    const_cast<QtMaterialIconButton*>(this)->resolveSpecIfNeeded();
    return m_spec.touchTarget;
}
void QtMaterialIconButton::mousePressEvent(QMouseEvent* event)
{
    if (m_ripple) m_ripple->addRipple(QtMaterial::mousePosition(event));
        QtMaterialAbstractButton::mousePressEvent(event);
}
void QtMaterialIconButton::paintEvent(QPaintEvent*)
{
    resolveSpecIfNeeded();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const QRectF bounds = rect().adjusted(4, 4, -4, -4);
    const qreal radius = bounds.width() / 2.0;
    const bool selected = isChecked();
    const QColor container = selected ? m_spec.selectedContainerColor : m_spec.containerColor;
    const QColor iconColor = selected ? m_spec.selectedIconColor : m_spec.iconColor;
    painter.setPen(Qt::NoPen);
    painter.setBrush(container);
    painter.drawRoundedRect(bounds, radius, radius);
    if (interactionState().isHovered()) {
        QtMaterialStateLayerPainter::paintRect(&painter, bounds, m_spec.stateLayerColor, 0.08, radius);
    }
    if (m_ripple) {
        m_ripple->setClipRect(bounds, radius);
        m_ripple->paint(&painter, m_spec.stateLayerColor);
    }
    QPixmap pix = m_icon.pixmap(QSize(m_spec.iconSize, m_spec.iconSize));
    QRect iconRect((width() - m_spec.iconSize) / 2, (height() - m_spec.iconSize) / 2, m_spec.iconSize, m_spec.iconSize);
    if (!pix.isNull()) {
        painter.drawPixmap(iconRect, pix);
    } else {
        painter.setPen(iconColor);
        painter.drawEllipse(iconRect.adjusted(2, 2, -2, -2));
    }
    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(&painter, bounds, m_spec.stateLayerColor, radius, 2.0);
    }
}
} // namespace QtMaterial
