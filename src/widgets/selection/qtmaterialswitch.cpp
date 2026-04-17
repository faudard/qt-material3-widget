#include "qtmaterial/widgets/selection/qtmaterialswitch.h"
#include <QPainter>
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
namespace QtMaterial {
QtMaterialSwitch::QtMaterialSwitch(QWidget* parent) : QtMaterialSelectionControl(parent), m_transition(new QtMaterialTransitionController(this))
{
    setText(QString());
}
QtMaterialSwitch::~QtMaterialSwitch() = default;
void QtMaterialSwitch::themeChangedEvent(const Theme& theme) { QtMaterialSelectionControl::themeChangedEvent(theme); m_specDirty = true; }
void QtMaterialSwitch::invalidateResolvedSpec() { m_specDirty = true; }
void QtMaterialSwitch::resolveSpecIfNeeded() const
{
    if (!m_specDirty) return;
    SpecFactory factory;
    m_spec = factory.switchSpec(theme(), density());
    m_specDirty = false;
}
QSize QtMaterialSwitch::sizeHint() const
{
    const_cast<QtMaterialSwitch*>(this)->resolveSpecIfNeeded();
    return m_spec.touchTarget;
}
void QtMaterialSwitch::paintEvent(QPaintEvent*)
{
    resolveSpecIfNeeded();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QRectF track((width() - m_spec.trackWidth) / 2.0, (height() - m_spec.trackHeight) / 2.0, m_spec.trackWidth, m_spec.trackHeight);
    QColor trackColor = isChecked() ? m_spec.selectedTrackColor : m_spec.unselectedTrackColor;
    QColor handleColor = isChecked() ? m_spec.selectedHandleColor : m_spec.unselectedHandleColor;
    painter.setPen(Qt::NoPen);
    painter.setBrush(trackColor);
    painter.drawRoundedRect(track, track.height() / 2.0, track.height() / 2.0);
    const qreal handleDiameter = isChecked() ? m_spec.selectedHandleDiameter : m_spec.handleDiameter;
    const qreal x = isChecked() ? track.right() - handleDiameter - 4 : track.left() + 4;
    QRectF handle(x, track.center().y() - handleDiameter / 2.0, handleDiameter, handleDiameter);
    painter.setBrush(handleColor);
    painter.drawEllipse(handle);
    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(&painter, track.adjusted(-3, -3, 3, 3), m_spec.stateLayerColor, track.height() / 2.0 + 3, 2.0);
    }
}
} // namespace QtMaterial
