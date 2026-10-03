#include "qtmaterial/widgets/inputs/qtmaterialrangeslider.h"

#include <QFocusEvent>
#include <QKeyEvent>
#include <QLineF>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

namespace QtMaterial {

class QtMaterialRangeSliderPrivate final
{
public:
    int minimum = 0;
    int maximum = 100;
    int lowerValue = 25;
    int upperValue = 75;
    Qt::Orientation orientation = Qt::Horizontal;
    QtMaterialRangeSlider::Handle activeHandle = QtMaterialRangeSlider::Handle::Lower;
    bool dragging = false;

    void updateAccessibility(QtMaterialRangeSlider* q) const
    {
        const QString active =
            activeHandle == QtMaterialRangeSlider::Handle::Lower
                ? QtMaterialRangeSlider::tr("Lower")
                : QtMaterialRangeSlider::tr("Upper");
        q->setAccessibleDescription(
            QtMaterialRangeSlider::tr(
                "Lower %1, upper %2. %3 handle active")
                .arg(lowerValue)
                .arg(upperValue)
                .arg(active));
    }
};

namespace {
constexpr int kMargin = 14;
constexpr qreal kTrackThickness = 4.0;
constexpr qreal kHandleRadius = 10.0;
}

QtMaterialRangeSlider::QtMaterialRangeSlider(QWidget* parent)
    : QWidget(parent)
    , d_ptr(std::make_unique<QtMaterialRangeSliderPrivate>())
{
    setObjectName(QStringLiteral("qtmaterial_range_slider"));
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(80, 32);
    setAccessibleName(tr("Range slider"));
    d_ptr->updateAccessibility(this);
}

QtMaterialRangeSlider::~QtMaterialRangeSlider() = default;

int QtMaterialRangeSlider::minimum() const noexcept { return d_ptr->minimum; }
int QtMaterialRangeSlider::maximum() const noexcept { return d_ptr->maximum; }

void QtMaterialRangeSlider::setMinimum(int value) { setRange(value, d_ptr->maximum); }
void QtMaterialRangeSlider::setMaximum(int value) { setRange(d_ptr->minimum, value); }

void QtMaterialRangeSlider::setRange(int minimum, int maximum)
{
    if (minimum > maximum) {
        qSwap(minimum, maximum);
    }
    if (d_ptr->minimum == minimum && d_ptr->maximum == maximum) {
        return;
    }
    d_ptr->minimum = minimum;
    d_ptr->maximum = maximum;
    const int lower = qBound(d_ptr->minimum, d_ptr->lowerValue, d_ptr->maximum);
    const int upper = qBound(lower, d_ptr->upperValue, d_ptr->maximum);
    d_ptr->lowerValue = lower;
    d_ptr->upperValue = upper;
    d_ptr->updateAccessibility(this);
    update();
    emit rangeChanged(d_ptr->minimum, d_ptr->maximum);
    emit valuesChanged(d_ptr->lowerValue, d_ptr->upperValue);
}

int QtMaterialRangeSlider::lowerValue() const noexcept { return d_ptr->lowerValue; }
int QtMaterialRangeSlider::upperValue() const noexcept { return d_ptr->upperValue; }

void QtMaterialRangeSlider::setLowerValue(int value)
{
    const int bounded = qBound(d_ptr->minimum, value, d_ptr->upperValue);
    if (d_ptr->lowerValue == bounded) {
        return;
    }
    d_ptr->lowerValue = bounded;
    d_ptr->updateAccessibility(this);
    update();
    emit lowerValueChanged(bounded);
    emit valuesChanged(d_ptr->lowerValue, d_ptr->upperValue);
}

void QtMaterialRangeSlider::setUpperValue(int value)
{
    const int bounded = qBound(d_ptr->lowerValue, value, d_ptr->maximum);
    if (d_ptr->upperValue == bounded) {
        return;
    }
    d_ptr->upperValue = bounded;
    d_ptr->updateAccessibility(this);
    update();
    emit upperValueChanged(bounded);
    emit valuesChanged(d_ptr->lowerValue, d_ptr->upperValue);
}

void QtMaterialRangeSlider::setValues(int lower, int upper)
{
    lower = qBound(d_ptr->minimum, lower, d_ptr->maximum);
    upper = qBound(d_ptr->minimum, upper, d_ptr->maximum);
    if (lower > upper) {
        qSwap(lower, upper);
    }

    const bool lowerChanged = d_ptr->lowerValue != lower;
    const bool upperChanged = d_ptr->upperValue != upper;
    if (!lowerChanged && !upperChanged) {
        return;
    }

    d_ptr->lowerValue = lower;
    d_ptr->upperValue = upper;
    d_ptr->updateAccessibility(this);
    update();
    if (lowerChanged) emit lowerValueChanged(lower);
    if (upperChanged) emit upperValueChanged(upper);
    emit valuesChanged(lower, upper);
}

Qt::Orientation QtMaterialRangeSlider::orientation() const noexcept { return d_ptr->orientation; }

void QtMaterialRangeSlider::setOrientation(Qt::Orientation orientation)
{
    if (d_ptr->orientation == orientation) {
        return;
    }
    d_ptr->orientation = orientation;
    updateGeometry();
    update();
}

QSize QtMaterialRangeSlider::sizeHint() const
{
    return d_ptr->orientation == Qt::Horizontal ? QSize(200, 48) : QSize(48, 200);
}

QSize QtMaterialRangeSlider::minimumSizeHint() const
{
    return d_ptr->orientation == Qt::Horizontal ? QSize(80, 32) : QSize(32, 80);
}

qreal QtMaterialRangeSlider::normalizedForValue(int value) const noexcept
{
    if (d_ptr->maximum <= d_ptr->minimum) {
        return 0.0;
    }
    qreal t = qreal(value - d_ptr->minimum) / qreal(d_ptr->maximum - d_ptr->minimum);
    if (d_ptr->orientation == Qt::Horizontal && layoutDirection() == Qt::RightToLeft) {
        t = 1.0 - t;
    }
    return qBound<qreal>(0.0, t, 1.0);
}

QPointF QtMaterialRangeSlider::handleCenter(Handle handle) const noexcept
{
    const int value = handle == Handle::Lower ? d_ptr->lowerValue : d_ptr->upperValue;
    const qreal t = normalizedForValue(value);
    if (d_ptr->orientation == Qt::Horizontal) {
        const qreal x = kMargin + t * qMax(1, width() - 2 * kMargin);
        return QPointF(x, height() / 2.0);
    }
    const qreal y = height() - kMargin - t * qMax(1, height() - 2 * kMargin);
    return QPointF(width() / 2.0, y);
}

int QtMaterialRangeSlider::valueForPosition(const QPoint& position) const noexcept
{
    qreal t = 0.0;
    if (d_ptr->orientation == Qt::Horizontal) {
        t = qreal(position.x() - kMargin) / qMax(1, width() - 2 * kMargin);
        if (layoutDirection() == Qt::RightToLeft) {
            t = 1.0 - t;
        }
    } else {
        t = qreal(height() - kMargin - position.y()) / qMax(1, height() - 2 * kMargin);
    }
    t = qBound<qreal>(0.0, t, 1.0);
    return d_ptr->minimum + qRound(t * (d_ptr->maximum - d_ptr->minimum));
}

void QtMaterialRangeSlider::moveActiveHandleTo(int value)
{
    if (d_ptr->activeHandle == Handle::Lower) {
        setLowerValue(value);
    } else {
        setUpperValue(value);
    }
}

void QtMaterialRangeSlider::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QPointF lower = handleCenter(Handle::Lower);
    const QPointF upper = handleCenter(Handle::Upper);
    const QColor inactive = palette().color(QPalette::Mid);
    const QColor active = palette().color(QPalette::Highlight);

    painter.setPen(QPen(inactive, kTrackThickness, Qt::SolidLine, Qt::RoundCap));
    if (d_ptr->orientation == Qt::Horizontal) {
        painter.drawLine(QPointF(kMargin, height() / 2.0), QPointF(width() - kMargin, height() / 2.0));
    } else {
        painter.drawLine(QPointF(width() / 2.0, kMargin), QPointF(width() / 2.0, height() - kMargin));
    }

    painter.setPen(QPen(active, kTrackThickness, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(lower, upper);

    painter.setPen(Qt::NoPen);
    painter.setBrush(active);
    painter.drawEllipse(lower, kHandleRadius, kHandleRadius);
    painter.drawEllipse(upper, kHandleRadius, kHandleRadius);

    if (hasFocus()) {
        const QPointF focusCenter = handleCenter(d_ptr->activeHandle);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(active, 2.0));
        painter.drawEllipse(focusCenter, kHandleRadius + 4.0, kHandleRadius + 4.0);
    }
}

void QtMaterialRangeSlider::mousePressEvent(QMouseEvent* event)
{
    if (!event || event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }
    const QPointF p = event->pos();
    const qreal lowerDistance = QLineF(p, handleCenter(Handle::Lower)).length();
    const qreal upperDistance = QLineF(p, handleCenter(Handle::Upper)).length();
    d_ptr->activeHandle = lowerDistance <= upperDistance ? Handle::Lower : Handle::Upper;
    d_ptr->updateAccessibility(this);
    d_ptr->dragging = true;
    setFocus(Qt::MouseFocusReason);
    moveActiveHandleTo(valueForPosition(event->pos()));
    event->accept();
}

void QtMaterialRangeSlider::mouseMoveEvent(QMouseEvent* event)
{
    if (!d_ptr->dragging || !event) {
        QWidget::mouseMoveEvent(event);
        return;
    }
    moveActiveHandleTo(valueForPosition(event->pos()));
    event->accept();
}

void QtMaterialRangeSlider::mouseReleaseEvent(QMouseEvent* event)
{
    if (event && event->button() == Qt::LeftButton && d_ptr->dragging) {
        d_ptr->dragging = false;
        moveActiveHandleTo(valueForPosition(event->pos()));
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void QtMaterialRangeSlider::keyPressEvent(QKeyEvent* event)
{
    if (!event) {
        return;
    }

    int delta = 0;
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Down:
        delta = -1;
        break;
    case Qt::Key_Right:
    case Qt::Key_Up:
        delta = 1;
        break;
    case Qt::Key_Home:
        moveActiveHandleTo(d_ptr->activeHandle == Handle::Lower ? d_ptr->minimum : d_ptr->lowerValue);
        event->accept();
        return;
    case Qt::Key_End:
        moveActiveHandleTo(d_ptr->activeHandle == Handle::Upper ? d_ptr->maximum : d_ptr->upperValue);
        event->accept();
        return;
    default:
        QWidget::keyPressEvent(event);
        return;
    }

    if (d_ptr->orientation == Qt::Horizontal && layoutDirection() == Qt::RightToLeft) {
        delta = -delta;
    }
    moveActiveHandleTo((d_ptr->activeHandle == Handle::Lower ? d_ptr->lowerValue : d_ptr->upperValue) + delta);
    event->accept();
}

void QtMaterialRangeSlider::focusInEvent(QFocusEvent* event)
{
    QWidget::focusInEvent(event);

    if (event) {
        if (event->reason() == Qt::BacktabFocusReason) {
            d_ptr->activeHandle = Handle::Upper;
        } else if (event->reason() == Qt::TabFocusReason) {
            d_ptr->activeHandle = Handle::Lower;
        }
    }

    d_ptr->updateAccessibility(this);
    update();
}

bool QtMaterialRangeSlider::focusNextPrevChild(bool next)
{
    if (next && d_ptr->activeHandle == Handle::Lower) {
        d_ptr->activeHandle = Handle::Upper;
        d_ptr->updateAccessibility(this);
        update();
        return true;
    }

    if (!next && d_ptr->activeHandle == Handle::Upper) {
        d_ptr->activeHandle = Handle::Lower;
        d_ptr->updateAccessibility(this);
        update();
        return true;
    }

    return QWidget::focusNextPrevChild(next);
}

} // namespace QtMaterial
