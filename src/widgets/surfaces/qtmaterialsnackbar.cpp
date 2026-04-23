#include "qtmaterial/widgets/surfaces/qtmaterialsnackbar.h"

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QTimer>

#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialsnackbarspec.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

QtMaterialSnackbar::QtMaterialSnackbar(QWidget* parent)
    : QtMaterialOverlaySurface(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_StyledBackground, false);
    setAttribute(Qt::WA_TranslucentBackground, true);
    hide();

    if (parentWidget()) {
        setHostWidget(parentWidget());
    }

    m_label = new QLabel(this);
    m_label->setWordWrap(true);
    m_label->setTextInteractionFlags(Qt::NoTextInteraction);

    m_actionButton = new QPushButton(this);
    m_actionButton->setFlat(true);
    m_actionButton->hide();

    m_dismissButton = new QPushButton(QStringLiteral("✕"), this);
    m_dismissButton->setFlat(true);
    m_dismissButton->hide();

    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(16, 14, 8, 14);
    m_layout->setSpacing(8);
    m_layout->addWidget(m_label, 1);
    m_layout->addWidget(m_actionButton, 0);
    m_layout->addWidget(m_dismissButton, 0);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);

    m_transition = new QtMaterialTransitionController(this);
    m_transition->applyMotionToken(theme(), MotionToken::Medium2);

    connect(m_timer, &QTimer::timeout, this, [this]() {
        dismiss(SnackbarDismissReason::Timeout);
    });

    connect(m_actionButton, &QPushButton::clicked, this, [this]() {
        emit actionTriggered();
        dismiss(SnackbarDismissReason::Action);
    });

    connect(m_dismissButton, &QPushButton::clicked, this, [this]() {
        dismiss(SnackbarDismissReason::Manual);
    });

    connect(m_transition, &QtMaterialTransitionController::progressChanged,
            this, [this](qreal value) {
                syncGeometryToHost();
                emit progressChanged(value);
                update();
            });

    connect(m_transition, &QtMaterialTransitionController::finished,
            this, [this]() {
                if (m_state == State::Entering) {
                    m_state = State::Visible;
                    updateAutoHide();
                    emit shown();
                } else if (m_state == State::Leaving) {
                    m_state = State::Hidden;
                    hide();
                    emit dismissed(m_pendingDismissReason);
                }
            });

    applyRequestToUi();
    applyResolvedTheme();
}

QtMaterialSnackbar::~QtMaterialSnackbar() = default;

void QtMaterialSnackbar::setRequest(const SnackbarRequest& request)
{
    m_request = request;
    applyRequestToUi();
    updateAutoHide();
    updateGeometry();
    update();
}

SnackbarRequest QtMaterialSnackbar::request() const noexcept
{
    return m_request;
}

void QtMaterialSnackbar::setText(const QString& text)
{
    if (m_request.text == text) {
        return;
    }

    m_request.text = text;
    applyRequestToUi();
}

QString QtMaterialSnackbar::text() const
{
    return m_request.text;
}

void QtMaterialSnackbar::setActionText(const QString& text)
{
    if (m_request.actionText == text) {
        return;
    }

    m_request.actionText = text;
    applyRequestToUi();
}

QString QtMaterialSnackbar::actionText() const
{
    return m_request.actionText;
}

void QtMaterialSnackbar::setDuration(SnackbarDuration duration)
{
    if (m_request.duration == duration) {
        return;
    }

    m_request.duration = duration;
    updateAutoHide();
}

SnackbarDuration QtMaterialSnackbar::duration() const noexcept
{
    return m_request.duration;
}

void QtMaterialSnackbar::setShowDismissButton(bool on)
{
    if (m_request.showDismissButton == on) {
        return;
    }

    m_request.showDismissButton = on;
    applyRequestToUi();
}

bool QtMaterialSnackbar::showDismissButton() const noexcept
{
    return m_request.showDismissButton;
}

void QtMaterialSnackbar::showSnackbar()
{
    if (!hostWidget() && parentWidget()) {
        setHostWidget(parentWidget());
    }

    ensureSpecResolved();
    if (m_specPtr) {
        m_transition->applyMotionToken(theme(), m_specPtr->enterMotion);
    }

    applyResolvedTheme();
    applyRequestToUi();
    syncGeometryToHost();

    show();
    raise();

    m_timer->stop();
    m_pendingDismissReason = SnackbarDismissReason::Manual;
    m_state = State::Entering;
    m_transition->startForward();
}

void QtMaterialSnackbar::dismiss(SnackbarDismissReason reason)
{
    if (m_state == State::Hidden || m_state == State::Leaving) {
        return;
    }

    m_timer->stop();
    m_pendingDismissReason = reason;

    ensureSpecResolved();
    if (m_specPtr) {
        m_transition->applyMotionToken(theme(), m_specPtr->exitMotion);
    }

    m_state = State::Leaving;
    m_transition->startBackward();
}

qreal QtMaterialSnackbar::progress() const noexcept
{
    return m_transition ? m_transition->progress() : 0.0;
}

QSize QtMaterialSnackbar::sizeHint() const
{
    ensureSpecResolved();

    const int minHeight = m_specPtr ? m_specPtr->minHeight : 48;
    const int minWidth = 200;
    const QSize hint = layout() ? layout()->sizeHint() : QSize(minWidth, minHeight);

    return QSize(qMax(minWidth, hint.width()), qMax(minHeight, hint.height()));
}

QSize QtMaterialSnackbar::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(200, m_specPtr ? m_specPtr->minHeight : 48);
}

void QtMaterialSnackbar::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();
    if (!m_specPtr) {
        return;
    }

    const qreal p = progress();
    const qreal radius = resolvedCornerRadius();

    QColor fill = m_specPtr->containerColor;
    fill.setAlphaF(fill.alphaF() * p);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(Qt::NoPen);
    painter.setBrush(fill);
    painter.drawRoundedRect(containerRect(), radius, radius);
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

void QtMaterialSnackbar::syncGeometryToHost()
{
    QWidget* host = hostWidget();
    ensureSpecResolved();

    if (!host || !m_specPtr) {
        return;
    }

    m_layout->setContentsMargins(m_specPtr->contentPadding);
    m_layout->setSpacing(m_specPtr->actionSpacing);

    const QMargins margins = m_specPtr->outerMargins;
    const int maxWidth = qMin(m_specPtr->maxWidth,
                              qMax(200, host->width() - margins.left() - margins.right()));

    const int estimatedButtonWidth =
        (m_actionButton->isVisible()
             ? qMax(m_specPtr->actionMinWidth, m_actionButton->sizeHint().width())
             : 0)
        + (m_dismissButton->isVisible() ? m_specPtr->dismissButtonSize : 0)
        + 24;

    m_label->setMaximumWidth(qMax(120, maxWidth - estimatedButtonWidth));

    const QSize hint = sizeHint();
    const int w = qMin(maxWidth, qMax(hint.width(), 200));
    const int h = qMax(m_specPtr->minHeight, hint.height());

    const qreal p = progress();
    const int finalX = (host->width() - w) / 2;
    const int finalY = host->height() - margins.bottom() - h;
    const int hiddenY = finalY + 16;
    const int y = hiddenY - qRound((hiddenY - finalY) * p);

    setGeometry(finalX, y, w, h);
}

void QtMaterialSnackbar::ensureSpecResolved() const
{
    if (!m_specDirty && m_specPtr) {
        return;
    }

    static QtMaterial::SpecFactory factory;
    static QtMaterial::SnackbarSpec spec;
    spec = factory.snackbarSpec(theme());

    m_specPtr = &spec;
    m_specDirty = false;
}

void QtMaterialSnackbar::invalidateResolvedSpec()
{
    m_specDirty = true;
    m_specPtr = nullptr;
}

void QtMaterialSnackbar::applyResolvedTheme()
{
    ensureSpecResolved();
    if (!m_specPtr) {
        return;
    }

    m_layout->setContentsMargins(m_specPtr->contentPadding);
    m_layout->setSpacing(m_specPtr->actionSpacing);

    QColor textColor = m_specPtr->textColor;
    textColor.setAlphaF(textColor.alphaF() * qMax(progress(), 1.0));

    m_label->setStyleSheet(QStringLiteral(
        "background: transparent; color: %1;")
        .arg(m_specPtr->textColor.name(QColor::HexArgb)));

    m_actionButton->setMinimumWidth(m_specPtr->actionMinWidth);
    m_actionButton->setStyleSheet(QStringLiteral(
        "QPushButton { background: transparent; border: none; color: %1; padding: 8px 12px; }")
        .arg(m_specPtr->actionColor.name(QColor::HexArgb)));

    m_dismissButton->setFixedSize(m_specPtr->dismissButtonSize, m_specPtr->dismissButtonSize);
    m_dismissButton->setStyleSheet(QStringLiteral(
        "QPushButton { background: transparent; border: none; color: %1; }")
        .arg(m_specPtr->dismissIconColor.name(QColor::HexArgb)));
}

void QtMaterialSnackbar::applyRequestToUi()
{
    m_label->setText(m_request.text);

    const bool hasAction = !m_request.actionText.trimmed().isEmpty();
    m_actionButton->setVisible(hasAction);
    m_actionButton->setText(m_request.actionText);

    m_dismissButton->setVisible(m_request.showDismissButton);

    updateGeometry();
    syncGeometryToHost();
    update();
}

void QtMaterialSnackbar::updateAutoHide()
{
    m_timer->stop();

    const int durationMs = currentDurationMs();
    if (durationMs > 0 && (m_state == State::Entering || m_state == State::Visible)) {
        m_timer->start(durationMs);
    }
}

int QtMaterialSnackbar::currentDurationMs() const
{
    switch (m_request.duration) {
    case SnackbarDuration::Short:
        return 4000;
    case SnackbarDuration::Long:
        return 10000;
    case SnackbarDuration::Indefinite:
        return 0;
    }

    return 0;
}

QRectF QtMaterialSnackbar::containerRect() const
{
    return QRectF(rect());
}

qreal QtMaterialSnackbar::resolvedCornerRadius() const noexcept
{
    if (!m_specPtr) {
        return 8.0;
    }

    const int specRadius = theme().shapes().radius(m_specPtr->shapeRole);
    return specRadius > 0 ? qreal(specRadius) : 8.0;
}

} // namespace QtMaterial