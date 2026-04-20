#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"

#include <QKeyEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QVBoxLayout>

#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "private/qtmaterialsurfacerenderhelper_p.h"

namespace QtMaterial {

QtMaterialDialog::QtMaterialDialog(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
    , m_layout(new QVBoxLayout(this))
    , m_scrim(new QtMaterialScrimWidget(parent ? parent : this))
    , m_transition(new QtMaterialTransitionController(this))
{
    m_layout->setContentsMargins(24, 24, 24, 24);
    hide();
    if (m_scrim) {
        m_scrim->hide();
    }
}

QtMaterialDialog::~QtMaterialDialog() = default;

void QtMaterialDialog::setBodyWidget(QWidget* widget)
{
    if (m_bodyWidget == widget) {
        return;
    }

    if (m_bodyWidget) {
        m_layout->removeWidget(m_bodyWidget);
        m_bodyWidget->setParent(nullptr);
    }

    m_bodyWidget = widget;
    if (m_bodyWidget) {
        m_bodyWidget->setParent(this);
        m_layout->addWidget(m_bodyWidget);
    }
}

QWidget* QtMaterialDialog::bodyWidget() const
{
    return m_bodyWidget;
}

void QtMaterialDialog::open()
{
    resolveSpecIfNeeded();
    if (m_scrim) {
        m_scrim->setGeometry(parentWidget() ? parentWidget()->rect() : rect());
        QColor scrim = m_spec.scrimColor;
        scrim.setAlphaF(0.32 * qMax(0.0, m_transition ? m_transition->progress() : 1.0));
        m_scrim->setScrimColor(scrim);
        m_scrim->show();
    }
    show();
    raise();
    setFocus(Qt::OtherFocusReason);
    if (m_transition) {
        m_transition->startForward();
    }
}

void QtMaterialDialog::close()
{
    if (m_transition) {
        m_transition->startBackward();
    }
    hide();
    if (m_scrim) {
        m_scrim->hide();
    }
}

void QtMaterialDialog::themeChangedEvent(const Theme& theme)
{
    QtMaterialOverlaySurface::themeChangedEvent(theme);
    m_specDirty = true;
}

void QtMaterialDialog::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialDialog::resolveSpecIfNeeded() const
{
    if (!m_specDirty) {
        return;
    }
    SpecFactory factory;
    m_spec = factory.dialogSpec(theme());
    m_specDirty = false;
}

void QtMaterialDialog::syncChildGeometry()
{
    const int panelWidth = qMin(m_spec.maxWidth, width());
    const int panelHeight = qMax(160, sizeHint().height());
    resize(panelWidth, panelHeight);

    const QWidget* host = hostWidget() ? hostWidget() : parentWidget();
    if (!host) {
        return;
    }

    move((host->width() - width()) / 2, (host->height() - height()) / 2);
}

void QtMaterialDialog::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (m_scrim && parentWidget()) {
        m_scrim->setGeometry(parentWidget()->rect());
    }
}

void QtMaterialDialog::paintEvent(QPaintEvent*)
{
    resolveSpecIfNeeded();
    syncChildGeometry();

    QPainter painter(this);
    QtMaterialSurfaceRenderHelper::paintFrame(&painter, QtMaterialSurfaceRenderHelper::dialogFrame(theme(), m_spec, rect()));
}

} // namespace QtMaterial
