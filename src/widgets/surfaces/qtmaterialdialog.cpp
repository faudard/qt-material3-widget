#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"

#include <QEvent>
#include <QKeyEvent>
#include <QLayoutItem>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QVBoxLayout>

#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include "qtmaterial/effects/qtmaterialshadowrenderer.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {
namespace {

QSize boundedDialogSize(const QSize& desired, const QSize& hostSize)
{
    const int maxWidth = qMax(160, hostSize.width() - 32);
    const int maxHeight = qMax(120, hostSize.height() - 32);
    return QSize(qMin(desired.width(), maxWidth), qMin(desired.height(), maxHeight));
}

} // namespace

QtMaterialDialog::QtMaterialDialog(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
    , m_layout(new QVBoxLayout(this))
    , m_scrim(new QtMaterialScrimWidget(parent ? parent : this))
    , m_transition(new QtMaterialTransitionController(this))
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    m_layout->setSpacing(0);
    m_layout->setContentsMargins(24, 24, 24, 24);

    if (parentWidget()) {
        setHostWidget(parentWidget());
    }

    if (m_scrim) {
        m_scrim->hide();
        if (parentWidget()) {
            m_scrim->setGeometry(parentWidget()->rect());
        }
    }

    if (m_transition) {
        m_transition->setDuration(220);
        QObject::connect(m_transition, &QtMaterialTransitionController::progressChanged, this, [this](qreal) {
            updateScrimColor();
            update();
        });
        QObject::connect(m_transition, &QtMaterialTransitionController::finished, this, [this]() {
            if (m_transition && qFuzzyIsNull(m_transition->progress())) {
                QWidget::hide();
                if (m_scrim) {
                    m_scrim->hide();
                }
            }
        });
    }

    QWidget::hide();
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
        m_layout->addWidget(m_bodyWidget, 1);
    }

    updateGeometry();
    if (isVisible()) {
        syncGeometryToHost();
    }
}

QWidget* QtMaterialDialog::bodyWidget() const
{
    return m_bodyWidget.data();
}

QSize QtMaterialDialog::sizeHint() const
{
    const_cast<QtMaterialDialog*>(this)->resolveSpecIfNeeded();

    const QSize bodySize = m_bodyWidget ? m_bodyWidget->sizeHint() : QSize(320, 120);
    const int width = qMin(m_spec.maxWidth, qMax(280, bodySize.width() + (m_spec.padding * 2)));
    const int height = qMax(120, bodySize.height() + (m_spec.padding * 2));
    return QSize(width, height);
}

QSize QtMaterialDialog::minimumSizeHint() const
{
    const_cast<QtMaterialDialog*>(this)->resolveSpecIfNeeded();
    const QSize bodySize = m_bodyWidget ? m_bodyWidget->minimumSizeHint() : QSize(160, 80);
    const int width = qMin(m_spec.maxWidth, qMax(240, bodySize.width() + (m_spec.padding * 2)));
    const int height = qMax(120, bodySize.height() + (m_spec.padding * 2));
    return QSize(width, height);
}

void QtMaterialDialog::open()
{
    resolveSpecIfNeeded();

    if (parentWidget() && hostWidget() != parentWidget()) {
        setHostWidget(parentWidget());
    }

    if (m_scrim && parentWidget()) {
        if (m_scrim->parentWidget() != parentWidget()) {
            m_scrim->setParent(parentWidget());
        }
        m_scrim->setGeometry(parentWidget()->rect());
        updateScrimColor();
        m_scrim->show();
        m_scrim->lower();
    }

    applyLayoutMargins();
    syncGeometryToHost();
    QWidget::show();
    raise();
    activateWindow();
    setFocus(Qt::OtherFocusReason);

    if (m_transition) {
        m_transition->startForward();
    } else {
        update();
    }
}

void QtMaterialDialog::close()
{
    if (m_transition) {
        m_transition->startBackward();
        return;
    }

    QWidget::hide();
    if (m_scrim) {
        m_scrim->hide();
    }
}

void QtMaterialDialog::themeChangedEvent(const Theme& theme)
{
    QtMaterialOverlaySurface::themeChangedEvent(theme);
    Q_UNUSED(theme)
    m_specDirty = true;
    applyLayoutMargins();
    updateScrimColor();
    updateGeometry();
    if (isVisible()) {
        syncGeometryToHost();
    }
}

void QtMaterialDialog::invalidateResolvedSpec()
{
    m_specDirty = true;
    updateGeometry();
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

void QtMaterialDialog::applyLayoutMargins()
{
    resolveSpecIfNeeded();
    if (m_layout) {
        m_layout->setContentsMargins(m_spec.padding, m_spec.padding, m_spec.padding, m_spec.padding);
    }
}

void QtMaterialDialog::updateScrimColor()
{
    if (!m_scrim) {
        return;
    }

    resolveSpecIfNeeded();
    QColor scrimColor = m_spec.scrimColor;
    const qreal progress = m_transition ? m_transition->progress() : 1.0;
    scrimColor.setAlphaF(qBound<qreal>(0.0, scrimColor.alphaF() * progress, 1.0));
    m_scrim->setScrimColor(scrimColor);
}

QRectF QtMaterialDialog::panelRect() const
{
    return rect().adjusted(6, 6, -6, -6);
}

qreal QtMaterialDialog::panelRadius() const
{
    resolveSpecIfNeeded();
    return theme().shapes().radius(m_spec.shapeRole);
}

void QtMaterialDialog::syncGeometryToHost()
{
    QWidget* host = hostWidget() ? hostWidget() : parentWidget();
    if (!host) {
        return;
    }

    resolveSpecIfNeeded();
    const QSize desired = boundedDialogSize(sizeHint(), host->size());
    const QRect aligned = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, desired, host->rect());
    setGeometry(aligned);

    if (m_scrim) {
        if (m_scrim->parentWidget() != host) {
            m_scrim->setParent(host);
        }
        m_scrim->setGeometry(host->rect());
        if (isVisible()) {
            m_scrim->show();
            m_scrim->lower();
        }
    }
}

void QtMaterialDialog::resizeEvent(QResizeEvent* event)
{
    QtMaterialOverlaySurface::resizeEvent(event);
    applyLayoutMargins();
}

void QtMaterialDialog::keyPressEvent(QKeyEvent* event)
{
    if (event && event->key() == Qt::Key_Escape) {
        event->accept();
        close();
        return;
    }
    QtMaterialOverlaySurface::keyPressEvent(event);
}

void QtMaterialDialog::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    resolveSpecIfNeeded();

    const QRectF panel = panelRect();
    const qreal radius = panelRadius();
    const qreal progress = m_transition ? m_transition->progress() : 1.0;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.save();
    painter.translate(0.0, (1.0 - progress) * 12.0);
    painter.setOpacity(progress);

    QtMaterialShadowRenderer::paintRoundedShadow(
        &painter,
        panel,
        radius,
        QColor(0, 0, 0, 72),
        12,
        4
    );

    painter.setPen(Qt::NoPen);
    painter.setBrush(m_spec.containerColor);
    painter.drawRoundedRect(panel, radius, radius);
    painter.restore();
}

} // namespace QtMaterial
