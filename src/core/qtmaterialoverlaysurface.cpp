#include "qtmaterial/core/qtmaterialoverlaysurface.h"

#include <QEvent>
#include <QPoint>
#include <QRect>
#include <QShowEvent>
#include <QWidget>

namespace QtMaterial {

QtMaterialOverlaySurface::QtMaterialOverlaySurface(QWidget* parent)
    : QtMaterialSurface(parent)
    , m_hostWidget()
    , m_restoreVisibilityOnHostShow(false)
{
}

QtMaterialOverlaySurface::~QtMaterialOverlaySurface()
{
    clearHostGeometryWatchers();
}

QWidget* QtMaterialOverlaySurface::hostWidget() const noexcept
{
    return m_hostWidget.data();
}

void QtMaterialOverlaySurface::setHostWidget(QWidget* host)
{
    if (m_hostWidget == host) {
        return;
    }

    clearHostGeometryWatchers();

    m_hostWidget = host;
    m_restoreVisibilityOnHostShow = false;

    if (!m_hostWidget) {
        return;
    }

    rebuildHostGeometryWatchers();
    syncGeometryToHost();

    if (!m_hostWidget->isVisible()) {
        m_restoreVisibilityOnHostShow = isVisible();
        if (isVisible()) {
            hide();
        }
    }
}

bool QtMaterialOverlaySurface::eventFilter(QObject* watched, QEvent* event)
{
    if (!event) {
        return QtMaterialSurface::eventFilter(watched, event);
    }

    const bool isHost = watched == m_hostWidget.data();
    const bool isGeometryWatcher = isHostGeometryWatcher(watched);

    if (isGeometryWatcher) {
        switch (event->type()) {
        case QEvent::Resize:
        case QEvent::Move:
            syncGeometryToHost();
            break;

        case QEvent::ParentChange:
            rebuildHostGeometryWatchers();
            syncGeometryToHost();
            break;

        case QEvent::Show:
            syncGeometryToHost();
            if (isHost && m_restoreVisibilityOnHostShow) {
                m_restoreVisibilityOnHostShow = false;
                show();
            }
            break;

        case QEvent::Hide:
            if (isHost) {
                m_restoreVisibilityOnHostShow = isVisible();
                if (isVisible()) {
                    hide();
                }
            }
            break;

        default:
            break;
        }
    }

    return QtMaterialSurface::eventFilter(watched, event);
}

void QtMaterialOverlaySurface::rebuildHostGeometryWatchers()
{
    clearHostGeometryWatchers();

    for (QWidget* widget = m_hostWidget.data();
         widget;
         widget = widget->parentWidget()) {
        widget->installEventFilter(this);
        m_hostGeometryWatchers.append(widget);
    }
}

void QtMaterialOverlaySurface::clearHostGeometryWatchers()
{
    for (const QPointer<QWidget>& widget : m_hostGeometryWatchers) {
        if (widget) {
            widget->removeEventFilter(this);
        }
    }
    m_hostGeometryWatchers.clear();
}

bool QtMaterialOverlaySurface::isHostGeometryWatcher(
    QObject* object) const noexcept
{
    for (const QPointer<QWidget>& widget : m_hostGeometryWatchers) {
        if (widget.data() == object) {
            return true;
        }
    }
    return false;
}

void QtMaterialOverlaySurface::showEvent(QShowEvent* event)
{
    syncGeometryToHost();
    QtMaterialSurface::showEvent(event);
}

void QtMaterialOverlaySurface::syncGeometryToHost()
{
    if (!m_hostWidget) {
        return;
    }

    QRect targetGeometry;

    if (QWidget* overlayParent = parentWidget()) {
        const QPoint topLeft =
            overlayParent->mapFromGlobal(m_hostWidget->mapToGlobal(QPoint(0, 0)));
        targetGeometry = QRect(topLeft, m_hostWidget->size());
    } else {
        targetGeometry = QRect(m_hostWidget->mapToGlobal(QPoint(0, 0)),
                               m_hostWidget->size());
    }

    if (geometry() != targetGeometry) {
        setGeometry(targetGeometry);
    }
}

} // namespace QtMaterial