#include "qtmaterial/widgets/surfaces/qtmaterialtooltip.h"

#include <QEvent>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QPainter>
#include <QPointer>
#include <QScreen>
#include <QTimer>

#include "../resolution/qtmaterialmissingmaterial3specresolution_p.h"

namespace QtMaterial {

namespace {

constexpr int kHorizontalPadding = 12;
constexpr int kVerticalPadding = 6;
constexpr int kMaximumTextWidth = 280;
constexpr int kGap = 8;
constexpr int kCornerRadius = 4;

} // namespace

class QtMaterialTooltipPrivate
{
public:
    QString text;
    QtMaterialTooltip::Placement placement = QtMaterialTooltip::Placement::Auto;
    int showDelay = 500;
    QPointer<QWidget> target;
    QTimer* timer = nullptr;
};

QtMaterialTooltip::QtMaterialTooltip(QWidget* parent)
    : QtMaterialWidget(parent)
    , d_ptr(std::make_unique<QtMaterialTooltipPrivate>())
{
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_ShowWithoutActivating, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setFocusPolicy(Qt::NoFocus);
    setAccessibleName(tr("Tooltip"));

    d_ptr->timer = new QTimer(this);
    d_ptr->timer->setSingleShot(true);
    connect(d_ptr->timer, &QTimer::timeout, this, &QtMaterialTooltip::showTooltip);

    hide();
}

QtMaterialTooltip::~QtMaterialTooltip()
{
    setTargetWidget(nullptr);
}

QString QtMaterialTooltip::text() const
{
    return d_ptr->text;
}

void QtMaterialTooltip::setText(const QString& text)
{
    if (d_ptr->text == text) {
        return;
    }

    d_ptr->text = text;
    setAccessibleDescription(text);
    updateGeometry();
    if (isVisible()) {
        resize(sizeHint());
        reposition();
    }
    update();
    Q_EMIT textChanged(text);
}

QtMaterialTooltip::Placement QtMaterialTooltip::placement() const noexcept
{
    return d_ptr->placement;
}

void QtMaterialTooltip::setPlacement(Placement placement)
{
    if (d_ptr->placement == placement) {
        return;
    }

    d_ptr->placement = placement;
    if (isVisible()) {
        reposition();
    }
    Q_EMIT placementChanged(placement);
}

int QtMaterialTooltip::showDelay() const noexcept
{
    return d_ptr->showDelay;
}

void QtMaterialTooltip::setShowDelay(int milliseconds)
{
    milliseconds = qMax(0, milliseconds);
    if (d_ptr->showDelay == milliseconds) {
        return;
    }

    d_ptr->showDelay = milliseconds;
    Q_EMIT showDelayChanged(milliseconds);
}

QWidget* QtMaterialTooltip::targetWidget() const noexcept
{
    return d_ptr->target;
}

void QtMaterialTooltip::setTargetWidget(QWidget* target)
{
    if (d_ptr->target == target) {
        return;
    }

    if (d_ptr->target) {
        d_ptr->target->removeEventFilter(this);
    }

    d_ptr->target = target;

    if (d_ptr->target) {
        d_ptr->target->installEventFilter(this);
        if (accessibleDescription().isEmpty() && !d_ptr->target->toolTip().isEmpty()) {
            setText(d_ptr->target->toolTip());
        }
    } else {
        hideTooltip();
    }
}

bool QtMaterialTooltip::isTooltipVisible() const noexcept
{
    return isVisible();
}

QSize QtMaterialTooltip::sizeHint() const
{
    const QFontMetrics fm(font());
    const QRect bounds = fm.boundingRect(
        QRect(0, 0, kMaximumTextWidth, 1000),
        Qt::TextWordWrap | Qt::AlignLeft,
        d_ptr->text.isEmpty() ? QStringLiteral(" ") : d_ptr->text);

    return QSize(
        qMax(24, bounds.width() + 2 * kHorizontalPadding),
        qMax(24, bounds.height() + 2 * kVerticalPadding));
}

QSize QtMaterialTooltip::minimumSizeHint() const
{
    return QSize(24, 24);
}

void QtMaterialTooltip::showTooltip()
{
    d_ptr->timer->stop();
    if (!d_ptr->target || d_ptr->text.trimmed().isEmpty()) {
        return;
    }

    resize(sizeHint());
    reposition();
    const bool wasVisible = isVisible();
    show();
    raise();
    if (!wasVisible) {
        Q_EMIT tooltipVisibleChanged(true);
    }
}

void QtMaterialTooltip::hideTooltip()
{
    d_ptr->timer->stop();
    const bool wasVisible = isVisible();
    hide();
    if (wasVisible) {
        Q_EMIT tooltipVisibleChanged(false);
    }
}

bool QtMaterialTooltip::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == d_ptr->target) {
        switch (event->type()) {
        case QEvent::Enter:
        case QEvent::HoverEnter:
        case QEvent::FocusIn:
            scheduleShow();
            break;
        case QEvent::Leave:
        case QEvent::HoverLeave:
        case QEvent::FocusOut:
        case QEvent::Hide:
            hideTooltip();
            break;
        case QEvent::Move:
        case QEvent::Resize:
            if (isVisible()) {
                reposition();
            }
            break;
        default:
            break;
        }
    }

    return QtMaterialWidget::eventFilter(watched, event);
}

void QtMaterialTooltip::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const auto spec =
        MissingMaterial3SpecResolution::tooltipSpec(
            theme(),
            palette());

    painter.setPen(Qt::NoPen);
    painter.setBrush(spec.containerColor);
    painter.drawRoundedRect(rect(), kCornerRadius, kCornerRadius);

    painter.setPen(spec.contentColor);
    painter.drawText(
        rect().adjusted(
            kHorizontalPadding,
            kVerticalPadding,
            -kHorizontalPadding,
            -kVerticalPadding),
        Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignVCenter,
        d_ptr->text);
}

void QtMaterialTooltip::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialWidget::themeChangedEvent(theme);
    update();
}

void QtMaterialTooltip::scheduleShow()
{
    if (!d_ptr->target || d_ptr->text.trimmed().isEmpty()) {
        return;
    }

    if (d_ptr->showDelay == 0) {
        showTooltip();
    } else {
        d_ptr->timer->start(d_ptr->showDelay);
    }
}

void QtMaterialTooltip::reposition()
{
    if (!d_ptr->target) {
        return;
    }

    const QPoint targetTopLeft = d_ptr->target->mapToGlobal(QPoint(0, 0));
    const QRect targetRect(targetTopLeft, d_ptr->target->size());
    const QSize tooltipSize = size();

    Placement effective = d_ptr->placement;
    QScreen* screen = QGuiApplication::screenAt(targetRect.center());
    const QRect available = screen
        ? screen->availableGeometry()
        : QRect(targetRect.adjusted(-1000, -1000, 1000, 1000));

    if (effective == Placement::Auto) {
        effective = targetRect.bottom() + kGap + tooltipSize.height() <= available.bottom()
            ? Placement::Below
            : Placement::Above;
    }

    QPoint pos;
    switch (effective) {
    case Placement::Above:
        pos = QPoint(
            targetRect.center().x() - tooltipSize.width() / 2,
            targetRect.top() - kGap - tooltipSize.height());
        break;
    case Placement::Below:
        pos = QPoint(
            targetRect.center().x() - tooltipSize.width() / 2,
            targetRect.bottom() + kGap);
        break;
    case Placement::Left:
        pos = QPoint(
            targetRect.left() - kGap - tooltipSize.width(),
            targetRect.center().y() - tooltipSize.height() / 2);
        break;
    case Placement::Right:
        pos = QPoint(
            targetRect.right() + kGap,
            targetRect.center().y() - tooltipSize.height() / 2);
        break;
    case Placement::Auto:
        break;
    }

    pos.setX(qBound(available.left(), pos.x(), qMax(available.left(), available.right() - tooltipSize.width() + 1)));
    pos.setY(qBound(available.top(), pos.y(), qMax(available.top(), available.bottom() - tooltipSize.height() + 1)));
    move(pos);
}

} // namespace QtMaterial
