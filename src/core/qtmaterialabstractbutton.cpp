#include "qtmaterial/core/qtmaterialabstractbutton.h"
#include <QFocusEvent>
#include <QMouseEvent>
#include "qtmaterial/theme/qtmaterialthememanager.h"
namespace QtMaterial {
QtMaterialAbstractButton::QtMaterialAbstractButton(QWidget* parent) : QAbstractButton(parent), m_state(), m_density(Density::Default)
{
    setAttribute(Qt::WA_Hover, true);
    QObject::connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &QtMaterialAbstractButton::handleThemeChanged);
    m_state.setCheckable(isCheckable());
    m_state.setEnabled(isEnabled());
}
QtMaterialAbstractButton::~QtMaterialAbstractButton() = default;
Density QtMaterialAbstractButton::density() const noexcept { return m_density; }
void QtMaterialAbstractButton::setDensity(Density density) { if (m_density == density) return; m_density = density; invalidateResolvedSpec(); updateGeometry(); update(); }
const Theme& QtMaterialAbstractButton::theme() const { return ThemeManager::instance().theme(); }
const QtMaterialInteractionState& QtMaterialAbstractButton::interactionState() const noexcept { return m_state; }
QtMaterialInteractionState& QtMaterialAbstractButton::interactionState() noexcept { return m_state; }
void QtMaterialAbstractButton::themeChangedEvent(const Theme&) { invalidateResolvedSpec(); updateGeometry(); update(); }
void QtMaterialAbstractButton::invalidateResolvedSpec() {}
void QtMaterialAbstractButton::stateChangedEvent() { update(); }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void QtMaterialAbstractButton::enterEvent(QEnterEvent *event)
#else
void QtMaterialAbstractButton::enterEvent(QEvent *event)
#endif
{ m_state.setHovered(true); stateChangedEvent(); QAbstractButton::enterEvent(event); }

void QtMaterialAbstractButton::leaveEvent(QEvent* event) { m_state.setHovered(false); m_state.setPressed(false); stateChangedEvent(); QAbstractButton::leaveEvent(event); }
void QtMaterialAbstractButton::focusInEvent(QFocusEvent* event) { m_state.setFocused(true); stateChangedEvent(); QAbstractButton::focusInEvent(event); }
void QtMaterialAbstractButton::focusOutEvent(QFocusEvent* event) { m_state.setFocused(false); m_state.setPressed(false); stateChangedEvent(); QAbstractButton::focusOutEvent(event); }
void QtMaterialAbstractButton::changeEvent(QEvent* event) { if (event->type() == QEvent::EnabledChange) { m_state.setEnabled(isEnabled()); stateChangedEvent(); } QAbstractButton::changeEvent(event); }
void QtMaterialAbstractButton::checkStateSet() { m_state.setChecked(isChecked()); QAbstractButton::checkStateSet(); stateChangedEvent(); }
void QtMaterialAbstractButton::nextCheckState() { QAbstractButton::nextCheckState(); m_state.setChecked(isChecked()); m_state.setCheckable(isCheckable()); stateChangedEvent(); }
void QtMaterialAbstractButton::mousePressEvent(QMouseEvent* event) { m_state.setPressed(true); stateChangedEvent(); QAbstractButton::mousePressEvent(event); }
void QtMaterialAbstractButton::mouseReleaseEvent(QMouseEvent* event) { m_state.setPressed(false); m_state.setChecked(isChecked()); stateChangedEvent(); QAbstractButton::mouseReleaseEvent(event); }
void QtMaterialAbstractButton::handleThemeChanged(const Theme& theme) { themeChangedEvent(theme); }
} // namespace QtMaterial
