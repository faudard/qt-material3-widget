#include "dashboardcharts.h"

#include <QFontMetrics>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

#include <algorithm>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"

namespace {

QColor color(QtMaterial::ColorRole role)
{
    return QtMaterial::ThemeManager::instance().theme().colorScheme().color(role);
}

} // namespace

LineChartWidget::LineChartWidget(QWidget* parent)
    : QWidget(parent)
    , m_values({42.0, 58.0, 51.0, 76.0, 68.0, 92.0, 83.0, 111.0, 99.0, 128.0, 118.0, 142.0})
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(
        &QtMaterial::ThemeManager::instance(),
        &QtMaterial::ThemeManager::themeChanged,
        this,
        [this](const QtMaterial::Theme&) { update(); });
}

void LineChartWidget::setValues(const QVector<qreal>& values)
{
    if (values == m_values || values.isEmpty()) {
        return;
    }
    m_values = values;
    update();
}

void LineChartWidget::setAccentColor(const QColor& accentColor)
{
    if (m_accentColor == accentColor) {
        return;
    }
    m_accentColor = accentColor;
    update();
}

void LineChartWidget::clearAccentColor()
{
    if (!m_accentColor.isValid()) {
        return;
    }
    m_accentColor = QColor();
    update();
}

QSize LineChartWidget::sizeHint() const
{
    return QSize(620, 270);
}

QSize LineChartWidget::minimumSizeHint() const
{
    return QSize(300, 180);
}

void LineChartWidget::paintEvent(QPaintEvent*)
{
    if (m_values.size() < 2) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF plot = rect().adjusted(16.0, 8.0, -16.0, -28.0);
    if (plot.width() <= 0.0 || plot.height() <= 0.0) {
        return;
    }

    const auto minmax = std::minmax_element(m_values.cbegin(), m_values.cend());
    const qreal minimum = *minmax.first;
    const qreal maximum = *minmax.second;
    const qreal span = std::max<qreal>(1.0, maximum - minimum);

    QColor grid = color(QtMaterial::ColorRole::OutlineVariant);
    grid.setAlpha(115);
    painter.setPen(QPen(grid, 1.0));
    for (int i = 0; i <= 4; ++i) {
        const qreal y = plot.top() + (plot.height() * i / 4.0);
        painter.drawLine(QPointF(plot.left(), y), QPointF(plot.right(), y));
    }

    QPainterPath line;
    QVector<QPointF> points;
    points.reserve(m_values.size());
    for (int i = 0; i < m_values.size(); ++i) {
        const qreal x = plot.left() + plot.width() * i / (m_values.size() - 1.0);
        const qreal normalized = (m_values.at(i) - minimum) / span;
        const qreal y = plot.bottom() - normalized * plot.height();
        points.append(QPointF(x, y));
    }

    line.moveTo(points.first());
    for (int i = 1; i < points.size(); ++i) {
        line.lineTo(points.at(i));
    }

    QPainterPath area(line);
    area.lineTo(points.last().x(), plot.bottom());
    area.lineTo(points.first().x(), plot.bottom());
    area.closeSubpath();

    const QColor accent = m_accentColor.isValid()
        ? m_accentColor
        : color(QtMaterial::ColorRole::Primary);

    QLinearGradient fill(plot.topLeft(), plot.bottomLeft());
    QColor top = accent;
    top.setAlpha(100);
    QColor bottom = accent;
    bottom.setAlpha(4);
    fill.setColorAt(0.0, top);
    fill.setColorAt(1.0, bottom);
    painter.fillPath(area, fill);

    painter.setPen(QPen(accent, 2.7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPath(line);

    painter.setBrush(color(QtMaterial::ColorRole::Surface));
    painter.setPen(QPen(accent, 1.7));
    for (const QPointF& point : points) {
        painter.drawEllipse(point, 3.2, 3.2);
    }

    static const char* months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    painter.setPen(color(QtMaterial::ColorRole::OnSurfaceVariant));
    QFont labelFont = font();
    labelFont.setPointSizeF(std::max<qreal>(8.0, labelFont.pointSizeF() - 1.0));
    painter.setFont(labelFont);
    const QFontMetrics metrics(labelFont);

    for (int i = 0; i < m_values.size() && i < 12; ++i) {
        if (width() < 540 && (i % 2) != 0) {
            continue;
        }
        const qreal x = plot.left() + plot.width() * i / (m_values.size() - 1.0);
        const QString label = QString::fromLatin1(months[i]);
        painter.drawText(
            QRectF(x - 24.0, plot.bottom() + 6.0, 48.0, metrics.height() + 2.0),
            Qt::AlignHCenter | Qt::AlignTop,
            label);
    }
}

DonutChartWidget::DonutChartWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(
        &QtMaterial::ThemeManager::instance(),
        &QtMaterial::ThemeManager::themeChanged,
        this,
        [this](const QtMaterial::Theme&) { update(); });
}

void DonutChartWidget::setValue(int value)
{
    value = qBound(0, value, 100);
    if (value == m_value) {
        return;
    }
    m_value = value;
    update();
}

int DonutChartWidget::value() const noexcept
{
    return m_value;
}

QSize DonutChartWidget::sizeHint() const
{
    return QSize(220, 220);
}

QSize DonutChartWidget::minimumSizeHint() const
{
    return QSize(160, 160);
}

void DonutChartWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const int side = std::max(0, std::min(width(), height()) - 36);
    const QRectF ring(
        (width() - side) / 2.0,
        (height() - side) / 2.0,
        side,
        side);

    const qreal stroke = std::max<qreal>(12.0, side * 0.075);
    painter.setPen(QPen(color(QtMaterial::ColorRole::SurfaceContainerHighest), stroke, Qt::SolidLine, Qt::FlatCap));
    painter.drawArc(ring, 0, 360 * 16);

    const int pending = qMin(20, 100 - m_value);
    const int refund = qMax(0, 100 - m_value - pending);
    const int gap = 2;
    int start = 90 * 16;

    const auto drawSegment = [&](int percentage, const QColor& segmentColor) mutable {
        if (percentage <= 0) {
            return;
        }
        const int span = qMax(0, percentage * 360 * 16 / 100 - gap * 16);
        painter.setPen(QPen(segmentColor, stroke, Qt::SolidLine, Qt::FlatCap));
        painter.drawArc(ring, start, -span);
        start -= percentage * 360 * 16 / 100;
    };

    drawSegment(m_value, color(QtMaterial::ColorRole::Error));
    drawSegment(pending, color(QtMaterial::ColorRole::Tertiary));
    drawSegment(refund, color(QtMaterial::ColorRole::Primary));

    QFont valueFont = font();
    valueFont.setBold(true);
    valueFont.setPointSizeF(std::max<qreal>(18.0, valueFont.pointSizeF() + 8.0));
    painter.setFont(valueFont);
    painter.setPen(color(QtMaterial::ColorRole::OnSurface));
    painter.drawText(ring, Qt::AlignCenter, QStringLiteral("%1%").arg(m_value));
}
