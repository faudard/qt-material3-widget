#include "qtmaterial/core/qtmaterialinteractionstate.h"

#include <QWidget>
#include <qvariant.h>

namespace QtMaterial {

QtMaterialInteractionState::QtMaterialInteractionState()
    : m_hovered(false)
    , m_focused(false)
    , m_pressed(false)
    , m_checked(false)
    , m_checkable(false)
    , m_enabled(true)
    , m_selected(false)
    , m_error(false)
    , m_readOnly(false)
    , m_indeterminate(false)
    , m_dragged(false)
    , m_busy(false)
    , m_expanded(false)
    , m_invalid(false)
{
}

QtMaterialInteractionState::~QtMaterialInteractionState() = default;

bool QtMaterialInteractionState::isHovered() const noexcept { return m_hovered; }
bool QtMaterialInteractionState::isFocused() const noexcept { return m_focused; }
bool QtMaterialInteractionState::isPressed() const noexcept { return m_pressed; }
bool QtMaterialInteractionState::isChecked() const noexcept { return m_checked; }
bool QtMaterialInteractionState::isCheckable() const noexcept { return m_checkable; }
bool QtMaterialInteractionState::isEnabled() const noexcept { return m_enabled; }
bool QtMaterialInteractionState::isSelected() const noexcept { return m_selected; }
bool QtMaterialInteractionState::hasError() const noexcept { return m_error; }
bool QtMaterialInteractionState::isReadOnly() const noexcept { return m_readOnly; }
bool QtMaterialInteractionState::isIndeterminate() const noexcept { return m_indeterminate; }
bool QtMaterialInteractionState::isDragged() const noexcept { return m_dragged; }
bool QtMaterialInteractionState::isBusy() const noexcept { return m_busy; }
bool QtMaterialInteractionState::isExpanded() const noexcept { return m_expanded; }
bool QtMaterialInteractionState::isInvalid() const noexcept { return m_invalid; }

void QtMaterialInteractionState::setHovered(bool value) noexcept { m_hovered = m_enabled && value; }
void QtMaterialInteractionState::setFocused(bool value) noexcept { m_focused = m_enabled && value; }
void QtMaterialInteractionState::setPressed(bool value) noexcept { m_pressed = m_enabled && value; }
void QtMaterialInteractionState::setChecked(bool value) noexcept { m_checked = m_checkable && value; }
void QtMaterialInteractionState::setSelected(bool value) noexcept { m_selected = m_enabled && value; }
void QtMaterialInteractionState::setError(bool value) noexcept { m_error = value; }
void QtMaterialInteractionState::setReadOnly(bool value) noexcept { m_readOnly = value; }
void QtMaterialInteractionState::setIndeterminate(bool value) noexcept { m_indeterminate = m_checkable && value; }
void QtMaterialInteractionState::setDragged(bool value) noexcept { m_dragged = m_enabled && value; }
void QtMaterialInteractionState::setBusy(bool value) noexcept { m_busy = value; }
void QtMaterialInteractionState::setExpanded(bool value) noexcept { m_expanded = value; }
void QtMaterialInteractionState::setInvalid(bool value) noexcept { m_invalid = value; }

void QtMaterialInteractionState::setCheckable(bool value) noexcept
{
    m_checkable = value;
    if (!m_checkable) {
        m_checked = false;
        m_indeterminate = false;
    }
}

void QtMaterialInteractionState::setEnabled(bool value) noexcept
{
    m_enabled = value;
    if (!m_enabled) {
        clearTransientState();
        m_selected = false;
    }
}

void QtMaterialInteractionState::clearTransientState() noexcept
{
    m_hovered = false;
    m_focused = false;
    m_pressed = false;
    m_dragged = false;
}

void QtMaterialInteractionState::syncFromWidget(const QWidget* widget) noexcept
{
    if (!widget) {
        return;
    }
    setEnabled(widget->isEnabled());
    setFocused(widget->hasFocus());
    setReadOnly(widget->property("readOnly").toBool());
}

QStringList QtMaterialInteractionState::stateNames() const
{
    QStringList names;
    names << (m_enabled ? QStringLiteral("enabled") : QStringLiteral("disabled"));

    if (m_hovered) names << QStringLiteral("hovered");
    if (m_focused) names << QStringLiteral("focused");
    if (m_pressed) names << QStringLiteral("pressed");
    if (m_checkable) names << QStringLiteral("checkable");
    if (m_checked) names << QStringLiteral("checked");
    if (m_selected) names << QStringLiteral("selected");
    if (m_error) names << QStringLiteral("error");
    if (m_readOnly) names << QStringLiteral("readOnly");
    if (m_indeterminate) names << QStringLiteral("indeterminate");
    if (m_dragged) names << QStringLiteral("dragged");
    if (m_busy) names << QStringLiteral("busy");
    if (m_expanded) names << QStringLiteral("expanded");
    if (m_invalid) names << QStringLiteral("invalid");

    return names;
}

QString QtMaterialInteractionState::toPropertyString() const
{
    return stateNames().join(QLatin1Char(' '));
}

bool operator==(const QtMaterialInteractionState& lhs,
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

} // namespace QtMaterial
