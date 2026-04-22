#include "qtmaterial/core/qtmaterialabstractbutton.h"

#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include "qtmaterial/core/qtmaterialeventcompat.h"

#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "private/qtmaterialaccessibilityhelper_p.h"

namespace QtMaterial {

struct Snapshot {
    bool hovered;
    bool focused;
    bool pressed;
    bool checked;
    bool checkable;
    bool enabled;
};

static Snapshot capture(const QtMaterial::QtMaterialInteractionState& s) noexcept {
    return {
        s.isHovered(), s.isFocused(), s.isPressed(),
        s.isChecked(), s.isCheckable(), s.isEnabled()
    };
}

void QtMaterialAbstractButton::syncFromButtonState() noexcept
{
    m_state.setEnabled(isEnabled());
    m_state.setCheckable(isCheckable());
    m_state.setChecked(isChecked());
    m_state.setPressed(isEnabled() && isDown());

    if (!isEnabled()) {
        m_state.setHovered(false);
        m_state.setFocused(false);
    }
}

void QtMaterialAbstractButton::syncAndNotifyIfChanged() noexcept
{
    const auto before = capture(m_state);
    syncFromButtonState();
    const auto after = capture(m_state);

    if (std::memcmp(&before, &after, sizeof(Snapshot)) != 0) {
        stateChangedEvent();
    }
}


QtMaterialAbstractButton::QtMaterialAbstractButton(QWidget* parent)
    : QAbstractButton(parent)
    , m_state()
    , m_density(Density::Default)
{
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::StrongFocus);
    QObject::connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &QtMaterialAbstractButton::handleThemeChanged);
    m_state.setCheckable(isCheckable());
    m_state.setEnabled(isEnabled());
    syncAccessibilityState();
}

QtMaterialAbstractButton::~QtMaterialAbstractButton() = default;

Density QtMaterialAbstractButton::density() const noexcept { return m_density; }

void QtMaterialAbstractButton::setDensity(Density density)
{
    if (m_density == density) return;
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

const Theme& QtMaterialAbstractButton::theme() const { return ThemeManager::instance().theme(); }
const QtMaterialInteractionState& QtMaterialAbstractButton::interactionState() const noexcept { return m_state; }
QtMaterialInteractionState& QtMaterialAbstractButton::interactionState() noexcept { return m_state; }

void QtMaterialAbstractButton::themeChangedEvent(const Theme&)
{
    invalidateResolvedSpec();
    updateGeometry();
    update();
}

void QtMaterialAbstractButton::invalidateResolvedSpec() {}
void QtMaterialAbstractButton::stateChangedEvent() { update(); }
void QtMaterialAbstractButton::syncAccessibilityState() { AccessibilityHelper::applyButtonAccessibility(this); }
void QtMaterialAbstractButton::contentChangedEvent() {}

void QtMaterialAbstractButton::enterEvent(EnterEvent* event)
{
    m_state.setHovered(true);
    stateChangedEvent();
    QAbstractButton::enterEvent(event);
}

void QtMaterialAbstractButton::leaveEvent(QEvent* event)
{
    m_state.setHovered(false);
    m_state.setPressed(false);
    stateChangedEvent();
    QAbstractButton::leaveEvent(event);
}

void QtMaterialAbstractButton::focusInEvent(QFocusEvent* event)
{
    m_state.setFocused(true);
    stateChangedEvent();
    QAbstractButton::focusInEvent(event);
}

void QtMaterialAbstractButton::focusOutEvent(QFocusEvent* event)
{
    m_state.setFocused(false);
    m_state.setPressed(false);
    stateChangedEvent();
    QAbstractButton::focusOutEvent(event);
}

void QtMaterialAbstractButton::mousePressEvent(QMouseEvent* event)
{
    if (!isEnabled() || event->button() != Qt::LeftButton) {
        QAbstractButton::mousePressEvent(event);
        syncAndNotifyIfChanged();
        return;
    }

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
    QAbstractButton::keyPressEvent(event);
    syncAndNotifyIfChanged();
}

void QtMaterialAbstractButton::keyReleaseEvent(QKeyEvent* event)
{
    QAbstractButton::keyReleaseEvent(event);
    syncAndNotifyIfChanged();
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

void QtMaterialAbstractButton::changeEvent(QEvent* event)
{
    QAbstractButton::changeEvent(event);

    if (event->type() == QEvent::EnabledChange) {
        syncAndNotifyIfChanged();
    }

    syncAccessibilityState();
}


void QtMaterialAbstractButton::handleThemeChanged(const Theme& theme) { themeChangedEvent(theme); }


} // namespace QtMaterial
