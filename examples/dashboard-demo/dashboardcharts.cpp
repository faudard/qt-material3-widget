#include "dashboardcharts.h"
#include "dashboarddemostyle.h"

#include <QAbstractItemView>
#include <QBitmap>
#include <QComboBox>
#include <QEvent>
#include <QFontMetrics>
#include <QFrame>
#include <QHeaderView>
#include <QListView>
#include <QWidget>
#include <QSizePolicy>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

#include <algorithm>

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"
#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"

namespace {

QColor color(QtMaterial::ColorRole role)
{
    return QtMaterial::ThemeManager::instance().theme().colorScheme().color(role);
}

class DashboardComboItemDelegate final : public QStyledItemDelegate
{
public:
    explicit DashboardComboItemDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
    {
    }

    QSize sizeHint(
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override
    {
        QSize result = QStyledItemDelegate::sizeHint(option, index);
        result.setHeight(qMax(result.height(), 44));
        return result;
    }
};

class DashboardComboPopupFilter final : public QObject
{
public:
    explicit DashboardComboPopupFilter(QObject* parent)
        : QObject(parent)
    {
    }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if (event->type() != QEvent::Show
            && event->type() != QEvent::Resize) {
            return QObject::eventFilter(watched, event);
        }

        auto* popup = qobject_cast<QWidget*>(watched);
        if (!popup || popup->width() <= 0 || popup->height() <= 0) {
            return QObject::eventFilter(watched, event);
        }

        QBitmap mask(popup->size());
        mask.fill(Qt::color0);

        QPainter painter(&mask);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::color1);
        painter.drawRoundedRect(
            popup->rect().adjusted(0, 0, -1, -1),
            16.0,
            16.0);
        painter.end();

        popup->setMask(mask);
        return QObject::eventFilter(watched, event);
    }
};

void polishDemoCombo(QtMaterial::QtMaterialComboBox* combo)
{
    if (!combo) {
        return;
    }

    combo->setMinimumHeight(42);
    combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    combo->setMaxVisibleItems(8);
    combo->setMinimumWidth(
        qMax(combo->minimumWidth(), combo->sizeHint().width() + 12));

    if (QAbstractItemView* view = combo->view()) {
        view->setFrameShape(QFrame::NoFrame);
        view->setMinimumWidth(qMax(combo->minimumWidth(), 190));

        QWidget* popup = view->window();
        if (popup) {
            popup->setObjectName(QStringLiteral("dashboardComboPopup"));
            popup->setAttribute(Qt::WA_StyledBackground, true);
            popup->setStyleSheet(QStringLiteral(
                "#dashboardComboPopup {"
                " background:%1;"
                " border:1px solid %2;"
                " border-radius:16px;"
                " }")
                .arg(color(QtMaterial::ColorRole::Surface).name(QColor::HexRgb))
                .arg(color(QtMaterial::ColorRole::OutlineVariant).name(QColor::HexRgb)));

            if (!popup->property("dashboardRoundedPopup").toBool()) {
                popup->installEventFilter(
                    new DashboardComboPopupFilter(popup));
                popup->setProperty("dashboardRoundedPopup", true);
            }
        }

        if (!view->property("dashboardDemoStyled").toBool()) {
            view->setItemDelegate(new DashboardComboItemDelegate(view));
            view->setProperty("dashboardDemoStyled", true);

            if (auto* list = qobject_cast<QListView*>(view)) {
                list->setUniformItemSizes(true);
                list->setSpacing(2);
            }
        }
    }
}

void polishDemoButton(QtMaterial::QtMaterialTextButton* button)
{
    if (!button) {
        return;
    }

    button->setDensity(QtMaterial::Density::Comfortable);
    button->setMinimumHeight(40);
    button->setMaximumHeight(44);

    QSizePolicy policy = button->sizePolicy();
    policy.setVerticalPolicy(QSizePolicy::Fixed);
    button->setSizePolicy(policy);

    QFont font = button->font();
    font.setWeight(QFont::DemiBold);
    button->setFont(font);
}

void polishDemoTable(QtMaterial::QtMaterialTable* table)
{
    if (!table) {
        return;
    }

    const QColor surface = color(QtMaterial::ColorRole::Surface);
    const QColor header = color(QtMaterial::ColorRole::SurfaceContainerLow);
    const QColor hover = color(QtMaterial::ColorRole::SurfaceContainer);
    const QColor divider = color(QtMaterial::ColorRole::OutlineVariant);
    const QColor foreground = color(QtMaterial::ColorRole::OnSurface);
    const QColor muted = color(QtMaterial::ColorRole::OnSurfaceVariant);
    const QColor selected = color(QtMaterial::ColorRole::PrimaryContainer);
    const QColor selectedText = color(QtMaterial::ColorRole::OnPrimaryContainer);

    table->setAlternatingRowColors(false);
    table->setShowGrid(false);
    table->setFrameShape(QFrame::NoFrame);
    table->setCornerButtonEnabled(false);
    table->setWordWrap(false);
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(table->dense() ? 52 : 64);
    table->horizontalHeader()->setMinimumHeight(52);
    table->horizontalHeader()->setMaximumHeight(52);

    QFont headerFont = table->horizontalHeader()->font();
    headerFont.setWeight(QFont::DemiBold);
    table->horizontalHeader()->setFont(headerFont);

    table->setStyleSheet(QStringLiteral(
        "QTableView {"
        " background:%1;"
        " color:%2;"
        " border:0;"
        " border-radius:0;"
        " outline:0;"
        " gridline-color:transparent;"
        " selection-background-color:%7;"
        " selection-color:%8;"
        " }"
        "QTableView::item {"
        " padding:0 16px;"
        " border:0;"
        " border-bottom:1px dotted %3;"
        " }"
        "QTableView::item:hover { background:%4; }"
        "QTableView::item:selected { background:%7; color:%8; }"
        "QHeaderView { background:%5; border:0; }"
        "QHeaderView::section {"
        " background:%5;"
        " color:%6;"
        " padding:0 16px;"
        " border:0;"
        " font-weight:600;"
        " }"
        "QTableCornerButton::section { background:%5; border:0; }"
        "QScrollBar { background:transparent; }")
        .arg(surface.name(QColor::HexRgb))
        .arg(foreground.name(QColor::HexRgb))
        .arg(divider.name(QColor::HexRgb))
        .arg(hover.name(QColor::HexRgb))
        .arg(header.name(QColor::HexRgb))
        .arg(muted.name(QColor::HexRgb))
        .arg(selected.name(QColor::HexRgb))
        .arg(selectedText.name(QColor::HexRgb)));
}

QPainterPath smoothPath(const QVector<QPointF>& points)
{
    QPainterPath path;
    if (points.isEmpty()) {
        return path;
    }

    path.moveTo(points.first());
    if (points.size() == 1) {
        return path;
    }

    for (int i = 0; i < points.size() - 1; ++i) {
        const QPointF p0 = i > 0 ? points.at(i - 1) : points.at(i);
        const QPointF p1 = points.at(i);
        const QPointF p2 = points.at(i + 1);
        const QPointF p3 =
            i + 2 < points.size()
                ? points.at(i + 2)
                : points.at(i + 1);

        const QPointF c1(
            p1.x() + (p2.x() - p0.x()) / 6.0,
            p1.y() + (p2.y() - p0.y()) / 6.0);
        const QPointF c2(
            p2.x() - (p3.x() - p1.x()) / 6.0,
            p2.y() - (p3.y() - p1.y()) / 6.0);

        path.cubicTo(c1, c2, p2);
    }
    return path;
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

    QVector<QPointF> points;
    points.reserve(m_values.size());
    for (int i = 0; i < m_values.size(); ++i) {
        const qreal x = plot.left() + plot.width() * i / (m_values.size() - 1.0);
        const qreal normalized = (m_values.at(i) - minimum) / span;
        const qreal y = plot.bottom() - normalized * plot.height();
        points.append(QPointF(x, y));
    }

    const QPainterPath line = smoothPath(points);
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

    auto drawSegment = [&](int percentage, const QColor& segmentColor) {
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


namespace DashboardDemoStyle {

void polishControls(QWidget* root)
{
    if (!root || root->property("dashboardDemoControlsPolished").toBool()) {
        return;
    }

    root->setProperty("dashboardDemoControlsPolished", true);

    const auto combos =
        root->findChildren<QtMaterial::QtMaterialComboBox*>();
    for (QtMaterial::QtMaterialComboBox* combo : combos) {
        polishDemoCombo(combo);
    }

    const auto buttons =
        root->findChildren<QtMaterial::QtMaterialTextButton*>();
    for (QtMaterial::QtMaterialTextButton* button : buttons) {
        polishDemoButton(button);
    }

    const auto segmented =
        root->findChildren<QtMaterial::QtMaterialSegmentedButton*>();
    for (QtMaterial::QtMaterialSegmentedButton* control : segmented) {
        control->setMinimumHeight(42);
        control->setMinimumWidth(
            qMax(control->minimumWidth(), control->sizeHint().width()));
    }
}

void apply(QWidget* root)
{
    if (!root) {
        return;
    }

    polishControls(root);

    const QColor surface = color(QtMaterial::ColorRole::Surface);
    const QColor surfaceLow = color(QtMaterial::ColorRole::SurfaceContainerLow);
    const QColor surfaceHigh = color(QtMaterial::ColorRole::SurfaceContainerHigh);
    const QColor onSurface = color(QtMaterial::ColorRole::OnSurface);
    const QColor outline = color(QtMaterial::ColorRole::OutlineVariant);
    const QColor primary = color(QtMaterial::ColorRole::Primary);
    const QColor primaryContainer = color(QtMaterial::ColorRole::PrimaryContainer);
    const QColor onPrimaryContainer = color(QtMaterial::ColorRole::OnPrimaryContainer);

    const QString comboStyle = QStringLiteral(
        "QComboBox {"
        " background:%1;"
        " color:%2;"
        " border:1px solid %3;"
        " border-radius:14px;"
        " padding:7px 34px 7px 14px;"
        " min-height:28px;"
        " selection-background-color:%4;"
        " selection-color:%5;"
        " }"
        "QComboBox:hover {"
        " border-color:%6;"
        " background:%7;"
        " }"
        "QComboBox:focus {"
        " border:2px solid %6;"
        " padding:6px 33px 6px 13px;"
        " }"
        "QComboBox::drop-down {"
        " subcontrol-origin:padding;"
        " subcontrol-position:top right;"
        " width:32px;"
        " border:0;"
        " background:transparent;"
        " }")
        .arg(surface.name(QColor::HexRgb))
        .arg(onSurface.name(QColor::HexRgb))
        .arg(outline.name(QColor::HexRgb))
        .arg(primaryContainer.name(QColor::HexRgb))
        .arg(onPrimaryContainer.name(QColor::HexRgb))
        .arg(primary.name(QColor::HexRgb))
        .arg(surfaceLow.name(QColor::HexRgb));

    const QString popupStyle = QStringLiteral(
        "QAbstractItemView {"
        " background:%1;"
        " color:%2;"
        " border:1px solid %3;"
        " border-radius:16px;"
        " padding:8px;"
        " outline:0;"
        " selection-background-color:%4;"
        " selection-color:%5;"
        " }"
        "QAbstractItemView::item {"
        " min-height:36px;"
        " padding:5px 12px;"
        " border-radius:10px;"
        " }"
        "QAbstractItemView::item:hover {"
        " background:%6;"
        " }"
        "QAbstractItemView::item:selected {"
        " background:%4;"
        " color:%5;"
        " }")
        .arg(surface.name(QColor::HexRgb))
        .arg(onSurface.name(QColor::HexRgb))
        .arg(outline.name(QColor::HexRgb))
        .arg(primaryContainer.name(QColor::HexRgb))
        .arg(onPrimaryContainer.name(QColor::HexRgb))
        .arg(surfaceHigh.name(QColor::HexRgb));

    const auto combos =
        root->findChildren<QtMaterial::QtMaterialComboBox*>();
    for (QtMaterial::QtMaterialComboBox* combo : combos) {
        if (combo->styleSheet() != comboStyle) {
            combo->setStyleSheet(comboStyle);
        }
        if (QAbstractItemView* view = combo->view()) {
            if (view->styleSheet() != popupStyle) {
                view->setStyleSheet(popupStyle);
            }
        }
    }

    const auto tables =
        root->findChildren<QtMaterial::QtMaterialTable*>();
    for (QtMaterial::QtMaterialTable* table : tables) {
        polishDemoTable(table);
    }
}

} // namespace DashboardDemoStyle
