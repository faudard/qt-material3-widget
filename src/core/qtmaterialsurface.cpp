#include "qtmaterial/core/qtmaterialsurface.h"
#include <QPaintEvent>
#include <QPainter>
namespace QtMaterial {
QtMaterialSurface::QtMaterialSurface(QWidget* parent) : QtMaterialControl(parent), m_clipsContent(false) {}
QtMaterialSurface::~QtMaterialSurface() = default;
bool QtMaterialSurface::clipsContent() const noexcept { return m_clipsContent; }
void QtMaterialSurface::setClipsContent(bool value) { if (m_clipsContent == value) return; m_clipsContent = value; update(); }
void QtMaterialSurface::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(), theme().colorScheme().color(ColorRole::Surface));
}
} // namespace QtMaterial
