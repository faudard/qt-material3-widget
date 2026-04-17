#pragma once

#include <QtCore/qglobal.h>
#include <QMouseEvent>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QEnterEvent>
#endif

namespace QtMaterial {

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
using EnterEvent = QEnterEvent;
#else
using EnterEvent = QEvent;
#endif

inline QPointF mousePosition(QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

} // namespace QtMaterial