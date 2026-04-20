#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/core/qtmaterialeventcompat.h"
namespace QtMaterial {
QtMaterialControl::QtMaterialControl(QWidget* parent) : QtMaterialWidget(parent), m_state(), m_density(Density::Default) { setAttribute(Qt::WA_Hover, true); }
QtMaterialControl::~QtMaterialControl() = default;
bool QtMaterialControl::isHovered() const noexcept { return m_state.isHovered(); }
bool QtMaterialControl::isPressed() const noexcept { return m_state.isPressed(); }
bool QtMaterialControl::isFocusedControl() const noexcept { return m_state.isFocused(); }
Density QtMaterialControl::density() const noexcept { return m_density; }
void QtMaterialControl::setDensity(Density density) { if (m_density == density) return; m_density = density; invalidateResolvedSpec(); updateGeometry(); update(); }
const QtMaterialInteractionState& QtMaterialControl::interactionState() const noexcept { return m_state; }
QtMaterialInteractionState& QtMaterialControl::interactionState() noexcept { return m_state; }
void QtMaterialControl::themeChangedEvent(const QtMaterial::Theme&) { invalidateResolvedSpec(); updateGeometry(); update(); }
void QtMaterialControl::invalidateResolvedSpec() {}
void QtMaterialControl::stateChangedEvent() { update(); }
void QtMaterialControl::enterEvent(EnterEvent* event) { m_state.setHovered(true); stateChangedEvent(); QtMaterialWidget::enterEvent(event); }
void QtMaterialControl::leaveEvent(QEvent* event) { m_state.setHovered(false); m_state.setPressed(false); stateChangedEvent(); QtMaterialWidget::leaveEvent(event); }
void QtMaterialControl::focusInEvent(QFocusEvent* event) { m_state.setFocused(true); stateChangedEvent(); QtMaterialWidget::focusInEvent(event); }
void QtMaterialControl::focusOutEvent(QFocusEvent* event) { m_state.setFocused(false); m_state.setPressed(false); stateChangedEvent(); QtMaterialWidget::focusOutEvent(event); }
void QtMaterialControl::changeEvent(QEvent* event) { if (event->type() == QEvent::EnabledChange) { m_state.setEnabled(isEnabled()); stateChangedEvent(); } QtMaterialWidget::changeEvent(event); }
} // namespace QtMaterial
