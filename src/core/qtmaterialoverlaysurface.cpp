#include "qtmaterial/core/qtmaterialoverlaysurface.h"

#include <QEvent>
#include <QPoint>
#include <QRect>
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
    if (m_hostWidget) {
        m_hostWidget->removeEventFilter(this);
    }
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

    if (m_hostWidget) {
        m_hostWidget->removeEventFilter(this);
    }

    m_hostWidget = host;
    m_restoreVisibilityOnHostShow = false;

    if (!m_hostWidget) {
        return;
    }

    m_hostWidget->installEventFilter(this);
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
    if (watched == m_hostWidget && event) {
        switch (event->type()) {
        case QEvent::Resize:
        case QEvent::Move:
        case QEvent::ParentChange:
            syncGeometryToHost();
            break;

        case QEvent::Show:
            syncGeometryToHost();
            if (m_restoreVisibilityOnHostShow) {
                m_restoreVisibilityOnHostShow = false;
                show();
            }
            break;

        case QEvent::Hide:
            m_restoreVisibilityOnHostShow = isVisible();
            if (isVisible()) {
                hide();
            }
            break;

        default:
            break;
        }
    }

    return QtMaterialSurface::eventFilter(watched, event);
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