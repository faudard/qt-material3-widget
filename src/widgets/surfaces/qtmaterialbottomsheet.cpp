#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"

#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPointer>
#include <QResizeEvent>
#include <QShowEvent>
#include <QWidget>
#include <QtGlobal>

#include "qtmaterial/effects/qtmaterialscrimwidget.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialbottomsheetspec.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace QtMaterial {
namespace {

QPainterPath topRoundedSheetPath(const QRectF &r, qreal radius)
{
    QPainterPath path;
    if (!r.isValid()) {
        return path;
    }

    const qreal rr = qMin(radius, r.height() / 2.0);
    const qreal left = r.left();
    const qreal top = r.top();
    const qreal right = r.right();
    const qreal bottom = r.bottom();

    path.moveTo(left, bottom);
    path.lineTo(left, top + rr);
    path.quadTo(left, top, left + rr, top);
    path.lineTo(right - rr, top);
    path.quadTo(right, top, right, top + rr);
    path.lineTo(right, bottom);
    path.lineTo(left, bottom);
    path.closeSubpath();
    return path;
}

QPoint globalMousePosition(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->globalPosition().toPoint();
#else
    return event->globalPos();
#endif
}

} // namespace

QtMaterialBottomSheet::QtMaterialBottomSheet(QWidget *parent)
    : QtMaterialOverlaySurface(parent)
{
    setObjectName(QStringLiteral("qtmaterial_bottomsheet"));
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_StyledBackground, false);
    setAutoFillBackground(false);

    m_container = new QWidget(this);
    m_container->setObjectName(QStringLiteral("qtmaterial_bottomsheet_container"));
    m_container->setAttribute(Qt::WA_StyledBackground, false);
    m_container->setAutoFillBackground(false);
    m_container->hide();

    m_transition = new QtMaterialTransitionController(this);
    m_transition->applyMotionToken(theme(), MotionToken::Medium2);

    m_scrim = new QtMaterialScrimWidget(parent ? parent : this);
    if (m_scrim) {
        m_scrim->setObjectName(QStringLiteral("qtmaterial_bottomsheet_scrim"));
        m_scrim->hide();
        m_scrim->installEventFilter(this);
    }

    if (QWidget *host = parentWidget()) {
        host->installEventFilter(this);
    }

    connect(m_transition, &QtMaterialTransitionController::progressChanged, this, [this](qreal value) {
        invalidateCachedGeometry();
        syncContainerGeometry();
        applySheetMask();
        emit progressChanged(value);
        syncScrim();
        update();
    });

    connect(m_transition, &QtMaterialTransitionController::finished, this, [this]() {
        if (m_state == SheetState::Closing) {
            setState(SheetState::Closed);
            clearMask();
            hide();
            if (m_container) {
                m_container->hide();
            }
            if (m_scrim) {
                m_scrim->hide();
            }
            if (m_restoreFocusOnClose && m_lastFocusBeforeOpen) {
                m_lastFocusBeforeOpen->setFocus(Qt::OtherFocusReason);
            }
            emit closed();
        } else if (m_state == SheetState::Opening) {
            setState(SheetState::Open);
            focusFirstChild();
            emit opened();
        }
    });

    syncAccessibility();
}

QtMaterialBottomSheet::~QtMaterialBottomSheet()
{
    if (QWidget *host = parentWidget()) {
        host->removeEventFilter(this);
    }
    if (m_scrim) {
        m_scrim->removeEventFilter(this);
    }
}

void QtMaterialBottomSheet::open()
{
    if (isVisible() && (m_state == SheetState::Opening || m_state == SheetState::Open)) {
        return;
    }

    m_lastFocusBeforeOpen = QApplication::focusWidget();
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
    applySheetMask();
    raise();
    setState(SheetState::Opening);
    m_transition->startForward();
    syncAccessibility();
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

    setState(SheetState::Closing);
    emit dismissed();
    m_transition->startBackward();
    syncAccessibility();
}

bool QtMaterialBottomSheet::isOpen() const noexcept
{
    return m_state == SheetState::Open || m_state == SheetState::Opening;
}

qreal QtMaterialBottomSheet::progress() const noexcept
{
    return m_transition ? m_transition->progress() : 0.0;
}

QtMaterialBottomSheet::SheetState QtMaterialBottomSheet::state() const noexcept
{
    return m_state;
}

void QtMaterialBottomSheet::setModal(bool modal)
{
    if (m_modal == modal) {
        return;
    }

    m_modal = modal;
    syncScrim();
    applySheetMask();
    syncAccessibility();
    emit modalChanged(m_modal);
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
    if (m_collapsedHeight > m_expandedHeight) {
        m_collapsedHeight = m_expandedHeight;
        emit collapsedHeightChanged(m_collapsedHeight);
    }
    invalidateCachedGeometry();
    syncContainerGeometry();
    applySheetMask();
    updateGeometry();
    update();
    syncAccessibility();
    emit expandedHeightChanged(m_expandedHeight);
}

int QtMaterialBottomSheet::expandedHeight() const noexcept
{
    return m_expandedHeight;
}

void QtMaterialBottomSheet::setCollapsedHeight(int px)
{
    const int clamped = qBound(56, px, m_expandedHeight);
    if (m_collapsedHeight == clamped) {
        return;
    }

    m_collapsedHeight = clamped;
    invalidateCachedGeometry();
    syncContainerGeometry();
    applySheetMask();
    updateGeometry();
    update();
    syncAccessibility();
    emit collapsedHeightChanged(m_collapsedHeight);
}

int QtMaterialBottomSheet::collapsedHeight() const noexcept
{
    return m_collapsedHeight;
}

void QtMaterialBottomSheet::setExpanded(bool expanded)
{
    if (m_expanded == expanded) {
        return;
    }

    m_expanded = expanded;
    invalidateCachedGeometry();
    syncContainerGeometry();
    applySheetMask();
    updateGeometry();
    update();
    syncAccessibility();
    emit expandedChanged(m_expanded);
}

bool QtMaterialBottomSheet::isExpanded() const noexcept
{
    return m_expanded;
}

void QtMaterialBottomSheet::expand()
{
    setExpanded(true);
}

void QtMaterialBottomSheet::collapse()
{
    setExpanded(false);
}

void QtMaterialBottomSheet::setDismissOnEscape(bool enabled)
{
    if (m_dismissOnEscape == enabled) {
        return;
    }
    m_dismissOnEscape = enabled;
    emit dismissOnEscapeChanged(enabled);
}

bool QtMaterialBottomSheet::dismissOnEscape() const noexcept
{
    return m_dismissOnEscape;
}

void QtMaterialBottomSheet::setDismissOnScrim(bool enabled)
{
    if (m_dismissOnScrim == enabled) {
        return;
    }
    m_dismissOnScrim = enabled;
    emit dismissOnScrimChanged(enabled);
}

bool QtMaterialBottomSheet::dismissOnScrim() const noexcept
{
    return m_dismissOnScrim;
}

void QtMaterialBottomSheet::setDragToDismissEnabled(bool enabled)
{
    if (m_dragToDismissEnabled == enabled) {
        return;
    }
    m_dragToDismissEnabled = enabled;
    emit dragToDismissEnabledChanged(enabled);
}

bool QtMaterialBottomSheet::isDragToDismissEnabled() const noexcept
{
    return m_dragToDismissEnabled;
}

void QtMaterialBottomSheet::setRestoreFocusOnClose(bool enabled)
{
    if (m_restoreFocusOnClose == enabled) {
        return;
    }
    m_restoreFocusOnClose = enabled;
    emit restoreFocusOnCloseChanged(enabled);
}

bool QtMaterialBottomSheet::restoreFocusOnClose() const noexcept
{
    return m_restoreFocusOnClose;
}

void QtMaterialBottomSheet::setInitialFocusWidget(QWidget *widget)
{
    if (m_initialFocusWidget == widget) {
        return;
    }
    m_initialFocusWidget = widget;
}

QWidget *QtMaterialBottomSheet::initialFocusWidget() const noexcept
{
    return m_initialFocusWidget.data();
}

QSize QtMaterialBottomSheet::sizeHint() const
{
    return QSize(640, qMax(m_expandedHeight, 240));
}

QSize QtMaterialBottomSheet::minimumSizeHint() const
{
    return QSize(280, 120);
}

QWidget *QtMaterialBottomSheet::contentWidget() const noexcept
{
    return m_container;
}

void QtMaterialBottomSheet::setTitleText(const QString &text)
{
    if (m_titleText == text) {
        return;
    }
    m_titleText = text;
    syncAccessibility();
    emit titleTextChanged(m_titleText);
}

QString QtMaterialBottomSheet::titleText() const
{
    return m_titleText;
}

void QtMaterialBottomSheet::setSupportingText(const QString &text)
{
    if (m_supportingText == text) {
        return;
    }
    m_supportingText = text;
    syncAccessibility();
    emit supportingTextChanged(m_supportingText);
}

QString QtMaterialBottomSheet::supportingText() const
{
    return m_supportingText;
}

QString QtMaterialBottomSheet::accessibilitySummary() const
{
    QStringList parts;
    if (!m_titleText.trimmed().isEmpty()) {
        parts << m_titleText.trimmed();
    } else {
        parts << tr("Bottom sheet");
    }
    if (!m_supportingText.trimmed().isEmpty()) {
        parts << m_supportingText.trimmed();
    }
    parts << (m_expanded ? tr("Expanded") : tr("Collapsed"));
    if (m_modal) {
        parts << tr("Modal");
    }
    return parts.join(QStringLiteral(", "));
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
        case QEvent::MouseButtonPress:
            if (!isOpen() || m_modal || !m_dismissOnScrim) {
                break;
            }
            ensureGeometryResolved();
            if (const auto *mouseEvent = static_cast<QMouseEvent *>(event)) {
                if (!m_cachedVisualRect.contains(mouseEvent->pos())) {
                    close();
                    return true;
                }
            }
            break;
        default:
            break;
        }
    }

    if (watched == m_scrim && event->type() == QEvent::MouseButtonPress) {
        if (isOpen() && m_dismissOnScrim) {
            close();
            return true;
        }
    }

    return QtMaterialOverlaySurface::eventFilter(watched, event);
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
        if (m_dismissOnEscape) {
            close();
            event->accept();
            return;
        }
        break;
    case Qt::Key_Down:
        if (event->modifiers() & Qt::ControlModifier) {
            if (m_expanded) {
                collapse();
            } else {
                close();
            }
            event->accept();
            return;
        }
        break;
    case Qt::Key_Up:
        if (event->modifiers() & Qt::ControlModifier) {
            expand();
            event->accept();
            return;
        }
        break;
    default:
        break;
    }

    QtMaterialOverlaySurface::keyPressEvent(event);
}

void QtMaterialBottomSheet::mousePressEvent(QMouseEvent *event)
{
    if (m_dragToDismissEnabled && event->button() == Qt::LeftButton) {
        ensureGeometryResolved();
        if (m_cachedVisualRect.contains(event->pos())) {
            m_dragging = true;
            m_dragStartGlobalPos = globalMousePosition(event);
            event->accept();
            return;
        }
    }
    QtMaterialOverlaySurface::mousePressEvent(event);
}

void QtMaterialBottomSheet::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        event->accept();
        return;
    }
    QtMaterialOverlaySurface::mouseMoveEvent(event);
}

void QtMaterialBottomSheet::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_dragging) {
        const int deltaY = globalMousePosition(event).y() - m_dragStartGlobalPos.y();
        m_dragging = false;
        if (deltaY > qMax(48, effectiveSheetHeight() / 4)) {
            if (m_expanded && m_collapsedHeight < m_expandedHeight) {
                collapse();
            } else {
                close();
            }
        } else if (deltaY < -32) {
            expand();
        }
        event->accept();
        return;
    }
    QtMaterialOverlaySurface::mouseReleaseEvent(event);
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
    applySheetMask();
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
    const int sheetHeight = qMin(qMax(effectiveSheetHeight(), minHeight), hostRect.height());
    const qreal progressValue = m_transition ? m_transition->progress() : 1.0;
    const qreal p = qBound(0.0, progressValue, 1.0);
    const int hiddenY = hostRect.height();
    const int y = hiddenY - qRound(sheetHeight * p);

    m_cachedVisualRect = QRect(0, y, hostRect.width(), sheetHeight);
    m_cachedContentRect = m_cachedVisualRect.adjusted(0, m_specPtr->topPadding, 0, 0);

    const int specRadius = theme().shapes().radius(m_specPtr->shapeRole);
    const qreal resolvedRadius = specRadius > 0 ? qreal(specRadius) : 28.0;
    m_cachedCornerRadius = qMin(resolvedRadius, sheetHeight / 2.0);
    m_cachedContainerPath = topRoundedSheetPath(QRectF(m_cachedVisualRect), m_cachedCornerRadius);
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
    const qreal p = qBound(0.0, progressValue, 1.0);
    scrim.setAlphaF(qBound(0.0, scrim.alphaF() * p, 1.0));
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

    const QPainterPath localPath = m_cachedContainerPath.translated(-m_cachedVisualRect.topLeft());
    const QRegion maskRegion(localPath.toFillPolygon().toPolygon());
    m_container->setMask(maskRegion);
}

void QtMaterialBottomSheet::applySheetMask()
{
    ensureGeometryResolved();
    if (m_cachedVisualRect.isEmpty()) {
        clearMask();
        return;
    }

    if (m_modal) {
        clearMask();
        return;
    }

    const QRegion panelRegion(m_cachedContainerPath.toFillPolygon().toPolygon());
    setMask(panelRegion);
}

void QtMaterialBottomSheet::focusFirstChild()
{
    if (m_initialFocusWidget && m_initialFocusWidget->isVisible() && m_initialFocusWidget->isEnabled()) {
        m_initialFocusWidget->setFocus(Qt::OtherFocusReason);
        return;
    }

    if (m_container) {
        const auto children = m_container->findChildren<QWidget *>();
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

void QtMaterialBottomSheet::syncAccessibility()
{
    const QString summary = accessibilitySummary();
    if (accessibleName().trimmed().isEmpty()) {
        setAccessibleName(m_titleText.trimmed().isEmpty() ? tr("Bottom sheet") : m_titleText.trimmed());
    }
    setAccessibleDescription(summary);

    if (m_lastAccessibilitySummary != summary) {
        m_lastAccessibilitySummary = summary;
        emit accessibilitySummaryChanged(summary);
    }
}

void QtMaterialBottomSheet::setState(SheetState state)
{
    if (m_state == state) {
        return;
    }
    m_state = state;
    emit stateChanged(m_state);
}

int QtMaterialBottomSheet::effectiveSheetHeight() const
{
    return m_expanded ? m_expandedHeight : m_collapsedHeight;
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

} // namespace QtMaterial
