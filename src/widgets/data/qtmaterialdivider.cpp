#include "qtmaterial/widgets/data/qtmaterialdivider.h"

#include <QEvent>
#include <QPainter>
#include <QSizePolicy>

namespace QtMaterial {

QtMaterialDivider::QtMaterialDivider(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setOrientation(Qt::Horizontal);
}

QtMaterialDivider::QtMaterialDivider(Qt::Orientation orientation, QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setOrientation(orientation);
}

QtMaterialDivider::~QtMaterialDivider() = default;

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
        setMinimumSize(0, 1);
    } else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        setMinimumSize(1, 0);
    }

    updateGeometry();
    update();
}

int QtMaterialDivider::leadingInset() const noexcept
{
    return m_leadingInset;
}

void QtMaterialDivider::setLeadingInset(int value)
{
    if (m_leadingInset == value) {
        return;
    }

    m_leadingInset = qMax(0, value);
    update();
}

int QtMaterialDivider::trailingInset() const noexcept
{
    return m_trailingInset;
}

void QtMaterialDivider::setTrailingInset(int value)
{
    if (m_trailingInset == value) {
        return;
    }

    m_trailingInset = qMax(0, value);
    update();
}

QSize QtMaterialDivider::sizeHint() const
{
    return (m_orientation == Qt::Horizontal) ? QSize(64, 1) : QSize(1, 64);
}

QSize QtMaterialDivider::minimumSizeHint() const
{
    return QSize(1, 1);
}

void QtMaterialDivider::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    if (m_orientation == Qt::Horizontal) {
        const int x = m_leadingInset;
        const int w = qMax(0, width() - m_leadingInset - m_trailingInset);
        painter.fillRect(QRect(x, 0, w, 1), palette().mid());
    } else {
        const int y = m_leadingInset;
        const int h = qMax(0, height() - m_leadingInset - m_trailingInset);
        painter.fillRect(QRect(0, y, 1, h), palette().mid());
    }
}

void QtMaterialDivider::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    update();
}

void QtMaterialDivider::ensureSpecResolved() const
{
    m_specDirty = false;
}

} // namespace QtMaterial