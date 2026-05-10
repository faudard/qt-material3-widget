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

QColor resolvedColor(const QtMaterialDivider* self, const QtMaterialDividerPrivate* d)
{
    if (d->m_color.isValid()) {
        return d->m_color;
    }
    return self->palette().color(QPalette::Mid);
}

void syncAccessibility(QtMaterialDivider* self, QtMaterialDividerPrivate* d)
{
    const QString summary = self->accessibilitySummary();
    if (d->m_decorative) {
        self->setAccessibleName(QString());
        self->setAccessibleDescription(QString());
    } else {
        const QString label = !d->m_accessibilityLabel.trimmed().isEmpty()
            ? d->m_accessibilityLabel.trimmed()
            : orientationText(d->m_orientation);
        self->setAccessibleName(label);
        self->setAccessibleDescription(summary);
    }

    if (d->m_lastAccessibilitySummary != summary) {
        d->m_lastAccessibilitySummary = summary;
        Q_EMIT self->accessibilitySummaryChanged(summary);
    }
}


} // namespace

QtMaterialDivider::QtMaterialDivider(QWidget *parent)
    : QWidget(parent)
{
    d_ptr = std::make_unique<QtMaterialDividerPrivate>();

    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setFocusPolicy(Qt::NoFocus);
    setOrientation(Qt::Horizontal);
    syncAccessibility(this, d_ptr.get());
}

QtMaterialDivider::QtMaterialDivider(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
{
    d_ptr = std::make_unique<QtMaterialDividerPrivate>();

    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setFocusPolicy(Qt::NoFocus);
    setOrientation(orientation);
    syncAccessibility(this, d_ptr.get());
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
    syncAccessibility(this, d_ptr.get());
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
    syncAccessibility(this, d_ptr.get());
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
    syncAccessibility(this, d_ptr.get());
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
    syncAccessibility(this, d_ptr.get());
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
    syncAccessibility(this, d_ptr.get());
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
    syncAccessibility(this, d_ptr.get());
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

void QtMaterialDivider::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.fillRect(lineRect(), resolvedColor(this, d_ptr.get()));
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

} // namespace QtMaterial
