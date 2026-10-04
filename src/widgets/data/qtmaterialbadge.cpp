#include "qtmaterial/widgets/data/qtmaterialbadge.h"

#include <QFontMetrics>
#include <QPainter>

#include "../resolution/qtmaterialmissingmaterial3specresolution_p.h"

namespace QtMaterial {

namespace {

constexpr int kDotSize = 6;
constexpr int kBadgeHeight = 16;
constexpr int kHorizontalPadding = 4;

} // namespace

QtMaterialBadge::QtMaterialBadge(QWidget* parent)
    : QtMaterialWidget(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAccessibleName(tr("Badge"));
    syncAccessibility();
}

QtMaterialBadge::~QtMaterialBadge() = default;

int QtMaterialBadge::count() const noexcept
{
    return m_count;
}

void QtMaterialBadge::setCount(int count)
{
    count = qMax(0, count);
    if (m_count == count) {
        return;
    }

    m_count = count;
    syncAccessibility();
    updateGeometry();
    update();
    Q_EMIT countChanged(count);
}

int QtMaterialBadge::maximum() const noexcept
{
    return m_maximum;
}

void QtMaterialBadge::setMaximum(int maximum)
{
    maximum = qMax(1, maximum);
    if (m_maximum == maximum) {
        return;
    }

    m_maximum = maximum;
    syncAccessibility();
    updateGeometry();
    update();
    Q_EMIT maximumChanged(maximum);
}

bool QtMaterialBadge::isDot() const noexcept
{
    return m_dot;
}

void QtMaterialBadge::setDot(bool dot)
{
    if (m_dot == dot) {
        return;
    }

    m_dot = dot;
    syncAccessibility();
    updateGeometry();
    update();
    Q_EMIT dotChanged(dot);
}

QString QtMaterialBadge::displayText() const
{
    if (m_dot) {
        return {};
    }

    if (m_count > m_maximum) {
        return QStringLiteral("%1+").arg(m_maximum);
    }
    return QString::number(m_count);
}

QSize QtMaterialBadge::sizeHint() const
{
    if (m_dot) {
        return QSize(kDotSize, kDotSize);
    }

    const QFontMetrics fm(font());
    const int textWidth = fm.horizontalAdvance(displayText());
    return QSize(qMax(kBadgeHeight, textWidth + 2 * kHorizontalPadding), kBadgeHeight);
}

QSize QtMaterialBadge::minimumSizeHint() const
{
    return m_dot ? QSize(kDotSize, kDotSize) : QSize(kBadgeHeight, kBadgeHeight);
}

void QtMaterialBadge::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const auto spec =
        MissingMaterial3SpecResolution::badgeSpec(
            theme(),
            palette());

    painter.setPen(Qt::NoPen);
    painter.setBrush(spec.containerColor);
    painter.drawRoundedRect(rect(), height() / 2.0, height() / 2.0);

    if (!m_dot) {
        painter.setPen(spec.contentColor);
        painter.drawText(rect(), Qt::AlignCenter, displayText());
    }
}

void QtMaterialBadge::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialWidget::themeChangedEvent(theme);
    update();
}

void QtMaterialBadge::syncAccessibility()
{
    setAccessibleDescription(
        m_dot
            ? tr("New content")
            : tr("%1 notifications").arg(displayText()));
}

} // namespace QtMaterial
