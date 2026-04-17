#include "qtmaterial/core/qtmaterialoverlaysurface.h"
#include <QEvent>
namespace QtMaterial {
QtMaterialOverlaySurface::QtMaterialOverlaySurface(QWidget* parent) : QtMaterialSurface(parent) {}
QtMaterialOverlaySurface::~QtMaterialOverlaySurface() { if (m_hostWidget) m_hostWidget->removeEventFilter(this); }
QWidget* QtMaterialOverlaySurface::hostWidget() const noexcept { return m_hostWidget.data(); }
void QtMaterialOverlaySurface::setHostWidget(QWidget* host)
{
    if (m_hostWidget == host) return;
    if (m_hostWidget) m_hostWidget->removeEventFilter(this);
    m_hostWidget = host;
    if (m_hostWidget) { m_hostWidget->installEventFilter(this); syncGeometryToHost(); }
}
bool QtMaterialOverlaySurface::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_hostWidget && event) {
        switch (event->type()) {
        case QEvent::Resize:
        case QEvent::Move:
        case QEvent::Show:
            syncGeometryToHost();
            break;
        default:
            break;
        }
    }
    return QtMaterialSurface::eventFilter(watched, event);
}
void QtMaterialOverlaySurface::syncGeometryToHost()
{
    if (!m_hostWidget) return;
    setGeometry(m_hostWidget->rect());
}
} // namespace QtMaterial
