#include "qtmaterial/core/qtmaterialinteractionstate.h"
namespace QtMaterial {
QtMaterialInteractionState::QtMaterialInteractionState() : m_hovered(false), m_focused(false), m_pressed(false), m_checked(false), m_checkable(false), m_enabled(true), m_selected(false), m_error(false) {}
QtMaterialInteractionState::~QtMaterialInteractionState() = default;
bool QtMaterialInteractionState::isHovered() const noexcept { return m_hovered; }
bool QtMaterialInteractionState::isFocused() const noexcept { return m_focused; }
bool QtMaterialInteractionState::isPressed() const noexcept { return m_pressed; }
bool QtMaterialInteractionState::isChecked() const noexcept { return m_checked; }
bool QtMaterialInteractionState::isCheckable() const noexcept { return m_checkable; }
bool QtMaterialInteractionState::isEnabled() const noexcept { return m_enabled; }
bool QtMaterialInteractionState::isSelected() const noexcept { return m_selected; }
bool QtMaterialInteractionState::hasError() const noexcept { return m_error; }
void QtMaterialInteractionState::setHovered(bool value) noexcept { m_hovered = value; }
void QtMaterialInteractionState::setFocused(bool value) noexcept { m_focused = value; }
void QtMaterialInteractionState::setPressed(bool value) noexcept
{
    m_pressed = m_enabled && value;
}

void QtMaterialInteractionState::setChecked(bool value) noexcept
{
    m_checked = m_checkable && value;
}

void QtMaterialInteractionState::setCheckable(bool value) noexcept
{
    m_checkable = value;
    if (!m_checkable) {
        m_checked = false;
    }
}

void QtMaterialInteractionState::setEnabled(bool value) noexcept
{
    m_enabled = value;
    if (!m_enabled) {
        m_hovered = false;
        m_focused = false;
        m_pressed = false;
    }
}
void QtMaterialInteractionState::setSelected(bool value) noexcept { m_selected = value; }
void QtMaterialInteractionState::setError(bool value) noexcept { m_error = value; }
} // namespace QtMaterial
