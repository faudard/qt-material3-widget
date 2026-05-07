#include "qtmaterial/widgets/data/qtmaterialdivider.h"

#include <QPainter>
#include <QPalette>
#include <QSizePolicy>
#include <QtGlobal>

namespace QtMaterial {

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
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFocusPolicy(Qt::NoFocus);
    setOrientation(Qt::Horizontal);
    syncAccessibility();
}

QtMaterialDivider::QtMaterialDivider(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
{
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
    syncAccessibility();
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
    syncAccessibility();
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
    syncAccessibility();
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
    syncAccessibility();
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

bool QtMaterialDivider::isDecorative() const noexcept
{
    return m_decorative;
}

void QtMaterialDivider::setDecorative(bool decorative)
{
    if (m_decorative == decorative) {
        return;
    }

    m_decorative = decorative;
    emit decorativeChanged(m_decorative);
    syncAccessibility();
}

QString QtMaterialDivider::accessibilityLabel() const
{
    return m_accessibilityLabel;
}

void QtMaterialDivider::setAccessibilityLabel(const QString &label)
{
    if (m_accessibilityLabel == label) {
        return;
    }

    m_accessibilityLabel = label;
    emit accessibilityLabelChanged(m_accessibilityLabel);
    syncAccessibility();
}

QString QtMaterialDivider::accessibilitySummary() const
{
    if (m_decorative) {
        return QString();
    }

    if (!m_accessibilityLabel.trimmed().isEmpty()) {
        return m_accessibilityLabel.trimmed();
    }

    QString summary = orientationText(m_orientation);
    if (m_leadingInset > 0 || m_trailingInset > 0) {
        summary += QStringLiteral(", inset");
    }
    if (m_thickness > 1) {
        summary += QStringLiteral(", %1 px thick").arg(m_thickness);
    }

    return summary;
}

QRect QtMaterialDivider::lineRect() const
{
    const int lineThickness = qMax(1, m_thickness);

    if (m_orientation == Qt::Horizontal) {
        const int leading = qMax(0, m_leadingInset);
        const int trailing = qMax(0, m_trailingInset);
        const int x = layoutDirection() == Qt::RightToLeft ? trailing : leading;
        const int w = qMax(0, width() - leading - trailing);
        const int y = qMax(0, (height() - lineThickness) / 2);
        return QRect(x, y, w, lineThickness);
    }

    const int y = qMax(0, m_leadingInset);
    const int trailing = qMax(0, m_trailingInset);
    const int h = qMax(0, height() - y - trailing);
    const int x = qMax(0, (width() - lineThickness) / 2);
    return QRect(x, y, lineThickness, h);
}

QSize QtMaterialDivider::sizeHint() const
{
    return (m_orientation == Qt::Horizontal) ? QSize(64, m_thickness) : QSize(m_thickness, 64);
}

QSize QtMaterialDivider::minimumSizeHint() const
{
    return (m_orientation == Qt::Horizontal) ? QSize(0, m_thickness) : QSize(m_thickness, 0);
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

    if (m_decorative) {
        setAccessibleName(QString());
        setAccessibleDescription(QString());
    } else {
        const QString label = !m_accessibilityLabel.trimmed().isEmpty()
            ? m_accessibilityLabel.trimmed()
            : orientationText(m_orientation);
        setAccessibleName(label);
        setAccessibleDescription(summary);
    }

    if (m_lastAccessibilitySummary != summary) {
        m_lastAccessibilitySummary = summary;
        emit accessibilitySummaryChanged(summary);
    }
}

} // namespace QtMaterial
