#pragma once

#include <QString>
#include <QStringList>

#include "qtmaterial/qtmaterialglobal.h"

class QWidget;

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
    bool isReadOnly() const noexcept;
    bool isIndeterminate() const noexcept;
    bool isDragged() const noexcept;
    bool isBusy() const noexcept;
    bool isExpanded() const noexcept;
    bool isInvalid() const noexcept;

    void setHovered(bool value) noexcept;
    void setFocused(bool value) noexcept;
    void setPressed(bool value) noexcept;
    void setChecked(bool value) noexcept;
    void setCheckable(bool value) noexcept;
    void setEnabled(bool value) noexcept;
    void setSelected(bool value) noexcept;
    void setError(bool value) noexcept;
    void setReadOnly(bool value) noexcept;
    void setIndeterminate(bool value) noexcept;
    void setDragged(bool value) noexcept;
    void setBusy(bool value) noexcept;
    void setExpanded(bool value) noexcept;
    void setInvalid(bool value) noexcept;

    void clearTransientState() noexcept;
    void syncFromWidget(const QWidget* widget) noexcept;

    QStringList stateNames() const;
    QString toPropertyString() const;

    friend bool operator==(const QtMaterialInteractionState& lhs,
                           const QtMaterialInteractionState& rhs) noexcept;
    friend bool operator!=(const QtMaterialInteractionState& lhs,
                           const QtMaterialInteractionState& rhs) noexcept
    {
        return !(lhs == rhs);
    }

private:
    bool m_hovered;
    bool m_focused;
    bool m_pressed;
    bool m_checked;
    bool m_checkable;
    bool m_enabled;
    bool m_selected;
    bool m_error;
    bool m_readOnly;
    bool m_indeterminate;
    bool m_dragged;
    bool m_busy;
    bool m_expanded;
    bool m_invalid;
};

} // namespace QtMaterial
