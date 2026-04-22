#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"

#include <QKeyEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QShowEvent>
#include <QRegion>
#include <QWidget>
#include <QEvent>
#include <QMouseEvent>

#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/specs/qtmaterialbottomsheetspec.h"
#include "private/qtmaterialsurfacerenderhelper_p.h"

namespace QtMaterial {

QtMaterialBottomSheet::QtMaterialBottomSheet(QWidget *parent)
    : QtMaterialOverlaySurface(parent)
{
    setFocusPolicy(Qt::StrongFocus);

    m_container = new QWidget(this);
    m_container->setObjectName(QStringLiteral("qtmaterial_bottomsheet_container"));
    m_container->setAttribute(Qt::WA_StyledBackground, false);
    m_container->setAutoFillBackground(false);
    m_container->hide();

    m_transition = new QtMaterialTransitionController(this);
    m_transition->applyMotionToken(theme(), MotionToken::Medium2);

    m_scrim = new QtMaterialScrimWidget(parent ? parent : this);
    if (m_scrim) {
        m_scrim->hide();
    }

    if (QWidget *host = parentWidget()) {
        host->installEventFilter(this);
    }

    connect(m_transition, &QtMaterialTransitionController::progressChanged,
            this, [this](qreal value) {
                invalidateCachedGeometry();
                applySheetMask();
                emit progressChanged(value);
                syncScrim();
                update();
            });

    connect(m_transition, &QtMaterialTransitionController::finished,
            this, [this]() {
                if (m_state == SheetState::Closing) {
                    m_state = SheetState::Closed;
                    clearMask();
                    emit stateChanged(m_state);
                    hide();
                    if (m_container) {
                        m_container->hide();
                    }
                    if (m_scrim) {
                        m_scrim->hide();
                    }
                } else if (m_state == SheetState::Opening) {
                    m_state = SheetState::Open;
                    emit stateChanged(m_state);
                    focusFirstChild();
                }
            });
}

QtMaterialBottomSheet::~QtMaterialBottomSheet()
{
    if (QWidget *host = parentWidget()) {
        host->removeEventFilter(this);
    }
}

void QtMaterialBottomSheet::open()
{
    if (isVisible() && (m_state == SheetState::Opening || m_state == SheetState::Open)) {
        return;
    }

    ensureSpecResolved();
    if (m_specPtr) {
        m_transition->applyMotionToken(theme(), m_specPtr->motionToken);
    }

    syncToHost();
    show();
    raise();

    if (m_container) {
        m_container->show();
        m_container->raise();
    }

    if (m_scrim && m_modal) {
        m_scrim->show();
        m_scrim->raise();
    }

    syncContainerGeometry();

    raise();
    m_state = SheetState::Opening;
    emit stateChanged(m_state);
    m_transition->startForward();
}

void QtMaterialBottomSheet::close()
{
    if (m_state == SheetState::Closing || m_state == SheetState::Closed) {
        return;
    }

    ensureSpecResolved();
    if (m_specPtr) {
        m_transition->applyMotionToken(theme(), m_specPtr->motionToken);
    }

    m_state = SheetState::Closing;
    emit stateChanged(m_state);
    m_transition->startBackward();
}

bool QtMaterialBottomSheet::isOpen() const noexcept
{
    return m_state == SheetState::Open || m_state == SheetState::Opening;
}

void QtMaterialBottomSheet::setModal(bool modal)
{
    if (m_modal == modal) {
        return;
    }
    m_modal = modal;
    syncScrim();
}

bool QtMaterialBottomSheet::isModal() const noexcept
{
    return m_modal;
}

void QtMaterialBottomSheet::setExpandedHeight(int px)
{
    const int clamped = qMax(120, px);
    if (m_expandedHeight == clamped) {
        return;
    }

    m_expandedHeight = clamped;
    invalidateCachedGeometry();
    syncContainerGeometry();
    updateGeometry();
    update();
}

int QtMaterialBottomSheet::expandedHeight() const noexcept
{
    return m_expandedHeight;
}

QSize QtMaterialBottomSheet::sizeHint() const
{
    ensureSpecResolved();
    return QSize(640, qMax(m_expandedHeight, 240));
}

QSize QtMaterialBottomSheet::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(280, 120);
}

QWidget* QtMaterialBottomSheet::contentWidget() const noexcept
{
    return m_container;
}

void QtMaterialBottomSheet::paintEvent(QPaintEvent *)
{
    ensureSpecResolved();
    ensureGeometryResolved();

    if (!m_specPtr || m_cachedVisualRect.isEmpty()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_specPtr->containerColor);
    painter.drawPath(m_cachedContainerPath);
}

void QtMaterialBottomSheet::resizeEvent(QResizeEvent *event)
{
    QtMaterialOverlaySurface::resizeEvent(event);
    invalidateCachedGeometry();
    syncToHost();
    applySheetMask();
}

void QtMaterialBottomSheet::showEvent(QShowEvent *event)
{
    QtMaterialOverlaySurface::showEvent(event);
    syncToHost();
    applySheetMask();
    syncScrim();
}

void QtMaterialBottomSheet::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
    case Qt::Key_Back:
        close();
        event->accept();
        return;
    case Qt::Key_Down:
        if (event->modifiers() & Qt::ControlModifier) {
            close();
            event->accept();
            return;
        }
        break;
    default:
        break;
    }

    QtMaterialOverlaySurface::keyPressEvent(event);
}

void QtMaterialBottomSheet::themeChangedEvent(const QtMaterial::Theme &theme)
{
    QtMaterialOverlaySurface::themeChangedEvent(theme);

    m_specDirty = true;
    invalidateCachedGeometry();

    ensureSpecResolved();
    if (m_specPtr) {
        m_transition->applyMotionToken(this->theme(), m_specPtr->motionToken);
    }

    syncContainerGeometry();
    syncScrim();
    update();
}

void QtMaterialBottomSheet::ensureSpecResolved() const
{
    if (!m_specDirty && m_specPtr) {
        return;
    }

    static QtMaterial::SpecFactory factory;
    static QtMaterial::BottomSheetSpec spec;
    spec = factory.bottomSheetSpec(theme());

    m_specPtr = &spec;
    m_specDirty = false;
}

void QtMaterialBottomSheet::ensureGeometryResolved() const
{
    if (!m_geometryDirty) {
        return;
    }

    ensureSpecResolved();
    if (!m_specPtr) {
        return;
    }

    const QRect hostRect = rect();
    if (!hostRect.isValid()) {
        return;
    }

    const int minHeight = qMax(120, minimumSizeHint().height());
    const int sheetHeight = qMin(qMax(m_expandedHeight, minHeight), hostRect.height());

    const qreal progressValue = m_transition ? m_transition->progress() : 1.0;
    const qreal progress = qBound<qreal>(0.0, progressValue, 1.0);

    const int hiddenY = hostRect.height();
    const int y = hiddenY - qRound(sheetHeight * progress);

    m_cachedVisualRect = QRect(0, y, hostRect.width(), sheetHeight);
    m_cachedContentRect = m_cachedVisualRect.adjusted(0, m_specPtr->topPadding, 0, 0);
    m_cachedCornerRadius = 28.0; // TODO: dériver depuis shapeRole

    m_cachedContainerPath = QPainterPath();
    m_cachedContainerPath.addRoundedRect(m_cachedVisualRect,
                                         m_cachedCornerRadius,
                                         m_cachedCornerRadius);

    m_geometryDirty = false;
}

void QtMaterialBottomSheet::invalidateCachedGeometry()
{
    m_geometryDirty = true;
    m_cachedVisualRect = QRect();
    m_cachedContentRect = QRect();
    m_cachedContainerPath = QPainterPath();
    m_cachedCornerRadius = 0.0;
}

void QtMaterialBottomSheet::syncToHost()
{
    if (QWidget *host = parentWidget()) {
        setGeometry(host->rect());
        if (m_scrim) {
            m_scrim->setGeometry(host->rect());
        }
    }

    invalidateCachedGeometry();
    syncContainerGeometry();
}

void QtMaterialBottomSheet::syncScrim()
{
    if (!m_scrim) {
        return;
    }

    if (!m_modal || m_state == SheetState::Closed) {
        m_scrim->hide();
        return;
    }

    ensureSpecResolved();
    if (!m_specPtr) {
        return;
    }

    QColor scrim = m_specPtr->scrimColor;
    const qreal progressValue = m_transition ? m_transition->progress() : 1.0;
    const qreal progress = qBound<qreal>(0.0, progressValue, 1.0);
    scrim.setAlphaF(qBound<qreal>(0.0, scrim.alphaF() * progress, 1.0));

    m_scrim->setGeometry(rect());
    m_scrim->setScrimColor(scrim);

    if (!m_scrim->isVisible()) {
        m_scrim->show();
    }

    m_scrim->raise();
}

void QtMaterialBottomSheet::syncContainerGeometry()
{
    if (!m_container) {
        return;
    }

    ensureGeometryResolved();
    if (m_cachedVisualRect.isEmpty()) {
        m_container->hide();
        return;
    }

    m_container->setGeometry(m_cachedVisualRect);
    applyContainerClip();

    if (isVisible() && !m_container->isVisible()) {
        m_container->show();
    }

    m_container->raise();
}

void QtMaterialBottomSheet::applyContainerClip()
{
    if (!m_container) {
        return;
    }

    ensureGeometryResolved();
    if (m_cachedVisualRect.isEmpty()) {
        m_container->clearMask();
        return;
    }

    const QPainterPath localPath =
        m_cachedContainerPath.translated(-m_cachedVisualRect.topLeft());

    const QRegion maskRegion(localPath.toFillPolygon().toPolygon());
    m_container->setMask(maskRegion);
}

void QtMaterialBottomSheet::focusFirstChild()
{
    if (m_container) {
        const auto children = m_container->findChildren<QWidget*>();
        for (QWidget *child : children) {
            if (!child || !child->isVisible() || !child->isEnabled()) {
                continue;
            }
            if (child->focusPolicy() != Qt::NoFocus) {
                child->setFocus(Qt::OtherFocusReason);
                return;
            }
        }
    }

    setFocus(Qt::OtherFocusReason);
}

QRect QtMaterialBottomSheet::sheetVisualRect() const
{
    ensureGeometryResolved();
    return m_cachedVisualRect;
}

QRect QtMaterialBottomSheet::contentRect() const
{
    ensureGeometryResolved();
    return m_cachedContentRect;
}

qreal QtMaterialBottomSheet::cornerRadius() const
{
    ensureGeometryResolved();
    return m_cachedCornerRadius;
}

qreal QtMaterialBottomSheet::progress() const noexcept
{
    return m_transition ? m_transition->progress() : 0.0;
}

QtMaterialBottomSheet::SheetState QtMaterialBottomSheet::state() const noexcept
{
    return m_state;
}

void QtMaterialBottomSheet::applySheetMask()
{
    ensureGeometryResolved();

    if (m_cachedVisualRect.isEmpty()) {
        clearMask();
        return;
    }

    const QRegion panelRegion(m_cachedContainerPath.toFillPolygon().toPolygon());
    setMask(panelRegion);
}

void QtMaterialBottomSheet::mousePressEvent(QMouseEvent *event)
{
    ensureGeometryResolved();

    if (m_cachedVisualRect.contains(event->pos())) {
        event->ignore();
        QtMaterialOverlaySurface::mousePressEvent(event);
        return;
    }

    close();
    event->accept();
}

bool QtMaterialBottomSheet::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parentWidget()) {
        switch (event->type()) {
        case QEvent::Resize:
        case QEvent::Move:
        case QEvent::Show:
        case QEvent::WindowStateChange:
            syncToHost();
            syncScrim();
            update();
            break;
        default:
            break;
        }
    }

    return QtMaterialOverlaySurface::eventFilter(watched, event);
}

} // namespace QtMaterial