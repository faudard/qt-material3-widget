#include "qtmaterial/core/qtmaterialabstractbutton.h"

#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>

#include "qtmaterial/core/qtmaterialeventcompat.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "private/qtmaterialaccessibilityhelper_p.h"

namespace {

struct InteractionSnapshot {
    bool hovered;
    bool focused;
    bool pressed;
    bool checked;
    bool checkable;
    bool enabled;
    bool selected;
    bool error;
};

[[nodiscard]] InteractionSnapshot snapshotOf(const QtMaterial::QtMaterialInteractionState& state) noexcept
{
    return {
        state.isHovered(),
        state.isFocused(),
        state.isPressed(),
        state.isChecked(),
        state.isCheckable(),
        state.isEnabled(),
        state.isSelected(),
        state.hasError()
    };
}

[[nodiscard]] bool sameSnapshot(const InteractionSnapshot& lhs,
                                const InteractionSnapshot& rhs) noexcept
{
    return lhs.hovered   == rhs.hovered
        && lhs.focused   == rhs.focused
        && lhs.pressed   == rhs.pressed
        && lhs.checked   == rhs.checked
        && lhs.checkable == rhs.checkable
        && lhs.enabled   == rhs.enabled
        && lhs.selected  == rhs.selected
        && lhs.error     == rhs.error;
}

} // namespace

namespace QtMaterial {

QtMaterialAbstractButton::QtMaterialAbstractButton(QWidget* parent)
    : QAbstractButton(parent)
    , m_state()
    , m_density(Density::Default)
{
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::StrongFocus);

    QObject::connect(&ThemeManager::instance(),
                     &ThemeManager::themeChanged,
                     this,
                     &QtMaterialAbstractButton::handleThemeChanged);

    syncFromButtonState();
    syncAccessibilityState();
}

QtMaterialAbstractButton::~QtMaterialAbstractButton() = default;

Density QtMaterialAbstractButton::density() const noexcept
{
    return m_density;
}

void QtMaterialAbstractButton::setDensity(Density density)
{
    if (m_density == density) {
        return;
    }

    m_density = density;
    invalidateResolvedSpec();
    contentChangedEvent();
    updateGeometry();
    update();
}

void QtMaterialAbstractButton::setText(const QString& text)
{
    if (QAbstractButton::text() == text) {
        return;
    }

    QAbstractButton::setText(text);
    syncAccessibilityState();
    invalidateResolvedSpec();
    contentChangedEvent();
    updateGeometry();
    update();
}

void QtMaterialAbstractButton::setIcon(const QIcon& icon)
{
    QAbstractButton::setIcon(icon);
    invalidateResolvedSpec();
    contentChangedEvent();
    updateGeometry();
    update();
}

void QtMaterialAbstractButton::setCheckable(bool checkable)
{
    if (QAbstractButton::isCheckable() == checkable) {
        return;
    }

    QAbstractButton::setCheckable(checkable);
    syncAndNotifyIfChanged();
    syncAccessibilityState();
}

void QtMaterialAbstractButton::setDown(bool down)
{
    if (QAbstractButton::isDown() == down) {
        return;
    }

    QAbstractButton::setDown(down);
    syncAndNotifyIfChanged();
}

const Theme& QtMaterialAbstractButton::theme() const
{
    return ThemeManager::instance().theme();
}

const QtMaterialInteractionState& QtMaterialAbstractButton::interactionState() const noexcept
{
    return m_state;
}

QtMaterialInteractionState& QtMaterialAbstractButton::interactionState() noexcept
{
    return m_state;
}

void QtMaterialAbstractButton::syncFromButtonState() noexcept
{
    m_state.setEnabled(isEnabled());
    m_state.setCheckable(isCheckable());
    m_state.setChecked(isChecked());
    m_state.setPressed(isDown());
}

void QtMaterialAbstractButton::syncAndNotifyIfChanged() noexcept
{
    const auto before = snapshotOf(m_state);
    syncFromButtonState();

    if (!sameSnapshot(before, snapshotOf(m_state))) {
        stateChangedEvent();
    }
}

void QtMaterialAbstractButton::themeChangedEvent(const Theme&)
{
    invalidateResolvedSpec();
    updateGeometry();
    update();
}

void QtMaterialAbstractButton::invalidateResolvedSpec() {}

void QtMaterialAbstractButton::stateChangedEvent()
{
    update();
}

void QtMaterialAbstractButton::syncAccessibilityState()
{
    AccessibilityHelper::applyButtonAccessibility(this);
}

void QtMaterialAbstractButton::contentChangedEvent() {}

void QtMaterialAbstractButton::enterEvent(EnterEvent* event)
{
    const auto before = snapshotOf(m_state);

    QAbstractButton::enterEvent(event);

    m_state.setHovered(true);
    syncFromButtonState();

    if (!sameSnapshot(before, snapshotOf(m_state))) {
        stateChangedEvent();
    }
}

void QtMaterialAbstractButton::leaveEvent(QEvent* event)
{
    const auto before = snapshotOf(m_state);

    QAbstractButton::leaveEvent(event);

    m_state.setHovered(false);
    syncFromButtonState();

    if (!sameSnapshot(before, snapshotOf(m_state))) {
        stateChangedEvent();
    }
}

void QtMaterialAbstractButton::focusInEvent(QFocusEvent* event)
{
    const auto before = snapshotOf(m_state);

    QAbstractButton::focusInEvent(event);

    m_state.setFocused(true);
    syncFromButtonState();

    if (!sameSnapshot(before, snapshotOf(m_state))) {
        stateChangedEvent();
    }
}

void QtMaterialAbstractButton::focusOutEvent(QFocusEvent* event)
{
    const auto before = snapshotOf(m_state);

    QAbstractButton::focusOutEvent(event);

    m_state.setFocused(false);
    syncFromButtonState();

    if (!sameSnapshot(before, snapshotOf(m_state))) {
        stateChangedEvent();
    }
}

void QtMaterialAbstractButton::changeEvent(QEvent* event)
{
    QAbstractButton::changeEvent(event);

    if (event->type() == QEvent::EnabledChange) {
        syncAndNotifyIfChanged();
    }

    syncAccessibilityState();
}

void QtMaterialAbstractButton::checkStateSet()
{
    QAbstractButton::checkStateSet();
    syncAndNotifyIfChanged();
}

void QtMaterialAbstractButton::nextCheckState()
{
    QAbstractButton::nextCheckState();
    syncAndNotifyIfChanged();
}

void QtMaterialAbstractButton::mousePressEvent(QMouseEvent* event)
{
    QAbstractButton::mousePressEvent(event);
    syncAndNotifyIfChanged();
}

void QtMaterialAbstractButton::mouseReleaseEvent(QMouseEvent* event)
{
    QAbstractButton::mouseReleaseEvent(event);
    syncAndNotifyIfChanged();
}

void QtMaterialAbstractButton::keyPressEvent(QKeyEvent* event)
{
    if (!isEnabled()) {
        QAbstractButton::keyPressEvent(event);
        syncAndNotifyIfChanged();
        return;
    }

    if ((event->key() == Qt::Key_Return
         || event->key() == Qt::Key_Enter
         || event->key() == Qt::Key_Select)
        && !event->isAutoRepeat()) {
        click();
        syncAndNotifyIfChanged();
        event->accept();
        return;
    }

    QAbstractButton::keyPressEvent(event);
    syncAndNotifyIfChanged();
}

void QtMaterialAbstractButton::keyReleaseEvent(QKeyEvent* event)
{
    QAbstractButton::keyReleaseEvent(event);
    syncAndNotifyIfChanged();
}

void QtMaterialAbstractButton::handleThemeChanged(const Theme& theme)
{
    themeChangedEvent(theme);
}

} // namespace QtMaterial