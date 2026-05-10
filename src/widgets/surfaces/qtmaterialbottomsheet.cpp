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
#include <memory>
#include <QVariant>

namespace QtMaterial {

class QtMaterialBottomSheetPrivate {
public:
 mutable bool specDirty = true;
 mutable bool geometryDirty = true;
 mutable QtMaterial::BottomSheetSpec* specPtr = nullptr;
 mutable QRect cachedVisualRect;
 mutable QRect cachedContentRect;
 mutable QPainterPath cachedContainerPath;
 mutable qreal cachedCornerRadius = 0.0;
 QPointer<QtMaterialScrimWidget> scrim;
 QPointer<QtMaterialTransitionController> transition;
 QPointer<QWidget> container;
 QPointer<QWidget> initialFocusWidget;
 QPointer<QWidget> lastFocusBeforeOpen;
 QtMaterialBottomSheet::SheetState state = QtMaterialBottomSheet::SheetState::Closed;
 bool modal = true;
 bool expanded = true;
 bool dismissOnEscape = true;
 bool dismissOnScrim = true;
 bool dragToDismissEnabled = true;
 bool restoreFocusOnClose = true;
 bool dragging = false;
 int expandedHeight = 320;
 int collapsedHeight = 80;
 QPoint dragStartGlobalPos;
 QString titleText;
 QString supportingText;
 QString lastAccessibilitySummary;
 void invalidateCachedGeometry();
 int effectiveSheetHeight() const noexcept;
};

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
    , d_ptr(std::make_unique<QtMaterialBottomSheetPrivate>())
{
    setObjectName(QStringLiteral("qtmaterial_bottomsheet"));
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_StyledBackground, false);
    setAutoFillBackground(false);

    d_ptr->container = new QWidget(this);
    d_ptr->container->setObjectName(QStringLiteral("qtmaterial_bottomsheet_container"));
    d_ptr->container->setAttribute(Qt::WA_StyledBackground, false);
    d_ptr->container->setAutoFillBackground(false);
    d_ptr->container->hide();

    d_ptr->transition = new QtMaterialTransitionController(this);
    d_ptr->transition->applyMotionToken(theme(), MotionToken::Medium2);

    d_ptr->scrim = new QtMaterialScrimWidget(parent ? parent : this);
    if (d_ptr->scrim) {
        d_ptr->scrim->setObjectName(QStringLiteral("qtmaterial_bottomsheet_scrim"));
        d_ptr->scrim->hide();
        d_ptr->scrim->installEventFilter(this);
    }

    if (QWidget *host = parentWidget()) {
        host->installEventFilter(this);
    }

    connect(d_ptr->transition, &QtMaterialTransitionController::progressChanged, this, [this](qreal value) {
        d_ptr->invalidateCachedGeometry();
        syncContainerGeometry();
        applySheetMask();
        emit progressChanged(value);
        syncScrim();
        update();
    });

    connect(d_ptr->transition, &QtMaterialTransitionController::finished, this, [this]() {
        if (d_ptr->state == SheetState::Closing) {
            setState(SheetState::Closed);
            clearMask();
            hide();
            if (d_ptr->container) {
                d_ptr->container->hide();
            }
            if (d_ptr->scrim) {
                d_ptr->scrim->hide();
            }
            if (d_ptr->restoreFocusOnClose && d_ptr->lastFocusBeforeOpen) {
                d_ptr->lastFocusBeforeOpen->setFocus(Qt::OtherFocusReason);
            }
            emit closed();
        } else if (d_ptr->state == SheetState::Opening) {
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
    if (d_ptr->scrim) {
        d_ptr->scrim->removeEventFilter(this);
    }
}

void QtMaterialBottomSheet::open()
{
    if (isVisible() && (d_ptr->state == SheetState::Opening || d_ptr->state == SheetState::Open)) {
        return;
    }

    d_ptr->lastFocusBeforeOpen = QApplication::focusWidget();
    ensureSpecResolved();
    if (d_ptr->specPtr) {
        d_ptr->transition->applyMotionToken(theme(), d_ptr->specPtr->motionToken);
    }

    syncToHost();
    show();
    raise();

    if (d_ptr->container) {
        d_ptr->container->show();
        d_ptr->container->raise();
    }
    if (d_ptr->scrim && d_ptr->modal) {
        d_ptr->scrim->show();
        d_ptr->scrim->raise();
    }

    syncContainerGeometry();
    applySheetMask();
    raise();
    setState(SheetState::Opening);
    d_ptr->transition->startForward();
    syncAccessibility();
}

void QtMaterialBottomSheet::close()
{
    if (d_ptr->state == SheetState::Closing || d_ptr->state == SheetState::Closed) {
        return;
    }

    ensureSpecResolved();
    if (d_ptr->specPtr) {
        d_ptr->transition->applyMotionToken(theme(), d_ptr->specPtr->motionToken);
    }

    setState(SheetState::Closing);
    emit dismissed();
    d_ptr->transition->startBackward();
    syncAccessibility();
}

bool QtMaterialBottomSheet::isOpen() const noexcept
{
    return d_ptr->state == SheetState::Open || d_ptr->state == SheetState::Opening;
}

qreal QtMaterialBottomSheet::progress() const noexcept
{
    return d_ptr->transition ? d_ptr->transition->progress() : 0.0;
}

QtMaterialBottomSheet::SheetState QtMaterialBottomSheet::state() const noexcept
{
    return d_ptr->state;
}

void QtMaterialBottomSheet::setModal(bool modal)
{
    if (d_ptr->modal == modal) {
        return;
    }

    d_ptr->modal = modal;
    syncScrim();
    applySheetMask();
    syncAccessibility();
    emit modalChanged(d_ptr->modal);
}

bool QtMaterialBottomSheet::isModal() const noexcept
{
    return d_ptr->modal;
}

void QtMaterialBottomSheet::setExpandedHeight(int px)
{
    const int clamped = qMax(120, px);
    if (d_ptr->expandedHeight == clamped) {
        return;
    }

    d_ptr->expandedHeight = clamped;
    if (d_ptr->collapsedHeight > d_ptr->expandedHeight) {
        d_ptr->collapsedHeight = d_ptr->expandedHeight;
        emit collapsedHeightChanged(d_ptr->collapsedHeight);
    }
    d_ptr->invalidateCachedGeometry();
    syncContainerGeometry();
    applySheetMask();
    updateGeometry();
    update();
    syncAccessibility();
    emit expandedHeightChanged(d_ptr->expandedHeight);
}

int QtMaterialBottomSheet::expandedHeight() const noexcept
{
    return d_ptr->expandedHeight;
}

void QtMaterialBottomSheet::setCollapsedHeight(int px)
{
    const int clamped = qBound(56, px, d_ptr->expandedHeight);
    if (d_ptr->collapsedHeight == clamped) {
        return;
    }

    d_ptr->collapsedHeight = clamped;
    d_ptr->invalidateCachedGeometry();
    syncContainerGeometry();
    applySheetMask();
    updateGeometry();
    update();
    syncAccessibility();
    emit collapsedHeightChanged(d_ptr->collapsedHeight);
}

int QtMaterialBottomSheet::collapsedHeight() const noexcept
{
    return d_ptr->collapsedHeight;
}

void QtMaterialBottomSheet::setExpanded(bool expanded)
{
    if (d_ptr->expanded == expanded) {
        return;
    }

    d_ptr->expanded = expanded;
    d_ptr->invalidateCachedGeometry();
    syncContainerGeometry();
    applySheetMask();
    updateGeometry();
    update();
    syncAccessibility();
    emit expandedChanged(d_ptr->expanded);
}

bool QtMaterialBottomSheet::isExpanded() const noexcept
{
    return d_ptr->expanded;
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
    if (d_ptr->dismissOnEscape == enabled) {
        return;
    }
    d_ptr->dismissOnEscape = enabled;
    emit dismissOnEscapeChanged(enabled);
}

bool QtMaterialBottomSheet::dismissOnEscape() const noexcept
{
    return d_ptr->dismissOnEscape;
}

void QtMaterialBottomSheet::setDismissOnScrim(bool enabled)
{
    if (d_ptr->dismissOnScrim == enabled) {
        return;
    }
    d_ptr->dismissOnScrim = enabled;
    emit dismissOnScrimChanged(enabled);
}

bool QtMaterialBottomSheet::dismissOnScrim() const noexcept
{
    return d_ptr->dismissOnScrim;
}

void QtMaterialBottomSheet::setDragToDismissEnabled(bool enabled)
{
    if (d_ptr->dragToDismissEnabled == enabled) {
        return;
    }
    d_ptr->dragToDismissEnabled = enabled;
    emit dragToDismissEnabledChanged(enabled);
}

bool QtMaterialBottomSheet::isDragToDismissEnabled() const noexcept
{
    return d_ptr->dragToDismissEnabled;
}

void QtMaterialBottomSheet::setRestoreFocusOnClose(bool enabled)
{
    if (d_ptr->restoreFocusOnClose == enabled) {
        return;
    }
    d_ptr->restoreFocusOnClose = enabled;
    emit restoreFocusOnCloseChanged(enabled);
}

bool QtMaterialBottomSheet::restoreFocusOnClose() const noexcept
{
    return d_ptr->restoreFocusOnClose;
}

void QtMaterialBottomSheet::setInitialFocusWidget(QWidget *widget)
{
    if (d_ptr->initialFocusWidget == widget) {
        return;
    }
    d_ptr->initialFocusWidget = widget;
}

QWidget *QtMaterialBottomSheet::initialFocusWidget() const noexcept
{
    return d_ptr->initialFocusWidget.data();
}

QSize QtMaterialBottomSheet::sizeHint() const
{
    return QSize(640, qMax(d_ptr->expandedHeight, 240));
}

QSize QtMaterialBottomSheet::minimumSizeHint() const
{
    return QSize(280, 120);
}

QWidget *QtMaterialBottomSheet::contentWidget() const noexcept
{
    return d_ptr->container;
}

void QtMaterialBottomSheet::setTitleText(const QString &text)
{
    if (d_ptr->titleText == text) {
        return;
    }
    d_ptr->titleText = text;
    syncAccessibility();
    emit titleTextChanged(d_ptr->titleText);
}

QString QtMaterialBottomSheet::titleText() const
{
    return d_ptr->titleText;
}

void QtMaterialBottomSheet::setSupportingText(const QString &text)
{
    if (d_ptr->supportingText == text) {
        return;
    }
    d_ptr->supportingText = text;
    syncAccessibility();
    emit supportingTextChanged(d_ptr->supportingText);
}

QString QtMaterialBottomSheet::supportingText() const
{
    return d_ptr->supportingText;
}

QString QtMaterialBottomSheet::accessibilitySummary() const
{
    QStringList parts;
    if (!d_ptr->titleText.trimmed().isEmpty()) {
        parts << d_ptr->titleText.trimmed();
    } else {
        parts << tr("Bottom sheet");
    }
    if (!d_ptr->supportingText.trimmed().isEmpty()) {
        parts << d_ptr->supportingText.trimmed();
    }
    parts << (d_ptr->expanded ? tr("Expanded") : tr("Collapsed"));
    if (d_ptr->modal) {
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
            if (!isOpen() || d_ptr->modal || !d_ptr->dismissOnScrim) {
                break;
            }
            ensureGeometryResolved();
            if (const auto *mouseEvent = static_cast<QMouseEvent *>(event)) {
                if (!d_ptr->cachedVisualRect.contains(mouseEvent->pos())) {
                    close();
                    return true;
                }
            }
            break;
        default:
            break;
        }
    }

    if (watched == d_ptr->scrim && event->type() == QEvent::MouseButtonPress) {
        if (isOpen() && d_ptr->dismissOnScrim) {
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
    if (!d_ptr->specPtr || d_ptr->cachedVisualRect.isEmpty()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(d_ptr->specPtr->containerColor);
    painter.drawPath(d_ptr->cachedContainerPath);
}

void QtMaterialBottomSheet::resizeEvent(QResizeEvent *event)
{
    QtMaterialOverlaySurface::resizeEvent(event);
    d_ptr->invalidateCachedGeometry();
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
        if (d_ptr->dismissOnEscape) {
            close();
            event->accept();
            return;
        }
        break;
    case Qt::Key_Down:
        if (event->modifiers() & Qt::ControlModifier) {
            if (d_ptr->expanded) {
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
    if (d_ptr->dragToDismissEnabled && event->button() == Qt::LeftButton && isOpen()) {
        ensureGeometryResolved();

        const QRect targetVisualRect(
            0,
            qMax(0, height() - d_ptr->effectiveSheetHeight()),
            width(),
            qMin(d_ptr->effectiveSheetHeight(), height()));
        const bool pressInsideCurrentSheet = d_ptr->cachedVisualRect.contains(event->pos());
        const bool pressInsideTargetSheet = targetVisualRect.contains(event->pos());

        if (pressInsideCurrentSheet || pressInsideTargetSheet) {
            d_ptr->dragging = true;
            d_ptr->dragStartGlobalPos = globalMousePosition(event);
            setProperty("_qtm3_drag_start_local_y", event->pos().y());
            event->accept();
            return;
        }
    }

    QtMaterialOverlaySurface::mousePressEvent(event);
}

void QtMaterialBottomSheet::mouseMoveEvent(QMouseEvent *event)
{
    if (d_ptr->dragging) {
        event->accept();
        return;
    }
    QtMaterialOverlaySurface::mouseMoveEvent(event);
}

void QtMaterialBottomSheet::mouseReleaseEvent(QMouseEvent *event)
{
    if (d_ptr->dragging) {
        const int globalDeltaY = globalMousePosition(event).y() - d_ptr->dragStartGlobalPos.y();
        const int dragStartLocalY = property("_qtm3_drag_start_local_y").toInt();
        const int localDeltaY = event->pos().y() - dragStartLocalY;
        const int deltaY = qAbs(globalDeltaY) >= qAbs(localDeltaY) ? globalDeltaY : localDeltaY;
        d_ptr->dragging = false;
        setProperty("_qtm3_drag_start_local_y", QVariant());

        const int dragDownThreshold = qBound(32, d_ptr->effectiveSheetHeight() / 5, 96);
        if (deltaY >= dragDownThreshold) {
            if (d_ptr->expanded && d_ptr->collapsedHeight < d_ptr->expandedHeight) {
                collapse();
            } else {
                close();
            }
        } else if (deltaY <= -32) {
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
    d_ptr->specDirty = true;
    d_ptr->invalidateCachedGeometry();
    ensureSpecResolved();
    if (d_ptr->specPtr) {
        d_ptr->transition->applyMotionToken(this->theme(), d_ptr->specPtr->motionToken);
    }
    syncContainerGeometry();
    applySheetMask();
    syncScrim();
    update();
}

void QtMaterialBottomSheet::ensureSpecResolved() const
{
    if (!d_ptr->specDirty && d_ptr->specPtr) {
        return;
    }

    static QtMaterial::SpecFactory factory;
    static QtMaterial::BottomSheetSpec spec;
    spec = factory.bottomSheetSpec(theme());
    d_ptr->specPtr = &spec;
    d_ptr->specDirty = false;
}

void QtMaterialBottomSheet::ensureGeometryResolved() const
{
    if (!d_ptr->geometryDirty) {
        return;
    }

    ensureSpecResolved();
    if (!d_ptr->specPtr) {
        return;
    }

    const QRect hostRect = rect();
    if (!hostRect.isValid()) {
        return;
    }

    const int minHeight = qMax(120, minimumSizeHint().height());
    const int sheetHeight = qMin(qMax(d_ptr->effectiveSheetHeight(), minHeight), hostRect.height());
    const qreal progressValue = d_ptr->transition ? d_ptr->transition->progress() : 1.0;
    const qreal p = qBound(0.0, progressValue, 1.0);
    const int hiddenY = hostRect.height();
    const int y = hiddenY - qRound(sheetHeight * p);

    d_ptr->cachedVisualRect = QRect(0, y, hostRect.width(), sheetHeight);
    d_ptr->cachedContentRect = d_ptr->cachedVisualRect.adjusted(0, d_ptr->specPtr->topPadding, 0, 0);

    const int specRadius = theme().shapes().radius(d_ptr->specPtr->shapeRole);
    const qreal resolvedRadius = specRadius > 0 ? qreal(specRadius) : 28.0;
    d_ptr->cachedCornerRadius = qMin(resolvedRadius, sheetHeight / 2.0);
    d_ptr->cachedContainerPath = topRoundedSheetPath(QRectF(d_ptr->cachedVisualRect), d_ptr->cachedCornerRadius);
    d_ptr->geometryDirty = false;
}

void QtMaterialBottomSheetPrivate::invalidateCachedGeometry() {
    geometryDirty = true;
    cachedVisualRect = QRect();
    cachedContentRect = QRect();
    cachedContainerPath = QPainterPath();
    cachedCornerRadius = 0.0;
}

void QtMaterialBottomSheet::syncToHost()
{
    if (QWidget *host = parentWidget()) {
        setGeometry(host->rect());
        if (d_ptr->scrim) {
            d_ptr->scrim->setGeometry(host->rect());
        }
    }
    d_ptr->invalidateCachedGeometry();
    syncContainerGeometry();
}

void QtMaterialBottomSheet::syncScrim()
{
    if (!d_ptr->scrim) {
        return;
    }

    if (!d_ptr->modal || d_ptr->state == SheetState::Closed) {
        d_ptr->scrim->hide();
        return;
    }

    ensureSpecResolved();
    if (!d_ptr->specPtr) {
        return;
    }

    QColor scrim = d_ptr->specPtr->scrimColor;
    const qreal progressValue = d_ptr->transition ? d_ptr->transition->progress() : 1.0;
    const qreal p = qBound(0.0, progressValue, 1.0);
    scrim.setAlphaF(qBound(0.0, scrim.alphaF() * p, 1.0));
    d_ptr->scrim->setGeometry(rect());
    d_ptr->scrim->setScrimColor(scrim);
    if (!d_ptr->scrim->isVisible()) {
        d_ptr->scrim->show();
    }
    d_ptr->scrim->raise();
}

void QtMaterialBottomSheet::syncContainerGeometry()
{
    if (!d_ptr->container) {
        return;
    }

    ensureGeometryResolved();
    if (d_ptr->cachedVisualRect.isEmpty()) {
        d_ptr->container->hide();
        return;
    }

    d_ptr->container->setGeometry(d_ptr->cachedVisualRect);
    applyContainerClip();
    if (isVisible() && !d_ptr->container->isVisible()) {
        d_ptr->container->show();
    }
    d_ptr->container->raise();
}

void QtMaterialBottomSheet::applyContainerClip()
{
    if (!d_ptr->container) {
        return;
    }

    ensureGeometryResolved();
    if (d_ptr->cachedVisualRect.isEmpty()) {
        d_ptr->container->clearMask();
        return;
    }

    const QPainterPath localPath = d_ptr->cachedContainerPath.translated(-d_ptr->cachedVisualRect.topLeft());
    const QRegion maskRegion(localPath.toFillPolygon().toPolygon());
    d_ptr->container->setMask(maskRegion);
}

void QtMaterialBottomSheet::applySheetMask()
{
    ensureGeometryResolved();
    if (d_ptr->cachedVisualRect.isEmpty()) {
        clearMask();
        return;
    }

    if (d_ptr->modal) {
        clearMask();
        return;
    }

    const QRegion panelRegion(d_ptr->cachedContainerPath.toFillPolygon().toPolygon());
    setMask(panelRegion);
}

void QtMaterialBottomSheet::focusFirstChild()
{
    if (d_ptr->initialFocusWidget && d_ptr->initialFocusWidget->isVisible() && d_ptr->initialFocusWidget->isEnabled()) {
        d_ptr->initialFocusWidget->setFocus(Qt::OtherFocusReason);
        return;
    }

    if (d_ptr->container) {
        const auto children = d_ptr->container->findChildren<QWidget *>();
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
    constexpr const char* autoAccessibleNameProperty = "_qtm3_auto_accessible_name";

    const QString summary = accessibilitySummary();
    const QString autoName = d_ptr->titleText.trimmed().isEmpty() ? tr("Bottom sheet") : d_ptr->titleText.trimmed();
    const QString currentName = accessibleName().trimmed();
    const QString previousAutoName = property(autoAccessibleNameProperty).toString().trimmed();
    const bool hasLegacyFallbackAutoName = previousAutoName.isEmpty() && currentName == tr("Bottom sheet");
    const bool currentNameIsAuto = currentName.isEmpty()
        || hasLegacyFallbackAutoName
        || (!previousAutoName.isEmpty() && currentName == previousAutoName);

    if (currentNameIsAuto) {
        setAccessibleName(autoName);
        setProperty(autoAccessibleNameProperty, autoName);
    }

    setAccessibleDescription(summary);
    if (d_ptr->lastAccessibilitySummary != summary) {
        d_ptr->lastAccessibilitySummary = summary;
        emit accessibilitySummaryChanged(summary);
    }
}

void QtMaterialBottomSheet::setState(SheetState state)
{
    if (d_ptr->state == state) {
        return;
    }
    d_ptr->state = state;
    emit stateChanged(d_ptr->state);
}

int QtMaterialBottomSheetPrivate::effectiveSheetHeight() const noexcept {
    return expanded ? expandedHeight : collapsedHeight;
}

QRect QtMaterialBottomSheet::sheetVisualRect() const
{
    ensureGeometryResolved();
    return d_ptr->cachedVisualRect;
}

QRect QtMaterialBottomSheet::contentRect() const
{
    ensureGeometryResolved();
    return d_ptr->cachedContentRect;
}

qreal QtMaterialBottomSheet::cornerRadius() const
{
    ensureGeometryResolved();
    return d_ptr->cachedCornerRadius;
}

} // namespace QtMaterial
