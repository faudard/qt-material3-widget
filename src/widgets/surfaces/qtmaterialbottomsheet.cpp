#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"

#include <QPainter>
#include <QResizeEvent>

#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "private/qtmaterialsurfacerenderhelper_p.h"

namespace QtMaterial {

QtMaterialBottomSheet::QtMaterialBottomSheet(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
    , m_scrim(new QtMaterialScrimWidget(parent ? parent : this))
    , m_transition(new QtMaterialTransitionController(this))
{
    setMinimumHeight(240);
    hide();
    if (m_scrim) {
        m_scrim->hide();
    }
}

QtMaterialBottomSheet::~QtMaterialBottomSheet() = default;

void QtMaterialBottomSheet::open()
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

void QtMaterialBottomSheet::close()
{
    if (m_transition) {
        m_transition->startBackward();
    }
    hide();
    if (m_scrim) {
        m_scrim->hide();
    }
}

void QtMaterialBottomSheet::themeChangedEvent(const Theme& theme)
{
    QtMaterialOverlaySurface::themeChangedEvent(theme);
    m_specDirty = true;
}

void QtMaterialBottomSheet::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialBottomSheet::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }
    SpecFactory factory;
    m_spec = factory.bottomSheetSpec(theme());
    m_specDirty = false;
}

void QtMaterialBottomSheet::syncGeometryToHost()
{
    ensureSpecResolved();
    if (hostWidget()) {
        const int panelHeight = qMax(minimumHeight(), hostWidget()->height() / 3);
        setGeometry(0, hostWidget()->height() - panelHeight, hostWidget()->width(), panelHeight);
        if (m_scrim) {
            m_scrim->setGeometry(hostWidget()->rect());
        }
        return;
    }
    QtMaterialOverlaySurface::syncGeometryToHost();
}

void QtMaterialBottomSheet::resizeEvent(QResizeEvent* event)
{
    QtMaterialOverlaySurface::resizeEvent(event);
    if (m_scrim && parentWidget()) {
        m_scrim->setGeometry(parentWidget()->rect());
    }
}

void QtMaterialBottomSheet::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();

    QPainter painter(this);
    QtMaterialSurfaceRenderHelper::paintFrame(&painter, QtMaterialSurfaceRenderHelper::bottomSheetFrame(theme(), m_spec, rect()));
    painter.setPen(m_spec.contentColor);
    painter.drawText(rect().adjusted(24, m_spec.topPadding + 16, -24, -24), Qt::AlignLeft | Qt::AlignTop, QStringLiteral("Bottom sheet"));
}

} // namespace QtMaterial
