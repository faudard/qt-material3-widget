#include "qtmaterial/widgets/inputs/qtmaterialrangeslider.h"

#include <QKeyEvent>
#include <QLineF>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

namespace QtMaterial {
namespace {
constexpr int kMargin = 14;
constexpr qreal kTrackThickness = 4.0;
constexpr qreal kHandleRadius = 10.0;
}

QtMaterialRangeSlider::QtMaterialRangeSlider(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("qtmaterial_range_slider"));
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(80, 32);
    setAccessibleName(tr("Range slider"));
}

QtMaterialRangeSlider::~QtMaterialRangeSlider() = default;

int QtMaterialRangeSlider::minimum() const noexcept { return m_minimum; }
int QtMaterialRangeSlider::maximum() const noexcept { return m_maximum; }

void QtMaterialRangeSlider::setMinimum(int value) { setRange(value, m_maximum); }
void QtMaterialRangeSlider::setMaximum(int value) { setRange(m_minimum, value); }

void QtMaterialRangeSlider::setRange(int minimum, int maximum)
{
    if (minimum > maximum) {
        qSwap(minimum, maximum);
    }
    if (m_minimum == minimum && m_maximum == maximum) {
        return;
    }
    m_minimum = minimum;
    m_maximum = maximum;
    const int lower = qBound(m_minimum, m_lowerValue, m_maximum);
    const int upper = qBound(lower, m_upperValue, m_maximum);
    m_lowerValue = lower;
    m_upperValue = upper;
    update();
    emit rangeChanged(m_minimum, m_maximum);
    emit valuesChanged(m_lowerValue, m_upperValue);
}

int QtMaterialRangeSlider::lowerValue() const noexcept { return m_lowerValue; }
int QtMaterialRangeSlider::upperValue() const noexcept { return m_upperValue; }

void QtMaterialRangeSlider::setLowerValue(int value)
{
    const int bounded = qBound(m_minimum, value, m_upperValue);
    if (m_lowerValue == bounded) {
        return;
    }
    m_lowerValue = bounded;
    update();
    emit lowerValueChanged(bounded);
    emit valuesChanged(m_lowerValue, m_upperValue);
}

void QtMaterialRangeSlider::setUpperValue(int value)
{
    const int bounded = qBound(m_lowerValue, value, m_maximum);
    if (m_upperValue == bounded) {
        return;
    }
    m_upperValue = bounded;
    update();
    emit upperValueChanged(bounded);
    emit valuesChanged(m_lowerValue, m_upperValue);
}

void QtMaterialRangeSlider::setValues(int lower, int upper)
{
    lower = qBound(m_minimum, lower, m_maximum);
    upper = qBound(m_minimum, upper, m_maximum);
    if (lower > upper) {
        qSwap(lower, upper);
    }

    const bool lowerChanged = m_lowerValue != lower;
    const bool upperChanged = m_upperValue != upper;
    if (!lowerChanged && !upperChanged) {
        return;
    }

    m_lowerValue = lower;
    m_upperValue = upper;
    update();
    if (lowerChanged) emit lowerValueChanged(lower);
    if (upperChanged) emit upperValueChanged(upper);
    emit valuesChanged(lower, upper);
}

Qt::Orientation QtMaterialRangeSlider::orientation() const noexcept { return m_orientation; }

void QtMaterialRangeSlider::setOrientation(Qt::Orientation orientation)
{
    if (m_orientation == orientation) {
        return;
    }
    m_orientation = orientation;
    updateGeometry();
    update();
}

QSize QtMaterialRangeSlider::sizeHint() const
{
    return m_orientation == Qt::Horizontal ? QSize(200, 48) : QSize(48, 200);
}

QSize QtMaterialRangeSlider::minimumSizeHint() const
{
    return m_orientation == Qt::Horizontal ? QSize(80, 32) : QSize(32, 80);
}

qreal QtMaterialRangeSlider::normalizedForValue(int value) const noexcept
{
    if (m_maximum <= m_minimum) {
        return 0.0;
    }
    qreal t = qreal(value - m_minimum) / qreal(m_maximum - m_minimum);
    if (m_orientation == Qt::Horizontal && layoutDirection() == Qt::RightToLeft) {
        t = 1.0 - t;
    }
    return qBound<qreal>(0.0, t, 1.0);
}

QPointF QtMaterialRangeSlider::handleCenter(Handle handle) const noexcept
{
    const int value = handle == Handle::Lower ? m_lowerValue : m_upperValue;
    const qreal t = normalizedForValue(value);
    if (m_orientation == Qt::Horizontal) {
        const qreal x = kMargin + t * qMax(1, width() - 2 * kMargin);
        return QPointF(x, height() / 2.0);
    }
    const qreal y = height() - kMargin - t * qMax(1, height() - 2 * kMargin);
    return QPointF(width() / 2.0, y);
}

int QtMaterialRangeSlider::valueForPosition(const QPoint& position) const noexcept
{
    qreal t = 0.0;
    if (m_orientation == Qt::Horizontal) {
        t = qreal(position.x() - kMargin) / qMax(1, width() - 2 * kMargin);
        if (layoutDirection() == Qt::RightToLeft) {
            t = 1.0 - t;
        }
    } else {
        t = qreal(height() - kMargin - position.y()) / qMax(1, height() - 2 * kMargin);
    }
    t = qBound<qreal>(0.0, t, 1.0);
    return m_minimum + qRound(t * (m_maximum - m_minimum));
}

void QtMaterialRangeSlider::moveActiveHandleTo(int value)
{
    if (m_activeHandle == Handle::Lower) {
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
    if (m_orientation == Qt::Horizontal) {
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
        const QPointF focusCenter = handleCenter(m_activeHandle);
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
    m_activeHandle = lowerDistance <= upperDistance ? Handle::Lower : Handle::Upper;
    m_dragging = true;
    setFocus(Qt::MouseFocusReason);
    moveActiveHandleTo(valueForPosition(event->pos()));
    event->accept();
}

void QtMaterialRangeSlider::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_dragging || !event) {
        QWidget::mouseMoveEvent(event);
        return;
    }
    moveActiveHandleTo(valueForPosition(event->pos()));
    event->accept();
}

void QtMaterialRangeSlider::mouseReleaseEvent(QMouseEvent* event)
{
    if (event && event->button() == Qt::LeftButton && m_dragging) {
        m_dragging = false;
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
        moveActiveHandleTo(m_activeHandle == Handle::Lower ? m_minimum : m_lowerValue);
        event->accept();
        return;
    case Qt::Key_End:
        moveActiveHandleTo(m_activeHandle == Handle::Upper ? m_maximum : m_upperValue);
        event->accept();
        return;
    case Qt::Key_Tab:
        m_activeHandle = m_activeHandle == Handle::Lower ? Handle::Upper : Handle::Lower;
        update();
        event->accept();
        return;
    default:
        QWidget::keyPressEvent(event);
        return;
    }

    if (m_orientation == Qt::Horizontal && layoutDirection() == Qt::RightToLeft) {
        delta = -delta;
    }
    moveActiveHandleTo((m_activeHandle == Handle::Lower ? m_lowerValue : m_upperValue) + delta);
    event->accept();
}

} // namespace QtMaterial
