#pragma once
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
class QTMATERIAL3_CORE_EXPORT QtMaterialInteractionState
{
public:
    QtMaterialInteractionState();
    ~QtMaterialInteractionState();
    bool isHovered() const noexcept;
    bool isFocused() const noexcept;
    bool isPressed() const noexcept;
    bool isChecked() const noexcept;
    bool isCheckable() const noexcept;
    bool isEnabled() const noexcept;
    bool isSelected() const noexcept;
    bool hasError() const noexcept;
    void setHovered(bool value) noexcept;
    void setFocused(bool value) noexcept;
    void setPressed(bool value) noexcept;
    void setChecked(bool value) noexcept;
    void setCheckable(bool value) noexcept;
    void setEnabled(bool value) noexcept;
    void setSelected(bool value) noexcept;
    void setError(bool value) noexcept;
private:
    bool m_hovered;
    bool m_focused;
    bool m_pressed;
    bool m_checked;
    bool m_checkable;
    bool m_enabled;
    bool m_selected;
    bool m_error;
};
} // namespace QtMaterial
