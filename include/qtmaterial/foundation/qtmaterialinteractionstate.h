#pragma once

namespace QtMaterial {

/**
 * Dependency-free interaction state shared by Specs and Core.
 *
 * This value type deliberately contains no QWidget, QObject, QString, or other
 * Qt dependency. Widget synchronization and textual automation serialization
 * belong to Core.
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
    bool isReadOnly() const noexcept { return m_readOnly; }
    bool isIndeterminate() const noexcept { return m_indeterminate; }
    bool isDragged() const noexcept { return m_dragged; }
    bool isBusy() const noexcept { return m_busy; }
    bool isExpanded() const noexcept { return m_expanded; }
    bool isInvalid() const noexcept { return m_invalid; }

    void setHovered(bool value) noexcept
    {
        m_hovered = m_enabled && value;
    }

    void setFocused(bool value) noexcept
    {
        m_focused = m_enabled && value;
    }

    void setPressed(bool value) noexcept
    {
        m_pressed = m_enabled && value;
    }

    void setChecked(bool value) noexcept
    {
        m_checked = m_checkable && value;
    }

    void setCheckable(bool value) noexcept
    {
        m_checkable = value;
        if (!m_checkable) {
            m_checked = false;
            m_indeterminate = false;
        }
    }

    void setEnabled(bool value) noexcept
    {
        m_enabled = value;
        if (!m_enabled) {
            clearTransientState();
            m_selected = false;
        }
    }

    void setSelected(bool value) noexcept
    {
        m_selected = m_enabled && value;
    }

    void setError(bool value) noexcept
    {
        m_error = value;
    }

    void setReadOnly(bool value) noexcept
    {
        m_readOnly = value;
    }

    void setIndeterminate(bool value) noexcept
    {
        m_indeterminate = m_checkable && value;
    }

    void setDragged(bool value) noexcept
    {
        m_dragged = m_enabled && value;
    }

    void setBusy(bool value) noexcept
    {
        m_busy = value;
    }

    void setExpanded(bool value) noexcept
    {
        m_expanded = value;
    }

    void setInvalid(bool value) noexcept
    {
        m_invalid = value;
    }

    void clearTransientState() noexcept
    {
        m_hovered = false;
        m_focused = false;
        m_pressed = false;
        m_dragged = false;
    }

    friend bool operator==(const QtMaterialInteractionState& lhs,
                           const QtMaterialInteractionState& rhs) noexcept
    {
        return lhs.m_hovered == rhs.m_hovered
            && lhs.m_focused == rhs.m_focused
            && lhs.m_pressed == rhs.m_pressed
            && lhs.m_checked == rhs.m_checked
            && lhs.m_checkable == rhs.m_checkable
            && lhs.m_enabled == rhs.m_enabled
            && lhs.m_selected == rhs.m_selected
            && lhs.m_error == rhs.m_error
            && lhs.m_readOnly == rhs.m_readOnly
            && lhs.m_indeterminate == rhs.m_indeterminate
            && lhs.m_dragged == rhs.m_dragged
            && lhs.m_busy == rhs.m_busy
            && lhs.m_expanded == rhs.m_expanded
            && lhs.m_invalid == rhs.m_invalid;
    }

    friend bool operator!=(const QtMaterialInteractionState& lhs,
                           const QtMaterialInteractionState& rhs) noexcept
    {
        return !(lhs == rhs);
    }

private:
    bool m_hovered = false;
    bool m_focused = false;
    bool m_pressed = false;
    bool m_checked = false;
    bool m_checkable = false;
    bool m_enabled = true;
    bool m_selected = false;
    bool m_error = false;
    bool m_readOnly = false;
    bool m_indeterminate = false;
    bool m_dragged = false;
    bool m_busy = false;
    bool m_expanded = false;
    bool m_invalid = false;
};

} // namespace QtMaterial
