#pragma once

#include <QColor>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_THEME_EXPORT StateLayer
{
public:
    StateLayer();
    ~StateLayer();

    const QColor& color() const noexcept;
    void setColor(const QColor& color);

    qreal hoverOpacity() const noexcept;
    void setHoverOpacity(qreal value) noexcept;

    qreal focusOpacity() const noexcept;
    void setFocusOpacity(qreal value) noexcept;

    qreal pressOpacity() const noexcept;
    void setPressOpacity(qreal value) noexcept;

    qreal dragOpacity() const noexcept;
    void setDragOpacity(qreal value) noexcept;

private:
    QColor m_color;
    qreal m_hoverOpacity;
    qreal m_focusOpacity;
    qreal m_pressOpacity;
    qreal m_dragOpacity;
};

} // namespace QtMaterial
