#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
#include <QMouseEvent>
#include <QPainter>
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialripplecontroller.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/core/qtmaterialeventcompat.h"
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
    m_specDirty = false;
}
void QtMaterialCheckbox::mousePressEvent(QMouseEvent* event)
{
    if (m_ripple) m_ripple->addRipple(QtMaterial::mousePosition(event));
    QtMaterialSelectionControl::mousePressEvent(event);
}
void QtMaterialCheckbox::paintEvent(QPaintEvent*)
{
    resolveSpecIfNeeded();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const QRect box = indicatorRect();
    painter.setPen(QPen(isChecked() ? m_spec.selectedContainerColor : m_spec.unselectedOutlineColor, m_spec.outlineWidth));
    painter.setBrush(isChecked() ? m_spec.selectedContainerColor : Qt::NoBrush);
    painter.drawRoundedRect(box, 4, 4);
    if (isChecked()) {
        painter.setPen(QPen(m_spec.selectedIconColor, 2));
        painter.drawLine(box.left() + 4, box.center().y(), box.center().x(), box.bottom() - 4);
        painter.drawLine(box.center().x(), box.bottom() - 4, box.right() - 4, box.top() + 4);
    }
    painter.setPen(m_spec.labelColor);
    painter.drawText(labelRect(), labelAlignment(), text());
    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(&painter, box.adjusted(-3, -3, 3, 3), m_spec.focusRingColor, 6, 2.0);
    }
}
} // namespace QtMaterial
