#include "qtmaterial/widgets/data/qtmaterialdivider.h"

#include <QEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>

namespace QtMaterial {

QtMaterialDivider::QtMaterialDivider(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setOrientation(Qt::Horizontal);
}

QtMaterialDivider::QtMaterialDivider(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setOrientation(orientation);
}

QtMaterialDivider::~QtMaterialDivider() = default;

QtMaterialDivider *QtMaterialDivider::horizontal(QWidget *parent)
{
    return new QtMaterialDivider(Qt::Horizontal, parent);
}

QtMaterialDivider *QtMaterialDivider::vertical(QWidget *parent)
{
    return new QtMaterialDivider(Qt::Vertical, parent);
}

Qt::Orientation QtMaterialDivider::orientation() const noexcept
{
    return m_orientation;
}

void QtMaterialDivider::setOrientation(Qt::Orientation orientation)
{
    if (m_orientation == orientation) {
        return;
    }

    m_orientation = orientation;

    if (m_orientation == Qt::Horizontal) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setMinimumSize(0, m_thickness);
    } else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        setMinimumSize(m_thickness, 0);
    }

    emit orientationChanged(m_orientation);
    updateGeometry();
    update();
}

int QtMaterialDivider::leadingInset() const noexcept
{
    return m_leadingInset;
}

void QtMaterialDivider::setLeadingInset(int value)
{
    const int normalized = qMax(0, value);
    if (m_leadingInset == normalized) {
        return;
    }

    m_leadingInset = normalized;
    emit leadingInsetChanged(m_leadingInset);
    update();
}

int QtMaterialDivider::trailingInset() const noexcept
{
    return m_trailingInset;
}

void QtMaterialDivider::setTrailingInset(int value)
{
    const int normalized = qMax(0, value);
    if (m_trailingInset == normalized) {
        return;
    }

    m_trailingInset = normalized;
    emit trailingInsetChanged(m_trailingInset);
    update();
}

int QtMaterialDivider::thickness() const noexcept
{
    return m_thickness;
}

void QtMaterialDivider::setThickness(int value)
{
    const int normalized = qMax(1, value);
    if (m_thickness == normalized) {
        return;
    }

    m_thickness = normalized;

    if (m_orientation == Qt::Horizontal) {
        setMinimumSize(0, m_thickness);
    } else {
        setMinimumSize(m_thickness, 0);
    }

    emit thicknessChanged(m_thickness);
    updateGeometry();
    update();
}

QColor QtMaterialDivider::color() const noexcept
{
    return m_color;
}

void QtMaterialDivider::setColor(const QColor &color)
{
    if (m_color == color) {
        return;
    }

    m_color = color;
    emit colorChanged(m_color);
    update();
}

void QtMaterialDivider::resetColor()
{
    setColor(QColor());
}

QSize QtMaterialDivider::sizeHint() const
{
    return (m_orientation == Qt::Horizontal)
        ? QSize(64, m_thickness)
        : QSize(m_thickness, 64);
}

QSize QtMaterialDivider::minimumSizeHint() const
{
    return (m_orientation == Qt::Horizontal)
        ? QSize(0, m_thickness)
        : QSize(m_thickness, 0);
}

QColor QtMaterialDivider::resolvedColor() const
{
    if (m_color.isValid()) {
        return m_color;
    }

    return palette().color(QPalette::Mid);
}

void QtMaterialDivider::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    const int thickness = qMax(1, m_thickness);
    const QColor color = resolvedColor();

    if (m_orientation == Qt::Horizontal) {
        const int x = qMax(0, m_leadingInset);
        const int trailing = qMax(0, m_trailingInset);
        const int w = qMax(0, width() - x - trailing);
        const int y = qMax(0, (height() - thickness) / 2);

        painter.fillRect(QRect(x, y, w, thickness), color);
    } else {
        const int y = qMax(0, m_leadingInset);
        const int trailing = qMax(0, m_trailingInset);
        const int h = qMax(0, height() - y - trailing);
        const int x = qMax(0, (width() - thickness) / 2);

        painter.fillRect(QRect(x, y, thickness, h), color);
    }
}

void QtMaterialDivider::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);

    switch (event->type()) {
    case QEvent::PaletteChange:
    case QEvent::StyleChange:
        update();
        break;
    default:
        break;
    }
}

} // namespace QtMaterial
