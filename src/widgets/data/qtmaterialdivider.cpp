#include "qtmaterial/widgets/data/qtmaterialdivider.h"

#include <QPainter>
#include <QPalette>
#include <QSizePolicy>
#include <QtGlobal>
#include <memory>
#include <qcoreevent.h>

namespace QtMaterial {

struct QtMaterialDividerPrivate
{
    Qt::Orientation m_orientation = Qt::Horizontal;
    int m_leadingInset = 0;
    int m_trailingInset = 0;
    int m_thickness = 1;
    QColor m_color;
    bool m_decorative = true;
    QString m_accessibilityLabel;
    QString m_lastAccessibilitySummary;
};



namespace {

QString orientationText(Qt::Orientation orientation)
{
    return orientation == Qt::Horizontal
        ? QStringLiteral("Horizontal separator")
        : QStringLiteral("Vertical separator");
}

} // namespace

QtMaterialDivider::QtMaterialDivider(QWidget *parent)
    : QWidget(parent)
{
    d_ptr = std::make_unique<QtMaterialDividerPrivate>();

    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFocusPolicy(Qt::NoFocus);
    setOrientation(Qt::Horizontal);
    syncAccessibility();
}

QtMaterialDivider::QtMaterialDivider(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
{
    d_ptr = std::make_unique<QtMaterialDividerPrivate>();

    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFocusPolicy(Qt::NoFocus);
    setOrientation(orientation);
    syncAccessibility();
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
    return d_ptr->m_orientation;
}

void QtMaterialDivider::setOrientation(Qt::Orientation orientation)
{
    if (d_ptr->m_orientation == orientation) {
        return;
    }

    d_ptr->m_orientation = orientation;

    if (d_ptr->m_orientation == Qt::Horizontal) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setMinimumSize(0, d_ptr->m_thickness);
    } else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        setMinimumSize(d_ptr->m_thickness, 0);
    }

    emit orientationChanged(d_ptr->m_orientation);
    updateGeometry();
    syncAccessibility();
    update();
}

int QtMaterialDivider::leadingInset() const noexcept
{
    return d_ptr->m_leadingInset;
}

void QtMaterialDivider::setLeadingInset(int value)
{
    const int normalized = qMax(0, value);
    if (d_ptr->m_leadingInset == normalized) {
        return;
    }

    d_ptr->m_leadingInset = normalized;
    emit leadingInsetChanged(d_ptr->m_leadingInset);
    syncAccessibility();
    update();
}

int QtMaterialDivider::trailingInset() const noexcept
{
    return d_ptr->m_trailingInset;
}

void QtMaterialDivider::setTrailingInset(int value)
{
    const int normalized = qMax(0, value);
    if (d_ptr->m_trailingInset == normalized) {
        return;
    }

    d_ptr->m_trailingInset = normalized;
    emit trailingInsetChanged(d_ptr->m_trailingInset);
    syncAccessibility();
    update();
}

int QtMaterialDivider::thickness() const noexcept
{
    return d_ptr->m_thickness;
}

void QtMaterialDivider::setThickness(int value)
{
    const int normalized = qMax(1, value);
    if (d_ptr->m_thickness == normalized) {
        return;
    }

    d_ptr->m_thickness = normalized;

    if (d_ptr->m_orientation == Qt::Horizontal) {
        setMinimumSize(0, d_ptr->m_thickness);
    } else {
        setMinimumSize(d_ptr->m_thickness, 0);
    }

    emit thicknessChanged(d_ptr->m_thickness);
    updateGeometry();
    syncAccessibility();
    update();
}

QColor QtMaterialDivider::color() const noexcept
{
    return d_ptr->m_color;
}

void QtMaterialDivider::setColor(const QColor &color)
{
    if (d_ptr->m_color == color) {
        return;
    }

    d_ptr->m_color = color;
    emit colorChanged(d_ptr->m_color);
    update();
}

void QtMaterialDivider::resetColor()
{
    setColor(QColor());
}

bool QtMaterialDivider::isDecorative() const noexcept
{
    return d_ptr->m_decorative;
}

void QtMaterialDivider::setDecorative(bool decorative)
{
    if (d_ptr->m_decorative == decorative) {
        return;
    }

    d_ptr->m_decorative = decorative;
    emit decorativeChanged(d_ptr->m_decorative);
    syncAccessibility();
}

QString QtMaterialDivider::accessibilityLabel() const
{
    return d_ptr->m_accessibilityLabel;
}

void QtMaterialDivider::setAccessibilityLabel(const QString &label)
{
    if (d_ptr->m_accessibilityLabel == label) {
        return;
    }

    d_ptr->m_accessibilityLabel = label;
    emit accessibilityLabelChanged(d_ptr->m_accessibilityLabel);
    syncAccessibility();
}

QString QtMaterialDivider::accessibilitySummary() const
{
    if (d_ptr->m_decorative) {
        return QString();
    }

    if (!d_ptr->m_accessibilityLabel.trimmed().isEmpty()) {
        return d_ptr->m_accessibilityLabel.trimmed();
    }

    QString summary = orientationText(d_ptr->m_orientation);
    if (d_ptr->m_leadingInset > 0 || d_ptr->m_trailingInset > 0) {
        summary += QStringLiteral(", inset");
    }
    if (d_ptr->m_thickness > 1) {
        summary += QStringLiteral(", %1 px thick").arg(d_ptr->m_thickness);
    }

    return summary;
}

QRect QtMaterialDivider::lineRect() const
{
    const int lineThickness = qMax(1, d_ptr->m_thickness);

    if (d_ptr->m_orientation == Qt::Horizontal) {
        const int leading = qMax(0, d_ptr->m_leadingInset);
        const int trailing = qMax(0, d_ptr->m_trailingInset);
        const int x = layoutDirection() == Qt::RightToLeft ? trailing : leading;
        const int w = qMax(0, width() - leading - trailing);
        const int y = qMax(0, (height() - lineThickness) / 2);
        return QRect(x, y, w, lineThickness);
    }

    const int y = qMax(0, d_ptr->m_leadingInset);
    const int trailing = qMax(0, d_ptr->m_trailingInset);
    const int h = qMax(0, height() - y - trailing);
    const int x = qMax(0, (width() - lineThickness) / 2);
    return QRect(x, y, lineThickness, h);
}

QSize QtMaterialDivider::sizeHint() const
{
    return (d_ptr->m_orientation == Qt::Horizontal) ? QSize(64, d_ptr->m_thickness) : QSize(d_ptr->m_thickness, 64);
}

QSize QtMaterialDivider::minimumSizeHint() const
{
    return (d_ptr->m_orientation == Qt::Horizontal) ? QSize(0, d_ptr->m_thickness) : QSize(d_ptr->m_thickness, 0);
}

QColor QtMaterialDivider::resolvedColor() const
{
    if (d_ptr->m_color.isValid()) {
        return d_ptr->m_color;
    }

    return palette().color(QPalette::Mid);
}

void QtMaterialDivider::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.fillRect(lineRect(), resolvedColor());
}

void QtMaterialDivider::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);

    switch (event->type()) {
    case QEvent::PaletteChange:
    case QEvent::StyleChange:
        update();
        break;
    case QEvent::LayoutDirectionChange:
        update();
        break;
    default:
        break;
    }
}

void QtMaterialDivider::syncAccessibility()
{
    const QString summary = accessibilitySummary();

    if (d_ptr->m_decorative) {
        setAccessibleName(QString());
        setAccessibleDescription(QString());
    } else {
        const QString label = !d_ptr->m_accessibilityLabel.trimmed().isEmpty()
            ? d_ptr->m_accessibilityLabel.trimmed()
            : orientationText(d_ptr->m_orientation);
        setAccessibleName(label);
        setAccessibleDescription(summary);
    }

    if (d_ptr->m_lastAccessibilitySummary != summary) {
        d_ptr->m_lastAccessibilitySummary = summary;
        emit accessibilitySummaryChanged(summary);
    }
}

} // namespace QtMaterial
