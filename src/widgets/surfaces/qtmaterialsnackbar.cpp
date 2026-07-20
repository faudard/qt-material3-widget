#include "qtmaterial/widgets/surfaces/qtmaterialsnackbar.h"

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QTimer>

#include <QRectF>
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialsnackbarspec.h"
#include "qtmaterial/specs/qtmaterialsurfacespecresolver.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include <memory>
#include "qtmaterial/effects/qtmaterialelevationrenderer.h"

namespace QtMaterial {

class QtMaterialSnackbarPrivate {
public:
 mutable bool specDirty = true;
 mutable QtMaterial::SnackbarSpec spec;
    mutable QtMaterial::SnackbarSpec* specPtr = nullptr;
 SnackbarRequest request;
 SnackbarDismissReason pendingDismissReason = SnackbarDismissReason::Manual;
 QtMaterialSnackbar::State state = QtMaterialSnackbar::State::Hidden;
 QLabel* label = nullptr;
 QPushButton* actionButton = nullptr;
 QPushButton* dismissButton = nullptr;
 QHBoxLayout* layout = nullptr;
 QTimer* timer = nullptr;
 bool pauseAutoHideOnInteraction = true;
 bool autoHidePaused = false;
 int remainingAutoHideMs = 0;
 QString lastAccessibilitySummary;
 QPointer<QtMaterialTransitionController> transition;
};

namespace {

int snackbarDurationMs(
    const SnackbarRequest& request,
    const SnackbarSpec& spec)
{
    switch (request.duration) {
    case SnackbarDuration::Short:
        return spec.shortDurationMs;
    case SnackbarDuration::Long:
        return spec.longDurationMs;
    case SnackbarDuration::Indefinite:
        return 0;
    }
    return 0;
}

QRectF snackbarContainerRect(const QtMaterialSnackbar& snackbar)
{
    return QRectF(snackbar.rect());
}

} // namespace
QtMaterialSnackbar::QtMaterialSnackbar(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
    , d_ptr(std::make_unique<QtMaterialSnackbarPrivate>())
{
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_StyledBackground, false);
    setAttribute(Qt::WA_TranslucentBackground, true);
    hide();

    d_ptr->label = new QLabel(this);
    d_ptr->label->setWordWrap(true);
    d_ptr->label->setTextInteractionFlags(Qt::NoTextInteraction);

    d_ptr->actionButton = new QPushButton(this);
    d_ptr->actionButton->setFlat(true);
    d_ptr->actionButton->hide();

    d_ptr->dismissButton = new QPushButton(QString(QChar(0x2715)), this);
    d_ptr->dismissButton->setFlat(true);
    d_ptr->dismissButton->hide();

    d_ptr->layout = new QHBoxLayout(this);
    d_ptr->layout->setContentsMargins(QMargins());
    d_ptr->layout->setSpacing(0);
    d_ptr->layout->addWidget(d_ptr->label, 1);
    d_ptr->layout->addWidget(d_ptr->actionButton, 0);
    d_ptr->layout->addWidget(d_ptr->dismissButton, 0);

    d_ptr->timer = new QTimer(this);
    d_ptr->timer->setSingleShot(true);

    d_ptr->transition = new QtMaterialTransitionController(this);
    ensureSpecResolved();
    if (d_ptr->specPtr
        && d_ptr->specPtr->hasResolvedEnterMotion) {
        d_ptr->transition->applyMotionStyle(
            d_ptr->specPtr->enterMotionStyle);
    }

    connect(
        d_ptr->timer,
        &QTimer::timeout,
        this,
        [this]() {
            d_ptr->remainingAutoHideMs = 0;
            dismiss(SnackbarDismissReason::Timeout);
        });

    connect(d_ptr->actionButton, &QPushButton::clicked, this, [this]() {
        emit actionTriggered();
        dismiss(SnackbarDismissReason::Action);
    });

    connect(d_ptr->dismissButton, &QPushButton::clicked, this, [this]() {
        dismiss(SnackbarDismissReason::Manual);
    });

    connect(d_ptr->transition, &QtMaterialTransitionController::progressChanged,
            this, [this](qreal value) {
                syncGeometryToHost();
                emit progressChanged(value);
                update();
            });

    connect(d_ptr->transition, &QtMaterialTransitionController::finished,
            this, [this]() {
                if (d_ptr->state == State::Entering) {
                    d_ptr->state = State::Visible;
                    updateAutoHide();
                    emit shown();
                } else if (d_ptr->state == State::Leaving) {
                    d_ptr->state = State::Hidden;
                    hide();
                    emit dismissed(d_ptr->pendingDismissReason);
                }
            });

    applyRequestToUi();
    applyResolvedTheme();
}

QtMaterialSnackbar::~QtMaterialSnackbar() = default;

void QtMaterialSnackbar::setRequest(const SnackbarRequest& request)
{
    d_ptr->request = request;
    ensureSpecResolved();
    d_ptr->remainingAutoHideMs = d_ptr->specPtr
        ? snackbarDurationMs(request, *d_ptr->specPtr)
        : 0;
    applyRequestToUi();
    updateAutoHide();
    updateGeometry();
    update();
}

SnackbarRequest QtMaterialSnackbar::request() const noexcept
{
    return d_ptr->request;
}

void QtMaterialSnackbar::setText(const QString& text)
{
    if (d_ptr->request.text == text) {
        return;
    }

    d_ptr->request.text = text;
    applyRequestToUi();
}

QString QtMaterialSnackbar::text() const
{
    return d_ptr->request.text;
}

void QtMaterialSnackbar::setActionText(const QString& text)
{
    if (d_ptr->request.actionText == text) {
        return;
    }

    d_ptr->request.actionText = text;
    applyRequestToUi();
}

QString QtMaterialSnackbar::actionText() const
{
    return d_ptr->request.actionText;
}

void QtMaterialSnackbar::setDuration(SnackbarDuration duration)
{
    if (d_ptr->request.duration == duration) {
        return;
    }

    d_ptr->request.duration = duration;
    ensureSpecResolved();
    d_ptr->remainingAutoHideMs = d_ptr->specPtr
        ? snackbarDurationMs(
              d_ptr->request,
              *d_ptr->specPtr)
        : 0;
    updateAutoHide();
}

SnackbarDuration QtMaterialSnackbar::duration() const noexcept
{
    return d_ptr->request.duration;
}

void QtMaterialSnackbar::setShowDismissButton(bool on)
{
    if (d_ptr->request.showDismissButton == on) {
        return;
    }

    d_ptr->request.showDismissButton = on;
    applyRequestToUi();
}

bool QtMaterialSnackbar::showDismissButton() const noexcept
{
    return d_ptr->request.showDismissButton;
}

void QtMaterialSnackbar::showSnackbar()
{
    if (!hostWidget() && parentWidget()) {
        setHostWidget(parentWidget());
    }

    ensureSpecResolved();
    if (d_ptr->specPtr) {
        d_ptr->transition->applyMotionStyle(
            d_ptr->specPtr->enterMotionStyle);
    }

    applyResolvedTheme();
    applyRequestToUi();
    syncGeometryToHost();

    show();
    raise();

    d_ptr->timer->stop();
    d_ptr->remainingAutoHideMs = d_ptr->specPtr
        ? snackbarDurationMs(
              d_ptr->request,
              *d_ptr->specPtr)
        : 0;
    if (d_ptr->autoHidePaused) {
        d_ptr->autoHidePaused = false;
        Q_EMIT autoHidePausedChanged(false);
    }
    d_ptr->pendingDismissReason = SnackbarDismissReason::Manual;
    d_ptr->state = State::Entering;
    d_ptr->transition->startForward();
}

void QtMaterialSnackbar::dismiss(SnackbarDismissReason reason)
{
    if (d_ptr->state == State::Hidden || d_ptr->state == State::Leaving) {
        return;
    }

    d_ptr->timer->stop();
    d_ptr->remainingAutoHideMs = 0;
    if (d_ptr->autoHidePaused) {
        d_ptr->autoHidePaused = false;
        Q_EMIT autoHidePausedChanged(false);
    }
    d_ptr->pendingDismissReason = reason;

    ensureSpecResolved();
    if (d_ptr->specPtr) {
        d_ptr->transition->applyMotionStyle(
            d_ptr->specPtr->exitMotionStyle);
    }

    d_ptr->state = State::Leaving;
    d_ptr->transition->startBackward();
}

qreal QtMaterialSnackbar::progress() const noexcept
{
    return d_ptr->transition ? d_ptr->transition->progress() : 0.0;
}

const SnackbarSpec& QtMaterialSnackbar::resolvedSpec() const
{
    ensureSpecResolved();
    return *d_ptr->specPtr;
}

bool QtMaterialSnackbar::isAutoHidePaused() const noexcept
{
    return d_ptr->autoHidePaused;
}

int QtMaterialSnackbar::remainingAutoHideTimeMs() const noexcept
{
    if (d_ptr->timer && d_ptr->timer->isActive()) {
        return qMax(0, d_ptr->timer->remainingTime());
    }
    return qMax(0, d_ptr->remainingAutoHideMs);
}

QSize QtMaterialSnackbar::sizeHint() const
{
    ensureSpecResolved();
    if (!d_ptr->specPtr) {
        return QSize();
    }

    const QSize hint =
        layout()
            ? layout()->sizeHint()
            : QSize(
                  d_ptr->specPtr->minWidth,
                  d_ptr->specPtr->minHeight);
    return QSize(
        qMax(d_ptr->specPtr->minWidth, hint.width()),
        qMax(d_ptr->specPtr->minHeight, hint.height()));
}

QSize QtMaterialSnackbar::minimumSizeHint() const
{
    ensureSpecResolved();
    return d_ptr->specPtr
        ? QSize(
              d_ptr->specPtr->minWidth,
              d_ptr->specPtr->minHeight)
        : QSize();
}

qreal QtMaterialSnackbar::resolvedCornerRadius()
    const noexcept
{
    if (!d_ptr || !d_ptr->specPtr) {
        return 0.0;
    }
    if (d_ptr->specPtr->cornerRadius < 0.0) {
        return qMin(width(), height()) / 2.0;
    }
    return d_ptr->specPtr->cornerRadius;
}

void QtMaterialSnackbar::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();
    if (!d_ptr->specPtr) {
        return;
    }

    const qreal p = progress();
    const qreal radius = resolvedCornerRadius();
    const QRectF bounds = snackbarContainerRect(*this);

    QPainterPath containerPath;
    containerPath.addRoundedRect(bounds, radius, radius);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (d_ptr->specPtr->hasResolvedElevationStyle) {
        QColor shadow = d_ptr->specPtr->shadowColor;
        shadow.setAlphaF(shadow.alphaF() * p);
        QtMaterialElevationRenderer::paintPathElevation(
            &painter,
            containerPath,
            shadow,
            d_ptr->specPtr->elevationStyle);
    }

    QColor fill = d_ptr->specPtr->containerColor;
    fill.setAlphaF(fill.alphaF() * p);
    painter.setPen(Qt::NoPen);
    painter.setBrush(fill);
    painter.drawPath(containerPath);
}

void QtMaterialSnackbar::resizeEvent(QResizeEvent* event)
{
    QtMaterialOverlaySurface::resizeEvent(event);
    QWidget::resizeEvent(event);
}

void QtMaterialSnackbar::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        dismiss(SnackbarDismissReason::Manual);
        event->accept();
        return;
    }

    QtMaterialOverlaySurface::keyPressEvent(event);
}

void QtMaterialSnackbar::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialOverlaySurface::themeChangedEvent(theme);
    Q_UNUSED(theme)

    invalidateResolvedSpec();
    applyResolvedTheme();
    updateAutoHide();
    syncGeometryToHost();
    update();
}

bool QtMaterialSnackbar::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::Enter:
    case QEvent::HoverEnter:
    case QEvent::FocusIn:
        setAutoHidePaused(true);
        break;
    case QEvent::Leave:
    case QEvent::HoverLeave:
    case QEvent::FocusOut:
        setAutoHidePaused(false);
        break;
    default:
        break;
    }

    return QtMaterialOverlaySurface::event(event);
}

bool QtMaterialSnackbar::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == d_ptr->actionButton || watched == d_ptr->dismissButton) {
        switch (event->type()) {
        case QEvent::Enter:
        case QEvent::HoverEnter:
        case QEvent::FocusIn:
            setAutoHidePaused(true);
            break;
        case QEvent::Leave:
        case QEvent::HoverLeave:
        case QEvent::FocusOut:
            setAutoHidePaused(false);
            break;
        default:
            break;
        }
    }

    return QtMaterialOverlaySurface::eventFilter(watched, event);
}

QString QtMaterialSnackbar::accessibilitySummary() const
{
    QStringList parts;

    const QString message = d_ptr->request.text.trimmed();
    if (!message.isEmpty()) {
        parts << message;
    }

    const QString action = d_ptr->request.actionText.trimmed();
    if (!action.isEmpty()) {
        parts << tr("Action: %1").arg(action);
    }

    if (d_ptr->request.showDismissButton) {
        parts << tr("Dismissible");
    }

    return parts.join(QStringLiteral(". "));
}

bool QtMaterialSnackbar::pauseAutoHideOnInteraction() const noexcept
{
    return d_ptr->pauseAutoHideOnInteraction;
}

void QtMaterialSnackbar::setPauseAutoHideOnInteraction(bool enabled)
{
    if (d_ptr->pauseAutoHideOnInteraction == enabled) {
        return;
    }

    d_ptr->pauseAutoHideOnInteraction = enabled;
    if (!enabled && d_ptr->autoHidePaused) {
        d_ptr->autoHidePaused = false;
        updateAutoHide();
    }
}

void QtMaterialSnackbar::syncAccessibilityState()
{
    const QString summary = accessibilitySummary();

    if (accessibleName().isEmpty()) {
        setAccessibleName(tr("Snackbar"));
    }
    setAccessibleDescription(summary);

    if (d_ptr->label) {
        d_ptr->label->setAccessibleName(tr("Snackbar message"));
        d_ptr->label->setAccessibleDescription(d_ptr->request.text.trimmed());
    }

    const QString action = d_ptr->request.actionText.trimmed();
    if (d_ptr->actionButton) {
        if (!action.isEmpty()) {
            d_ptr->actionButton->setAccessibleName(action);
            d_ptr->actionButton->setAccessibleDescription(tr("Activates snackbar action"));
        } else {
            d_ptr->actionButton->setAccessibleName(tr("Snackbar action"));
            d_ptr->actionButton->setAccessibleDescription(QString());
        }
    }

    if (d_ptr->dismissButton) {
        d_ptr->dismissButton->setAccessibleName(tr("Dismiss snackbar"));
        d_ptr->dismissButton->setAccessibleDescription(tr("Dismisses the snackbar"));
    }

    if (summary != d_ptr->lastAccessibilitySummary) {
        d_ptr->lastAccessibilitySummary = summary;
        emit accessibilitySummaryChanged(summary);
    }
}

void QtMaterialSnackbar::setAutoHidePaused(bool paused)
{
    ensureSpecResolved();
    if (!d_ptr->pauseAutoHideOnInteraction
        || !d_ptr->specPtr
        || snackbarDurationMs(
               d_ptr->request,
               *d_ptr->specPtr) <= 0
        || d_ptr->autoHidePaused == paused) {
        return;
    }

    if (paused) {
        const int remaining =
            d_ptr->timer->remainingTime();
        if (remaining > 0) {
            d_ptr->remainingAutoHideMs = remaining;
        }
        d_ptr->timer->stop();
        d_ptr->autoHidePaused = true;
        Q_EMIT autoHidePausedChanged(true);
        return;
    }

    d_ptr->autoHidePaused = false;
    Q_EMIT autoHidePausedChanged(false);
    updateAutoHide();
}

void QtMaterialSnackbar::syncGeometryToHost()
{
    QWidget* host = hostWidget();
    ensureSpecResolved();
    if (!host || !d_ptr->specPtr) {
        return;
    }

    const SnackbarSpec& spec = *d_ptr->specPtr;
    const QMargins margins = spec.outerMargins;
    const int availableWidth = qMax(
        spec.minWidth,
        host->width()
            - margins.left()
            - margins.right());
    const int maxWidth =
        qMin(spec.maxWidth, availableWidth);

    const int estimatedControlsWidth =
        (d_ptr->actionButton->isVisible()
            ? qMax(
                spec.actionMinWidth,
                d_ptr->actionButton->sizeHint().width())
            : 0)
        + (d_ptr->dismissButton->isVisible()
            ? spec.dismissButtonSize
            : 0)
        + spec.controlsReserveSpacing;

    d_ptr->label->setMaximumWidth(
        qMax(
            spec.labelMinWidth,
            maxWidth - estimatedControlsWidth));

    const QSize hint = sizeHint();
    const int width =
        qMin(maxWidth, qMax(hint.width(), spec.minWidth));
    const int height =
        qMax(spec.minHeight, hint.height());

    const qreal transitionProgress = progress();
    const int finalX = (host->width() - width) / 2;
    const int finalY =
        host->height()
        - margins.bottom()
        - height;
    const int hiddenY =
        finalY + spec.slideDistance;
    const int y =
        hiddenY
        - qRound(
            (hiddenY - finalY)
            * transitionProgress);

    setGeometry(finalX, y, width, height);
}

void QtMaterialSnackbar::ensureSpecResolved() const
{
    if (!d_ptr->specDirty && d_ptr->specPtr) {
        return;
    }

    const QtMaterial::SurfaceSpecResolver resolver;
    d_ptr->spec = resolver.snackbarSpec(theme());
    d_ptr->specPtr = &d_ptr->spec;
    d_ptr->specDirty = false;
}

void QtMaterialSnackbar::invalidateResolvedSpec()
{
    d_ptr->specDirty = true;
    d_ptr->specPtr = nullptr;
}

void QtMaterialSnackbar::applyResolvedTheme()
{
    ensureSpecResolved();
    if (!d_ptr->specPtr
        || !d_ptr->layout
        || !d_ptr->label
        || !d_ptr->actionButton
        || !d_ptr->dismissButton) {
        return;
    }

    const SnackbarSpec& spec = *d_ptr->specPtr;

    d_ptr->layout->setContentsMargins(spec.contentPadding);
    d_ptr->layout->setSpacing(spec.actionSpacing);

    if (spec.hasResolvedTextFont) {
        d_ptr->label->setFont(spec.textFont);
    }
    if (spec.hasResolvedActionFont) {
        d_ptr->actionButton->setFont(spec.actionFont);
        d_ptr->dismissButton->setFont(spec.actionFont);
    }

    d_ptr->label->setStyleSheet(
        QStringLiteral(
            "background: transparent; color: %1;")
            .arg(spec.textColor.name(QColor::HexArgb)));

    d_ptr->actionButton->setMinimumWidth(
        spec.actionMinWidth);
    d_ptr->actionButton->setStyleSheet(
        QStringLiteral(
            "QPushButton {"
            " background: transparent;"
            " border: none;"
            " border-radius: %1px;"
            " color: %2;"
            " padding: %3px %4px;"
            "}")
            .arg(spec.actionCornerRadius)
            .arg(spec.actionColor.name(QColor::HexArgb))
            .arg(spec.actionPaddingVertical)
            .arg(spec.actionPaddingHorizontal));

    d_ptr->dismissButton->setFixedSize(
        spec.dismissButtonSize,
        spec.dismissButtonSize);
    d_ptr->dismissButton->setStyleSheet(
        QStringLiteral(
            "QPushButton {"
            " background: transparent;"
            " border: none;"
            " border-radius: %1px;"
            " color: %2;"
            "}")
            .arg(spec.dismissCornerRadius)
            .arg(
                spec.dismissIconColor.name(
                    QColor::HexArgb)));

    updateGeometry();
}

void QtMaterialSnackbar::applyRequestToUi()
{
    d_ptr->label->setText(d_ptr->request.text);

    const bool hasAction = !d_ptr->request.actionText.trimmed().isEmpty();
    d_ptr->actionButton->setVisible(hasAction);
    d_ptr->actionButton->setText(d_ptr->request.actionText);

    d_ptr->dismissButton->setVisible(d_ptr->request.showDismissButton);

    updateGeometry();
    syncGeometryToHost();
    update();
}

void QtMaterialSnackbar::updateAutoHide()
{
    d_ptr->timer->stop();
    ensureSpecResolved();

    if (!d_ptr->specPtr) {
        d_ptr->remainingAutoHideMs = 0;
        return;
    }

    const int totalDuration =
        snackbarDurationMs(
            d_ptr->request,
            *d_ptr->specPtr);
    const bool canRun =
        totalDuration > 0
        && (d_ptr->state == State::Entering
            || d_ptr->state == State::Visible);

    if (!canRun) {
        d_ptr->remainingAutoHideMs = 0;
        return;
    }

    if (d_ptr->remainingAutoHideMs <= 0
        || d_ptr->remainingAutoHideMs > totalDuration) {
        d_ptr->remainingAutoHideMs = totalDuration;
    }

    if (!d_ptr->autoHidePaused) {
        d_ptr->timer->start(
            d_ptr->remainingAutoHideMs);
    }
}

} // namespace QtMaterial