#pragma once

namespace QtMaterial {

/**
 * Dependency-light interaction state shared by Specs and Core.
 *
 * The implementation is intentionally inline so the Foundation target remains
 * header-only and Specs does not acquire a binary dependency on Core.
 */
class QtMaterialInteractionState
{
public:
    QtMaterialInteractionState() noexcept = default;
    ~QtMaterialInteractionState() = default;

    bool isHovered() const noexcept { return m_hovered; }
    bool isFocused() const noexcept { return m_focused; }
    bool isPressed() const noexcept { return m_pressed; }
    bool isChecked() const noexcept { return m_checked; }
    bool isCheckable() const noexcept { return m_checkable; }
    bool isEnabled() const noexcept { return m_enabled; }
    bool isSelected() const noexcept { return m_selected; }
    bool hasError() const noexcept { return m_error; }

    void setHovered(bool value) noexcept { m_hovered = value; }
    void setFocused(bool value) noexcept { m_focused = value; }
    void setPressed(bool value) noexcept { m_pressed = value; }
    void setChecked(bool value) noexcept { m_checked = value; }
    void setCheckable(bool value) noexcept { m_checkable = value; }
    void setEnabled(bool value) noexcept { m_enabled = value; }
    void setSelected(bool value) noexcept { m_selected = value; }
    void setError(bool value) noexcept { m_error = value; }

private:
    bool m_hovered = false;
    bool m_focused = false;
    bool m_pressed = false;
    bool m_checked = false;
    bool m_checkable = false;
    bool m_enabled = true;
    bool m_selected = false;
    bool m_error = false;
};

} // namespace QtMaterial
