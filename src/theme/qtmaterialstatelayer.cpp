#include "qtmaterial/theme/qtmaterialstatelayer.h"

namespace QtMaterial {

StateLayer::StateLayer()
    : m_color()
    , m_hoverOpacity(0.08)
    , m_focusOpacity(0.10)
    , m_pressOpacity(0.10)
    , m_dragOpacity(0.16)
{}

StateLayer::~StateLayer() = default;

const QColor& StateLayer::color() const noexcept { return m_color; }
void StateLayer::setColor(const QColor& color) { m_color = color; }
qreal StateLayer::hoverOpacity() const noexcept { return m_hoverOpacity; }
void StateLayer::setHoverOpacity(qreal value) noexcept { m_hoverOpacity = value; }
qreal StateLayer::focusOpacity() const noexcept { return m_focusOpacity; }
void StateLayer::setFocusOpacity(qreal value) noexcept { m_focusOpacity = value; }
qreal StateLayer::pressOpacity() const noexcept { return m_pressOpacity; }
void StateLayer::setPressOpacity(qreal value) noexcept { m_pressOpacity = value; }
qreal StateLayer::dragOpacity() const noexcept { return m_dragOpacity; }
void StateLayer::setDragOpacity(qreal value) noexcept { m_dragOpacity = value; }

} // namespace QtMaterial
