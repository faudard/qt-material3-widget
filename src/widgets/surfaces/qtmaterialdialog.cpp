#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"
#include <QPainter>
#include <QResizeEvent>
#include <QVBoxLayout>
#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include "qtmaterial/effects/qtmaterialshadowrenderer.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
namespace QtMaterial {
QtMaterialDialog::QtMaterialDialog(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
    , m_layout(new QVBoxLayout(this))
    , m_scrim(new QtMaterialScrimWidget(parent ? parent : this))
    , m_transition(new QtMaterialTransitionController(this))
{
    m_layout->setContentsMargins(24, 24, 24, 24);
    hide();
    if (m_scrim) m_scrim->hide();
}
QtMaterialDialog::~QtMaterialDialog() = default;
void QtMaterialDialog::setBodyWidget(QWidget* widget)
{
    if (m_bodyWidget == widget) return;
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
QWidget* QtMaterialDialog::bodyWidget() const { return m_bodyWidget; }
void QtMaterialDialog::open() { if (m_scrim) { m_scrim->setGeometry(parentWidget() ? parentWidget()->rect() : rect()); m_scrim->show(); } show(); raise(); if (m_transition) m_transition->startForward(); }
void QtMaterialDialog::close() { if (m_transition) m_transition->startBackward(); hide(); if (m_scrim) m_scrim->hide(); }
void QtMaterialDialog::themeChangedEvent(const Theme& theme) { QtMaterialOverlaySurface::themeChangedEvent(theme); m_specDirty = true; }
void QtMaterialDialog::invalidateResolvedSpec() { m_specDirty = true; }
void QtMaterialDialog::resolveSpecIfNeeded() const
{
    if (!m_specDirty) return;
    SpecFactory factory;
    m_spec = factory.dialogSpec(theme());
    m_specDirty = false;
}
void QtMaterialDialog::syncChildGeometry()
{
    const int w = qMin(m_spec.maxWidth, width());
    resize(w, qMax(160, sizeHint().height()));
    move((parentWidget() ? parentWidget()->width() : width()) / 2 - this->width() / 2,
         (parentWidget() ? parentWidget()->height() : height()) / 2 - this->height() / 2);
}
void QtMaterialDialog::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (m_scrim && parentWidget()) m_scrim->setGeometry(parentWidget()->rect());
}
void QtMaterialDialog::paintEvent(QPaintEvent*)
{
    resolveSpecIfNeeded();
    syncChildGeometry();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QRectF panel = rect().adjusted(2, 2, -2, -2);
    QtMaterialShadowRenderer::paintRoundedShadow(&painter, panel, 28.0, QColor(0, 0, 0, 80), 12, 4);
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_spec.containerColor);
    painter.drawRoundedRect(panel, 28.0, 28.0);
}
} // namespace QtMaterial
