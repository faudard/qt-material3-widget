#include "qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h"

#include <QPainter>
#include <QResizeEvent>

#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "private/qtmaterialsurfacerenderhelper_p.h"

namespace QtMaterial {

QtMaterialNavigationDrawer::QtMaterialNavigationDrawer(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
    , m_scrim(new QtMaterialScrimWidget(parent ? parent : this))
    , m_transition(new QtMaterialTransitionController(this))
{
    setMinimumWidth(280);
    hide();
    if (m_scrim) {
        m_scrim->hide();
    }
}

QtMaterialNavigationDrawer::~QtMaterialNavigationDrawer() = default;

void QtMaterialNavigationDrawer::open()
{
    ensureSpecResolved();
    if (m_scrim) {
        m_scrim->setGeometry(parentWidget() ? parentWidget()->rect() : rect());
        QColor scrim = m_spec.scrimColor;
        scrim.setAlphaF(0.32 * qMax(0.0, m_transition ? m_transition->progress() : 1.0));
        m_scrim->setScrimColor(scrim);
        m_scrim->show();
    }
    show();
    raise();
    if (m_transition) {
        m_transition->startForward();
    }
}

void QtMaterialNavigationDrawer::close()
{
    if (m_transition) {
        m_transition->startBackward();
    }
    hide();
    if (m_scrim) {
        m_scrim->hide();
    }
}

void QtMaterialNavigationDrawer::themeChangedEvent(const Theme& theme)
{
    QtMaterialOverlaySurface::themeChangedEvent(theme);
    m_specDirty = true;
}

void QtMaterialNavigationDrawer::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialNavigationDrawer::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }
    SpecFactory factory;
    m_spec = factory.navigationDrawerSpec(theme());
    m_specDirty = false;
}

void QtMaterialNavigationDrawer::syncGeometryToHost()
{
    ensureSpecResolved();
    if (hostWidget()) {
        const int width = qMin(m_spec.preferredWidth, hostWidget()->width());
        setGeometry(0, 0, width, hostWidget()->height());
        if (m_scrim) {
            m_scrim->setGeometry(hostWidget()->rect());
        }
        return;
    }
    QtMaterialOverlaySurface::syncGeometryToHost();
}

void QtMaterialNavigationDrawer::resizeEvent(QResizeEvent* event)
{
    QtMaterialOverlaySurface::resizeEvent(event);
    if (m_scrim && parentWidget()) {
        m_scrim->setGeometry(parentWidget()->rect());
    }
}

void QtMaterialNavigationDrawer::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();

    QPainter painter(this);
    QtMaterialSurfaceRenderHelper::paintFrame(&painter, QtMaterialSurfaceRenderHelper::drawerFrame(theme(), m_spec, rect()));
    painter.setPen(m_spec.contentColor);
    painter.drawText(rect().adjusted(24, 24, -24, -24), Qt::AlignLeft | Qt::AlignTop, QStringLiteral("Navigation drawer"));
}

} // namespace QtMaterial
