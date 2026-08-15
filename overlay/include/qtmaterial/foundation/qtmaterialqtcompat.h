#pragma once

#include <QtGlobal>
#include <QEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QWheelEvent>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#  include <QEnterEvent>
#endif

namespace QtMaterial {
namespace QtCompat {

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using EnterEvent = QEvent;
static constexpr int QtMajorVersion = 5;
#else
using EnterEvent = QEnterEvent;
static constexpr int QtMajorVersion = 6;
#endif

inline QPointF mousePosition(const QMouseEvent* event) noexcept
{
    if (!event) return QPointF();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return event->localPos();
#else
    return event->position();
#endif
}

inline QPointF mouseGlobalPosition(const QMouseEvent* event) noexcept
{
    if (!event) return QPointF();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return event->screenPos();
#else
    return event->globalPosition();
#endif
}

inline QPointF wheelPosition(const QWheelEvent* event) noexcept
{
    if (!event) return QPointF();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return event->posF();
#else
    return event->position();
#endif
}

inline bool isDevicePixelRatioChange(const QEvent* event) noexcept
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    return event && event->type() == QEvent::DevicePixelRatioChange;
#else
    Q_UNUSED(event);
    return false;
#endif
}

} // namespace QtCompat
} // namespace QtMaterial
