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
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include <memory>

namespace QtMaterial {


class QtMaterialSnackbarPrivate {
public:
 mutable bool specDirty = true;
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
 QString lastAccessibilitySummary;
 QPointer<QtMaterialTransitionController> transition;
};


namespace {

int snackbarDurationMs(const SnackbarRequest& request)
{
    switch (request.duration) {
    case SnackbarDuration::Short:
        return 4000;
    case SnackbarDuration::Long:
        return 10000;
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
    d_ptr->layout->setContentsMargins(16, 14, 8, 14);
    d_ptr->layout->setSpacing(8);
    d_ptr->layout->addWidget(d_ptr->label, 1);
    d_ptr->layout->addWidget(d_ptr->actionButton, 0);
    d_ptr->layout->addWidget(d_ptr->dismissButton, 0);

    d_ptr->timer = new QTimer(this);
    d_ptr->timer->setSingleShot(true);

    d_ptr->transition = new QtMaterialTransitionController(this);
    d_ptr->transition->applyMotionToken(theme(), MotionToken::Medium2);

    connect(d_ptr->timer, &QTimer::timeout, this, [this]() {
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
        d_ptr->transition->applyMotionToken(theme(), d_ptr->specPtr->enterMotion);
    }

    applyResolvedTheme();
    applyRequestToUi();
    syncGeometryToHost();

    show();
    raise();

    d_ptr->timer->stop();
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
    d_ptr->pendingDismissReason = reason;

    ensureSpecResolved();
    if (d_ptr->specPtr) {
        d_ptr->transition->applyMotionToken(theme(), d_ptr->specPtr->exitMotion);
    }

    d_ptr->state = State::Leaving;
    d_ptr->transition->startBackward();
}

qreal QtMaterialSnackbar::progress() const noexcept
{
    return d_ptr->transition ? d_ptr->transition->progress() : 0.0;
}

QSize QtMaterialSnackbar::sizeHint() const
{
    ensureSpecResolved();

    const int minHeight = d_ptr->specPtr ? d_ptr->specPtr->minHeight : 48;
    const int minWidth = 200;
    const QSize hint = layout() ? layout()->sizeHint() : QSize(minWidth, minHeight);

    return QSize(qMax(minWidth, hint.width()), qMax(minHeight, hint.height()));
}

QSize QtMaterialSnackbar::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(200, d_ptr->specPtr ? d_ptr->specPtr->minHeight : 48);
}

void QtMaterialSnackbar::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();
    if (!d_ptr->specPtr) {
        return;
    }

    const qreal p = progress();
    const qreal radius = resolvedCornerRadius();

    QColor fill = d_ptr->specPtr->containerColor;
    fill.setAlphaF(fill.alphaF() * p);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(Qt::NoPen);
    painter.setBrush(fill);
    painter.drawRoundedRect(snackbarContainerRect(*this), radius, radius);
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
    if (!d_ptr->pauseAutoHideOnInteraction || snackbarDurationMs(d_ptr->request) <= 0) {
        return;
    }

    if (d_ptr->autoHidePaused == paused) {
        return;
    }

    d_ptr->autoHidePaused = paused;
    if (d_ptr->autoHidePaused) {
        d_ptr->timer->stop();
        return;
    }

    updateAutoHide();
}


void QtMaterialSnackbar::syncGeometryToHost()
{
    QWidget* host = hostWidget();
    ensureSpecResolved();

    if (!host || !d_ptr->specPtr || !d_ptr->layout || !d_ptr->label || !d_ptr->actionButton || !d_ptr->dismissButton) {
        return;
    }

    d_ptr->layout->setContentsMargins(d_ptr->specPtr->contentPadding);
    d_ptr->layout->setSpacing(d_ptr->specPtr->actionSpacing);

    const QMargins margins = d_ptr->specPtr->outerMargins;
    const int maxWidth = qMin(d_ptr->specPtr->maxWidth,
                              qMax(200, host->width() - margins.left() - margins.right()));

    const int estimatedButtonWidth =
        (d_ptr->actionButton->isVisible()
             ? qMax(d_ptr->specPtr->actionMinWidth, d_ptr->actionButton->sizeHint().width())
             : 0)
        + (d_ptr->dismissButton->isVisible() ? d_ptr->specPtr->dismissButtonSize : 0)
        + 24;

    d_ptr->label->setMaximumWidth(qMax(120, maxWidth - estimatedButtonWidth));

    const QSize hint = sizeHint();
    const int w = qMin(maxWidth, qMax(hint.width(), 200));
    const int h = qMax(d_ptr->specPtr->minHeight, hint.height());

    const qreal p = progress();
    const int finalX = (host->width() - w) / 2;
    const int finalY = host->height() - margins.bottom() - h;
    const int hiddenY = finalY + 16;
    const int y = hiddenY - qRound((hiddenY - finalY) * p);

    setGeometry(finalX, y, w, h);
}

void QtMaterialSnackbar::ensureSpecResolved() const
{
    if (!d_ptr->specDirty && d_ptr->specPtr) {
        return;
    }

    static QtMaterial::SpecFactory factory;
    static QtMaterial::SnackbarSpec spec;
    spec = factory.snackbarSpec(theme());

    d_ptr->specPtr = &spec;
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
    if (!d_ptr->specPtr || !d_ptr->layout || !d_ptr->label || !d_ptr->actionButton || !d_ptr->dismissButton) {
        return;
    }

    d_ptr->layout->setContentsMargins(d_ptr->specPtr->contentPadding);
    d_ptr->layout->setSpacing(d_ptr->specPtr->actionSpacing);

    d_ptr->label->setStyleSheet(QStringLiteral(
                               "background: transparent; color: %1;")
                               .arg(d_ptr->specPtr->textColor.name(QColor::HexArgb)));

    d_ptr->actionButton->setMinimumWidth(d_ptr->specPtr->actionMinWidth);
    d_ptr->actionButton->setStyleSheet(QStringLiteral(
                                      "QPushButton { background: transparent; border: none; color: %1; padding: 8px 12px; }")
                                      .arg(d_ptr->specPtr->actionColor.name(QColor::HexArgb)));

    d_ptr->dismissButton->setFixedSize(d_ptr->specPtr->dismissButtonSize, d_ptr->specPtr->dismissButtonSize);
    d_ptr->dismissButton->setStyleSheet(QStringLiteral(
                                       "QPushButton { background: transparent; border: none; color: %1; }")
                                       .arg(d_ptr->specPtr->dismissIconColor.name(QColor::HexArgb)));
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

    const int durationMs = snackbarDurationMs(d_ptr->request);
    if (durationMs > 0 && (d_ptr->state == State::Entering || d_ptr->state == State::Visible)) {
        d_ptr->timer->start(durationMs);
    }
}


} // namespace QtMaterial