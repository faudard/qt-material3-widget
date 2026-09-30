#include "dashboardwindow.h"

#include "dashboardcharts.h"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QApplication>
#include <QButtonGroup>
#include <QCalendarWidget>
#include <QComboBox>
#include <QDate>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLinearGradient>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QResizeEvent>
#include <QScrollArea>
#include <QShortcut>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QStackedWidget>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>

#include <algorithm>
#include <cmath>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/data/qtmaterialpagination.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialsearchbar.h"
#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"
#include "qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h"
#include "qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h"
#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"
#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"
#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"
#include "qtmaterial/widgets/surfaces/qtmaterialnavigationdrawer.h"
#include "qtmaterial/widgets/surfaces/qtmaterialsnackbarhost.h"

namespace {

QColor materialColor(QtMaterial::ColorRole role)
{
    return QtMaterial::ThemeManager::instance().theme().colorScheme().color(role);
}

QString cssColor(const QColor& color)
{
    return color.name(QColor::HexRgb);
}

QLabel* makeLabel(
    const QString& text,
    QWidget* parent,
    qreal pointDelta = 0.0,
    bool bold = false)
{
    auto* label = new QLabel(text, parent);
    QFont font = label->font();
    font.setPointSizeF(std::max<qreal>(8.0, font.pointSizeF() + pointDelta));
    font.setBold(bold);
    label->setFont(font);
    return label;
}

void paintDashboardGlyph(
    QPainter& painter,
    const QRectF& bounds,
    const QString& name,
    const QColor& color)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    const QRectF r = bounds.adjusted(3.0, 3.0, -3.0, -3.0);
    const QPointF c = r.center();

    if (name == QStringLiteral("menu")) {
        for (int i = 0; i < 3; ++i) {
            const qreal y = r.top() + 4.0 + i * 6.0;
            painter.drawLine(QPointF(r.left() + 1.0, y), QPointF(r.right() - 1.0, y));
        }
    } else if (name == QStringLiteral("dashboard")) {
        painter.setBrush(color);
        const qreal w = (r.width() - 4.0) / 2.0;
        const qreal h = (r.height() - 4.0) / 2.0;
        painter.drawRoundedRect(QRectF(r.left(), r.top(), w, h), 2.0, 2.0);
        painter.drawRoundedRect(QRectF(r.left() + w + 4.0, r.top(), w, h), 2.0, 2.0);
        painter.drawRoundedRect(QRectF(r.left(), r.top() + h + 4.0, w, h), 2.0, 2.0);
        painter.drawRoundedRect(QRectF(r.left() + w + 4.0, r.top() + h + 4.0, w, h), 2.0, 2.0);
    } else if (name == QStringLiteral("analytics")) {
        QPainterPath path;
        path.moveTo(r.left(), r.bottom() - 2.0);
        path.lineTo(r.left() + r.width() * 0.28, r.top() + r.height() * 0.58);
        path.lineTo(r.left() + r.width() * 0.53, r.top() + r.height() * 0.68);
        path.lineTo(r.right(), r.top() + 2.0);
        painter.drawPath(path);
        painter.drawLine(QPointF(r.left(), r.bottom()), QPointF(r.right(), r.bottom()));
    } else if (name == QStringLiteral("orders") || name == QStringLiteral("invoice")) {
        painter.drawRoundedRect(r.adjusted(2.0, 0.0, -2.0, 0.0), 2.0, 2.0);
        painter.drawLine(QPointF(r.left() + 6.0, r.top() + 6.0), QPointF(r.right() - 5.0, r.top() + 6.0));
        painter.drawLine(QPointF(r.left() + 6.0, r.top() + 11.0), QPointF(r.right() - 7.0, r.top() + 11.0));
        painter.drawLine(QPointF(r.left() + 6.0, r.top() + 16.0), QPointF(r.right() - 9.0, r.top() + 16.0));
    } else if (name == QStringLiteral("customers") || name == QStringLiteral("profile")) {
        painter.drawEllipse(QPointF(c.x(), r.top() + 5.0), 4.0, 4.0);
        QPainterPath shoulders;
        shoulders.moveTo(r.left() + 3.0, r.bottom() - 2.0);
        shoulders.cubicTo(
            r.left() + 5.0, r.bottom() - 9.0,
            r.right() - 5.0, r.bottom() - 9.0,
            r.right() - 3.0, r.bottom() - 2.0);
        painter.drawPath(shoulders);
    } else if (name == QStringLiteral("components")) {
        const qreal size = 6.0;
        painter.drawRoundedRect(QRectF(r.left() + 1.0, r.top() + 1.0, size, size), 1.3, 1.3);
        painter.drawRoundedRect(QRectF(r.right() - size - 1.0, r.top() + 1.0, size, size), 1.3, 1.3);
        painter.drawRoundedRect(QRectF(r.left() + 1.0, r.bottom() - size - 1.0, size, size), 1.3, 1.3);
        painter.drawRoundedRect(QRectF(r.right() - size - 1.0, r.bottom() - size - 1.0, size, size), 1.3, 1.3);
    } else if (name == QStringLiteral("input")) {
        painter.drawRoundedRect(r.adjusted(0.0, 4.0, 0.0, -4.0), 3.0, 3.0);
        painter.drawLine(QPointF(r.left() + 5.0, c.y()), QPointF(r.right() - 7.0, c.y()));
    } else if (name == QStringLiteral("navigation")) {
        for (int i = 0; i < 3; ++i) {
            const qreal y = r.top() + 4.0 + i * 6.0;
            painter.setBrush(color);
            painter.drawEllipse(QPointF(r.left() + 3.0, y), 1.5, 1.5);
            painter.setBrush(Qt::NoBrush);
            painter.drawLine(QPointF(r.left() + 7.0, y), QPointF(r.right(), y));
        }
    } else if (name == QStringLiteral("surfaces")) {
        painter.drawRoundedRect(r.adjusted(3.0, 1.0, -1.0, -5.0), 2.0, 2.0);
        painter.drawRoundedRect(r.adjusted(1.0, 5.0, -3.0, -1.0), 2.0, 2.0);
    } else if (name == QStringLiteral("data")) {
        painter.drawRoundedRect(r, 2.0, 2.0);
        painter.drawLine(QPointF(r.left(), r.top() + 6.0), QPointF(r.right(), r.top() + 6.0));
        painter.drawLine(QPointF(r.left() + 7.0, r.top()), QPointF(r.left() + 7.0, r.bottom()));
        painter.drawLine(QPointF(r.left() + 14.0, r.top()), QPointF(r.left() + 14.0, r.bottom()));
    } else if (name == QStringLiteral("projects")) {
        QPainterPath folder;
        folder.moveTo(r.left(), r.top() + 5.0);
        folder.lineTo(r.left() + 7.0, r.top() + 5.0);
        folder.lineTo(r.left() + 10.0, r.top() + 2.0);
        folder.lineTo(r.right(), r.top() + 2.0);
        folder.lineTo(r.right(), r.bottom());
        folder.lineTo(r.left(), r.bottom());
        folder.closeSubpath();
        painter.setBrush(color);
        painter.drawPath(folder);
    } else if (name == QStringLiteral("pricing")) {
        painter.drawEllipse(c, 8.0, 8.0);
        QFont font = painter.font();
        font.setBold(true);
        font.setPointSize(10);
        painter.setFont(font);
        painter.drawText(r, Qt::AlignCenter, QStringLiteral("$"));
    } else if (name == QStringLiteral("message")) {
        QPainterPath bubble;
        bubble.addRoundedRect(r.adjusted(1.0, 2.0, -1.0, -5.0), 3.0, 3.0);
        bubble.moveTo(r.left() + 7.0, r.bottom() - 3.0);
        bubble.lineTo(r.left() + 5.0, r.bottom());
        bubble.lineTo(r.left() + 11.0, r.bottom() - 4.0);
        painter.drawPath(bubble);
    } else if (name == QStringLiteral("bell")) {
        QPainterPath bell;
        bell.moveTo(r.left() + 5.0, r.bottom() - 5.0);
        bell.cubicTo(r.left() + 7.0, r.top() + 8.0, r.left() + 6.0, r.top() + 3.0, c.x(), r.top() + 3.0);
        bell.cubicTo(r.right() - 6.0, r.top() + 3.0, r.right() - 7.0, r.top() + 8.0, r.right() - 5.0, r.bottom() - 5.0);
        bell.closeSubpath();
        painter.drawPath(bell);
        painter.drawLine(QPointF(c.x() - 3.0, r.bottom() - 2.0), QPointF(c.x() + 3.0, r.bottom() - 2.0));
    } else if (name == QStringLiteral("sun")) {
        painter.drawEllipse(c, 4.0, 4.0);
        for (int i = 0; i < 8; ++i) {
            const qreal angle = i * 3.14159265358979323846 / 4.0;
            const QPointF a(c.x() + std::cos(angle) * 7.0, c.y() + std::sin(angle) * 7.0);
            const QPointF b(c.x() + std::cos(angle) * 10.0, c.y() + std::sin(angle) * 10.0);
            painter.drawLine(a, b);
        }
    } else if (name == QStringLiteral("moon")) {
        QPainterPath moon;
        moon.moveTo(r.right() - 3.0, r.top() + 3.0);
        moon.cubicTo(r.left() + 6.0, r.top() + 2.0, r.left() + 3.0, r.bottom() - 5.0, c.x(), r.bottom() - 2.0);
        moon.cubicTo(r.right() - 1.0, r.bottom() - 2.0, r.right() + 1.0, r.top() + 8.0, r.right() - 3.0, r.top() + 3.0);
        painter.drawPath(moon);
    } else if (name == QStringLiteral("chevron")) {
        painter.drawLine(QPointF(c.x() - 3.0, c.y() - 5.0), QPointF(c.x() + 2.0, c.y()));
        painter.drawLine(QPointF(c.x() + 2.0, c.y()), QPointF(c.x() - 3.0, c.y() + 5.0));
    } else {
        painter.drawEllipse(c, 7.0, 7.0);
    }

    painter.restore();
}

QIcon dashboardIcon(
    const QString& name,
    const QColor& color,
    int badge = -1)
{
    QPixmap pixmap(28, 28);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    paintDashboardGlyph(painter, QRectF(2.0, 2.0, 22.0, 22.0), name, color);

    if (badge >= 0) {
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(Qt::NoPen);
        painter.setBrush(materialColor(QtMaterial::ColorRole::Error));
        painter.drawEllipse(QRectF(17.0, 0.0, 11.0, 11.0));
        QFont badgeFont = painter.font();
        badgeFont.setBold(true);
        badgeFont.setPointSize(6);
        painter.setFont(badgeFont);
        painter.setPen(materialColor(QtMaterial::ColorRole::OnError));
        painter.drawText(QRectF(17.0, 0.0, 11.0, 11.0), Qt::AlignCenter, QString::number(badge));
    }

    return QIcon(pixmap);
}

QToolButton* makeNavButton(
    const QString& text,
    const QString& iconName,
    QWidget* parent)
{
    auto* button = new QToolButton(parent);
    button->setText(text);
    button->setObjectName(QStringLiteral("dashboardNavButton"));
    button->setProperty("dashboardIconName", iconName);
    button->setIcon(dashboardIcon(
        iconName,
        materialColor(QtMaterial::ColorRole::InverseOnSurface)));
    button->setIconSize(QSize(22, 22));
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setCheckable(true);
    button->setAutoExclusive(true);
    button->setCursor(Qt::PointingHandCursor);
    button->setMinimumHeight(34);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    return button;
}

class StatusBadgeDelegate final : public QStyledItemDelegate
{
public:
    explicit StatusBadgeDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    void paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override
    {
        QStyleOptionViewItem base(option);
        initStyleOption(&base, index);
        const QString text = base.text;
        base.text.clear();

        QStyle* style = option.widget ? option.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &base, painter, option.widget);

        QColor foreground = materialColor(QtMaterial::ColorRole::OnPrimaryContainer);
        QColor background = materialColor(QtMaterial::ColorRole::PrimaryContainer);
        if (text == QStringLiteral("Pending")) {
            foreground = materialColor(QtMaterial::ColorRole::OnSecondaryContainer);
            background = materialColor(QtMaterial::ColorRole::SecondaryContainer);
        } else if (text == QStringLiteral("Refunded") || text == QStringLiteral("At risk")) {
            foreground = materialColor(QtMaterial::ColorRole::OnErrorContainer);
            background = materialColor(QtMaterial::ColorRole::ErrorContainer);
        } else if (text == QStringLiteral("Trial")) {
            foreground = materialColor(QtMaterial::ColorRole::OnTertiaryContainer);
            background = materialColor(QtMaterial::ColorRole::TertiaryContainer);
        }

        QFont font = option.font;
        font.setBold(true);
        font.setPointSizeF(std::max<qreal>(8.0, font.pointSizeF() - 1.0));
        painter->setFont(font);
        const QFontMetrics metrics(font);
        const int width = metrics.horizontalAdvance(text) + 18;
        QRect pill(
            option.rect.left() + 7,
            option.rect.center().y() - 12,
            qMin(width, option.rect.width() - 14),
            24);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        painter->setBrush(background);
        painter->drawRoundedRect(pill, 12.0, 12.0);
        painter->setPen(foreground);
        painter->drawText(pill, Qt::AlignCenter, text);
        painter->restore();
    }
};

class SocialBarsWidget final : public QWidget
{
public:
    explicit SocialBarsWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setObjectName(QStringLiteral("dashboardSocialBars"));
        setMinimumHeight(150);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const QRectF plot = rect().adjusted(20.0, 16.0, -20.0, -28.0);
        if (plot.width() <= 0.0 || plot.height() <= 0.0) {
            return;
        }

        struct Bar {
            const char* label;
            int value;
            QtMaterial::ColorRole role;
        };
        static const Bar bars[] = {
            {"Search", 82, QtMaterial::ColorRole::Primary},
            {"Social", 42, QtMaterial::ColorRole::Secondary},
            {"Referral", 68, QtMaterial::ColorRole::Tertiary}
        };

        const qreal slot = plot.width() / 3.0;
        const qreal barWidth = qMin<qreal>(44.0, slot * 0.42);

        QFont percentFont = font();
        percentFont.setPointSizeF(std::max<qreal>(8.0, percentFont.pointSizeF() - 1.0));
        percentFont.setBold(true);
        QFont labelFont = percentFont;
        labelFont.setBold(false);

        for (int i = 0; i < 3; ++i) {
            const qreal centerX = plot.left() + slot * (i + 0.5);
            const qreal height = plot.height() * bars[i].value / 100.0;
            QRectF bar(
                centerX - barWidth / 2.0,
                plot.bottom() - height,
                barWidth,
                height);

            painter.setPen(Qt::NoPen);
            painter.setBrush(materialColor(bars[i].role));
            painter.drawRoundedRect(bar, 3.0, 3.0);

            painter.setFont(percentFont);
            painter.setPen(materialColor(QtMaterial::ColorRole::OnSurface));
            painter.drawText(
                QRectF(centerX - 34.0, bar.top() - 24.0, 68.0, 18.0),
                Qt::AlignCenter,
                QStringLiteral("+%1%").arg(bars[i].value >= 60 ? 30 : 20));

            painter.setFont(labelFont);
            painter.setPen(materialColor(QtMaterial::ColorRole::OnSurfaceVariant));
            painter.drawText(
                QRectF(centerX - 42.0, plot.bottom() + 7.0, 84.0, 18.0),
                Qt::AlignCenter,
                QString::fromLatin1(bars[i].label));
        }
    }
};

void clearGridPosition(QGridLayout* layout, QWidget* widget)
{
    if (layout && widget) {
        layout->removeWidget(widget);
    }
}

class RevenueSummaryWidget final : public QWidget
{
public:
    explicit RevenueSummaryWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setMinimumHeight(140);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QRectF box = rect();
        box.adjust(0.5, 0.5, -0.5, -0.5);

        const QColor primary = materialColor(QtMaterial::ColorRole::Primary);
        const QColor onPrimary = materialColor(QtMaterial::ColorRole::OnPrimary);
        QLinearGradient background(box.topLeft(), box.bottomRight());
        background.setColorAt(0.0, primary.lighter(108));
        background.setColorAt(1.0, primary.darker(118));
        painter.setPen(Qt::NoPen);
        painter.setBrush(background);
        painter.drawRoundedRect(box, 10.0, 10.0);

        painter.setPen(onPrimary);
        QFont valueFont = font();
        valueFont.setPointSizeF(valueFont.pointSizeF() + 8.0);
        valueFont.setBold(true);
        painter.setFont(valueFont);
        painter.drawText(QRectF(20, 16, width() - 40, 34), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("€216,759"));

        QFont labelFont = font();
        labelFont.setPointSizeF(std::max<qreal>(8.0, labelFont.pointSizeF() - 1.0));
        painter.setFont(labelFont);
        QColor mutedOnPrimary = onPrimary;
        mutedOnPrimary.setAlpha(175);
        painter.setPen(mutedOnPrimary);
        painter.drawText(QRectF(20, 52, 110, 22), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("YTD Revenue"));

        painter.setPen(onPrimary);
        QFont smallValue = font();
        smallValue.setPointSizeF(smallValue.pointSizeF() + 3.0);
        smallValue.setBold(true);
        painter.setFont(smallValue);
        painter.drawText(QRectF(20, 91, 44, 26), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("49"));
        painter.drawText(QRectF(74, 91, 44, 26), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("09"));

        mutedOnPrimary.setAlpha(150);
        painter.setPen(mutedOnPrimary);
        painter.setFont(labelFont);
        painter.drawText(QRectF(20, 114, 44, 18), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Clients"));
        painter.drawText(QRectF(74, 114, 60, 18), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Countries"));

        if (width() < 210) {
            return;
        }

        const QRectF chart(width() * 0.43, 58, width() * 0.52, 72);
        const QVector<qreal> values = {0.12, 0.28, 0.21, 0.48, 0.40, 0.72, 0.55, 0.84, 0.70};
        QPainterPath path;
        for (int i = 0; i < values.size(); ++i) {
            const qreal x = chart.left() + chart.width() * i / (values.size() - 1.0);
            const qreal y = chart.bottom() - chart.height() * values.at(i);
            if (i == 0) {
                path.moveTo(x, y);
            } else {
                path.lineTo(x, y);
            }
        }
        painter.setPen(QPen(
            materialColor(QtMaterial::ColorRole::TertiaryFixed),
            3.0,
            Qt::SolidLine,
            Qt::RoundCap,
            Qt::RoundJoin));
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }
};

QFrame* makeSectionLabel(const QString& text, QWidget* parent)
{
    auto* host = new QFrame(parent);
    auto* layout = new QHBoxLayout(host);
    layout->setContentsMargins(16, 9, 16, 3);
    auto* label = new QLabel(text.toUpper(), host);
    QFont font = label->font();
    font.setPointSizeF(std::max<qreal>(8.0, font.pointSizeF() - 1.0));
    font.setLetterSpacing(QFont::AbsoluteSpacing, 0.6);
    label->setFont(font);
    label->setObjectName(QStringLiteral("dashboardSectionLabel"));
    layout->addWidget(label);
    return host;
}

QWidget* makePageShell(
    const QString& title,
    const QString& subtitle,
    QVBoxLayout** outLayout)
{
    auto* page = new QWidget;
    page->setObjectName(QStringLiteral("dashboardContent"));

    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 20, 24, 30);
    layout->setSpacing(16);

    auto* titleBlock = new QVBoxLayout;
    titleBlock->setSpacing(2);
    titleBlock->addWidget(makeLabel(title, page, 5.0, true));
    auto* subtitleLabel = makeLabel(subtitle, page, -1.0, false);
    subtitleLabel->setObjectName(QStringLiteral("pageSubtitle"));
    titleBlock->addWidget(subtitleLabel);
    layout->addLayout(titleBlock);

    if (outLayout) {
        *outLayout = layout;
    }
    return page;
}

} // namespace

DashboardWindow::DashboardWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Qt Material 3 - Dashboard Showcase"));
    setMinimumSize(640, 560);

    auto options = QtMaterial::ThemeManager::instance().options();
    options.sourceColor = QColor(QStringLiteral("#4455c7"));
    QtMaterial::ThemeManager::instance().setThemeOptions(options);

    m_central = new QWidget(this);
    auto* shell = new QHBoxLayout(m_central);
    shell->setContentsMargins(0, 0, 0, 0);
    shell->setSpacing(0);

    m_sidebar = createSidebar();
    shell->addWidget(m_sidebar);

    m_navigationRail = createNavigationRail();
    shell->addWidget(m_navigationRail);

    auto* right = new QWidget(m_central);
    auto* rightLayout = new QVBoxLayout(right);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    rightLayout->addWidget(createTopBar());

    m_pages = new QStackedWidget(right);

    auto createPageScroll = [right](QWidget* page) {
        auto* scroll = new QScrollArea(right);
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scroll->setWidget(page);
        return scroll;
    };

    m_contentHost = createDashboardPage();
    m_scroll = createPageScroll(m_contentHost);
    m_pages->addWidget(m_scroll);
    m_pages->addWidget(createPageScroll(createAnalyticsPage()));
    m_pages->addWidget(createPageScroll(createOrdersPage()));
    m_pages->addWidget(createPageScroll(createCustomersPage()));
    m_pages->addWidget(createPageScroll(createComponentsPage()));
    m_pages->addWidget(createPageScroll(createProfilePage()));
    m_pages->addWidget(createPageScroll(createPricingPage()));
    m_pages->addWidget(createPageScroll(createApplicationStatesPage()));
    m_pages->addWidget(createPageScroll(createShowcaseSettingsPage()));
    rightLayout->addWidget(m_pages, 1);

    shell->addWidget(right, 1);
    setCentralWidget(m_central);

    m_navigationDrawer = createNavigationDrawer();

    m_snackbarHost = new QtMaterial::QtMaterialSnackbarHost(m_central, this);
    m_commandPalette = new QtMaterial::QtMaterialCommandPalette(this);
    populateCommandPalette();

    auto* commandShortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+K")), this);
    connect(commandShortcut, &QShortcut::activated, m_commandPalette, &QDialog::open);

    connect(
        m_commandPalette,
        &QtMaterial::QtMaterialCommandPalette::commandActivated,
        this,
        [this](const QModelIndex& index) {
            if (index.isValid()) {
                setCurrentSection(index.row());
            }
        });

    connect(
        &QtMaterial::ThemeManager::instance(),
        &QtMaterial::ThemeManager::themeChanged,
        this,
        [this](const QtMaterial::Theme&) {
            applyThemeChrome();
            applyPeriod();
            if (m_themeButton) {
                m_themeButton->setIcon(dashboardIcon(
                    QtMaterial::ThemeManager::instance().theme().isDark()
                        ? QStringLiteral("sun")
                        : QStringLiteral("moon"),
                    materialColor(QtMaterial::ColorRole::OnSurfaceVariant)));
            }
        });

    applyThemeChrome();
    applyPeriod();
    updateResponsiveLayout();
}

DashboardWindow::~DashboardWindow() = default;

void DashboardWindow::showDemoPage(int index)
{
    setCurrentSection(index);
}

void DashboardWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    updateResponsiveLayout();
}

QWidget* DashboardWindow::createSidebar()
{
    auto* sidebar = new QWidget(m_central);
    sidebar->setObjectName(QStringLiteral("dashboardSidebar"));
    sidebar->setFixedWidth(226);

    auto* shell = new QVBoxLayout(sidebar);
    shell->setContentsMargins(0, 0, 0, 0);
    shell->setSpacing(0);

    auto* brand = new QWidget(sidebar);
    brand->setFixedHeight(70);
    auto* brandLayout = new QHBoxLayout(brand);
    brandLayout->setContentsMargins(16, 12, 12, 10);
    brandLayout->setSpacing(11);

    auto* logo = new QLabel(QStringLiteral("M3"), brand);
    logo->setAlignment(Qt::AlignCenter);
    logo->setFixedSize(36, 36);
    logo->setObjectName(QStringLiteral("dashboardBrandLogo"));

    auto* brandText = new QVBoxLayout;
    brandText->setSpacing(0);
    auto* brandTitle = makeLabel(QStringLiteral("Material 3"), brand, 1.0, true);
    auto* brandSubtitle = makeLabel(QStringLiteral("Qt Widgets"), brand, -1.0, false);
    brandTitle->setObjectName(QStringLiteral("dashboardBrandTitle"));
    brandSubtitle->setObjectName(QStringLiteral("dashboardBrandSubtitle"));
    brandText->addWidget(brandTitle);
    brandText->addWidget(brandSubtitle);

    brandLayout->addWidget(logo);
    brandLayout->addLayout(brandText, 1);
    shell->addWidget(brand);

    auto* navigationScroll = new QScrollArea(sidebar);
    navigationScroll->setFrameShape(QFrame::NoFrame);
    navigationScroll->setWidgetResizable(true);
    navigationScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    navigationScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    navigationScroll->setObjectName(QStringLiteral("dashboardNavScroll"));

    auto* navigationContent = new QWidget(navigationScroll);
    navigationContent->setObjectName(QStringLiteral("dashboardNavContent"));
    auto* navigationLayout = new QVBoxLayout(navigationContent);
    navigationLayout->setContentsMargins(0, 0, 0, 8);
    navigationLayout->setSpacing(1);

    auto* group = new QButtonGroup(navigationContent);
    group->setExclusive(true);

    const auto addPrimaryNavigation =
        [this, navigationContent, navigationLayout, group](
            const QString& label,
            const QString& iconName,
            int index) {
            auto* button = makeNavButton(label, iconName, navigationContent);
            group->addButton(button, index);
            m_navButtons.append(button);
            navigationLayout->addWidget(button);
            connect(button, &QToolButton::clicked, this, [this, index]() {
                setCurrentSection(index);
            });
            return button;
        };

    const auto addShortcut =
        [this, navigationContent, navigationLayout](
            const QString& label,
            const QString& iconName,
            int targetIndex) {
            auto* button = makeNavButton(label, iconName, navigationContent);
            button->setCheckable(false);
            navigationLayout->addWidget(button);
            connect(button, &QToolButton::clicked, this, [this, targetIndex]() {
                setCurrentSection(targetIndex);
            });
            return button;
        };

    const auto addInformational =
        [this, navigationContent, navigationLayout](
            const QString& label,
            const QString& iconName) {
            auto* button = makeNavButton(label, iconName, navigationContent);
            button->setCheckable(false);
            navigationLayout->addWidget(button);
            connect(button, &QToolButton::clicked, this, [this, label]() {
                showMessage(QStringLiteral("%1 is represented as navigation content in this showcase.").arg(label));
            });
            return button;
        };

    navigationLayout->addWidget(makeSectionLabel(QStringLiteral("Application"), navigationContent));
    addPrimaryNavigation(QStringLiteral("Dashboard"), QStringLiteral("dashboard"), 0);
    addPrimaryNavigation(QStringLiteral("Analytics"), QStringLiteral("analytics"), 1);
    addPrimaryNavigation(QStringLiteral("Orders"), QStringLiteral("orders"), 2);
    addPrimaryNavigation(QStringLiteral("Customers"), QStringLiteral("customers"), 3);
    addShortcut(QStringLiteral("Metrics"), QStringLiteral("analytics"), 1);
    addShortcut(QStringLiteral("Widgets"), QStringLiteral("components"), 4);
    addShortcut(QStringLiteral("Apps"), QStringLiteral("components"), 4);
    addShortcut(QStringLiteral("Ecommerce"), QStringLiteral("orders"), 2);
    m_navButtons.first()->setChecked(true);

    navigationLayout->addWidget(makeSectionLabel(QStringLiteral("MUI Components"), navigationContent));
    addPrimaryNavigation(QStringLiteral("Components"), QStringLiteral("components"), 4);
    addShortcut(QStringLiteral("   Inputs"), QStringLiteral("input"), 4);
    addShortcut(QStringLiteral("   Navigations"), QStringLiteral("navigation"), 4);
    addShortcut(QStringLiteral("   Surfaces"), QStringLiteral("surfaces"), 4);
    addShortcut(QStringLiteral("   Feedback"), QStringLiteral("message"), 4);
    addShortcut(QStringLiteral("   Data Display"), QStringLiteral("data"), 4);
    addShortcut(QStringLiteral("   Util"), QStringLiteral("components"), 4);

    navigationLayout->addWidget(makeSectionLabel(QStringLiteral("Pages"), navigationContent));
    addInformational(QStringLiteral("Authentication"), QStringLiteral("profile"));
    addInformational(QStringLiteral("Coming Soon"), QStringLiteral("analytics"));
    addInformational(QStringLiteral("Errors"), QStringLiteral("message"));
    addShortcut(QStringLiteral("Invoice"), QStringLiteral("invoice"), 2);
    addInformational(QStringLiteral("Maintenance"), QStringLiteral("components"));
    addPrimaryNavigation(QStringLiteral("Profile"), QStringLiteral("profile"), 5);
    addPrimaryNavigation(QStringLiteral("Pricing"), QStringLiteral("pricing"), 6);

    navigationLayout->addWidget(makeSectionLabel(QStringLiteral("System"), navigationContent));
    addPrimaryNavigation(QStringLiteral("Application States"), QStringLiteral("message"), 7);
    auto* themeStudio = makeNavButton(
        QStringLiteral("Theme Studio"),
        QStringLiteral("components"),
        navigationContent);
    themeStudio->setCheckable(false);
    navigationLayout->addWidget(themeStudio);
    connect(themeStudio, &QToolButton::clicked, this, [this]() {
        showMessage(QStringLiteral("Theme Studio is available as a separate example."));
    });

    addPrimaryNavigation(
        QStringLiteral("Showcase Settings"),
        QStringLiteral("components"),
        8);

    navigationLayout->addStretch(1);
    navigationScroll->setWidget(navigationContent);
    shell->addWidget(navigationScroll, 1);

    auto* profile = new QFrame(sidebar);
    profile->setObjectName(QStringLiteral("dashboardProfile"));
    auto* profileLayout = new QHBoxLayout(profile);
    profileLayout->setContentsMargins(14, 11, 14, 11);
    auto* avatar = new QLabel(QStringLiteral("Q"), profile);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setFixedSize(32, 32);
    avatar->setObjectName(QStringLiteral("dashboardProfileAvatar"));
    auto* user = new QVBoxLayout;
    user->setSpacing(0);
    auto* name = makeLabel(QStringLiteral("QtMaterial Demo"), profile, -1.0, true);
    auto* role = makeLabel(QStringLiteral("Desktop showcase"), profile, -2.0, false);
    name->setObjectName(QStringLiteral("dashboardProfileName"));
    role->setObjectName(QStringLiteral("dashboardProfileRole"));
    user->addWidget(name);
    user->addWidget(role);
    profileLayout->addWidget(avatar);
    profileLayout->addLayout(user, 1);
    shell->addWidget(profile);

    return sidebar;
}

QtMaterial::QtMaterialNavigationRail* DashboardWindow::createNavigationRail()
{
    auto* rail = new QtMaterial::QtMaterialNavigationRail(m_central);
    rail->setLabelsVisible(false);

    const struct {
        const char* label;
        const char* icon;
    } destinations[] = {
        {"Dashboard", "dashboard"},
        {"Analytics", "analytics"},
        {"Orders", "orders"},
        {"Customers", "customers"},
        {"Components", "components"},
        {"Profile", "profile"},
        {"Pricing", "pricing"},
        {"States", "message"},
        {"Settings", "components"}
    };

    for (const auto& destination : destinations) {
        rail->addDestination(
            QString::fromLatin1(destination.label),
            dashboardIcon(
                QString::fromLatin1(destination.icon),
                materialColor(QtMaterial::ColorRole::OnSurfaceVariant)));
    }
    rail->setCurrentIndex(0);

    connect(
        rail,
        &QtMaterial::QtMaterialNavigationRail::currentIndexChanged,
        this,
        [this](int index) {
            setCurrentSection(index);
        });

    return rail;
}

QtMaterial::QtMaterialNavigationDrawer* DashboardWindow::createNavigationDrawer()
{
    auto* drawer = new QtMaterial::QtMaterialNavigationDrawer(m_central);
    drawer->setObjectName(QStringLiteral("dashboardNavigationDrawer"));
    drawer->setHostWidget(m_central);
    drawer->setEdge(QtMaterial::QtMaterialNavigationDrawer::Edge::Left);

    auto* layout = new QVBoxLayout(drawer);
    layout->setContentsMargins(22, 24, 22, 24);
    layout->setSpacing(8);

    auto* header = new QHBoxLayout;
    header->addWidget(makeLabel(QStringLiteral("Navigation"), drawer, 3.0, true));
    header->addStretch(1);

    auto* close = new QtMaterial::QtMaterialIconButton(
        dashboardIcon(
            QStringLiteral("chevron"),
            materialColor(QtMaterial::ColorRole::OnSurfaceVariant)),
        drawer);
    close->setAccessibleName(QStringLiteral("Close navigation"));
    close->setRequiresAccessibleName(true);
    close->setToolTip(QStringLiteral("Close navigation"));
    header->addWidget(close);
    layout->addLayout(header);

    auto* group = new QButtonGroup(drawer);
    group->setExclusive(true);

    const struct {
        const char* label;
        const char* icon;
    } destinations[] = {
        {"Dashboard", "dashboard"},
        {"Analytics", "analytics"},
        {"Orders", "orders"},
        {"Customers", "customers"},
        {"Components", "components"},
        {"Profile", "profile"},
        {"Pricing", "pricing"},
        {"States", "message"},
        {"Settings", "components"}
    };

    for (int i = 0; i < 9; ++i) {
        auto* button = makeNavButton(
            QString::fromLatin1(destinations[i].label),
            QString::fromLatin1(destinations[i].icon),
            drawer);
        button->setProperty("dashboardDrawerIndex", i);
        group->addButton(button, i);
        layout->addWidget(button);
        connect(button, &QToolButton::clicked, this, [this, i]() {
            setCurrentSection(i);
            if (m_navigationDrawer) {
                m_navigationDrawer->closeDrawer();
            }
        });
    }

    layout->addStretch(1);

    connect(close, &QAbstractButton::clicked, drawer, &QtMaterial::QtMaterialNavigationDrawer::closeDrawer);
    return drawer;
}

QWidget* DashboardWindow::createTopBar()
{
    m_topBar = new QFrame(m_central);
    m_topBar->setObjectName(QStringLiteral("dashboardTopBar"));
    m_topBar->setFixedHeight(62);

    auto* layout = new QHBoxLayout(m_topBar);
    layout->setContentsMargins(20, 8, 16, 8);
    layout->setSpacing(10);

    m_menuButton = new QtMaterial::QtMaterialIconButton(
        dashboardIcon(
            QStringLiteral("menu"),
            materialColor(QtMaterial::ColorRole::OnSurfaceVariant)),
        m_topBar);
    m_menuButton->setAccessibleName(QStringLiteral("Open navigation"));
    m_menuButton->setRequiresAccessibleName(true);
    m_menuButton->setToolTip(QStringLiteral("Open navigation"));
    m_menuButton->setProperty("dashboardIconName", QStringLiteral("menu"));
    layout->addWidget(m_menuButton);

    m_breadcrumb = new QtMaterial::QtMaterialBreadcrumb(m_topBar);
    m_breadcrumb->setItems({
        QStringLiteral("Application"),
        QStringLiteral("Dashboard")
    });
    m_breadcrumb->setCurrentIndex(1);
    layout->addWidget(m_breadcrumb);
    layout->addStretch(1);

    m_search = new QtMaterial::QtMaterialSearchBar(m_topBar);
    m_search->setPlaceholderText(QStringLiteral("Search..."));
    m_search->setClearButtonVisible(true);
    m_search->setFixedWidth(240);
    m_search->setFixedHeight(40);
    layout->addWidget(m_search);

    auto* language = new QToolButton(m_topBar);
    language->setText(QStringLiteral("EN"));
    language->setCursor(Qt::PointingHandCursor);
    language->setObjectName(QStringLiteral("topBarButton"));
    language->setProperty("dashboardCompactOptional", true);
    language->setFixedSize(42, 36);
    layout->addWidget(language);

    auto* messages = new QtMaterial::QtMaterialIconButton(
        dashboardIcon(
            QStringLiteral("message"),
            materialColor(QtMaterial::ColorRole::OnSurfaceVariant),
            4),
        m_topBar);
    messages->setAccessibleName(QStringLiteral("Messages"));
    messages->setRequiresAccessibleName(true);
    messages->setToolTip(QStringLiteral("Messages"));
    messages->setCursor(Qt::PointingHandCursor);
    messages->setProperty("dashboardIconName", QStringLiteral("message"));
    messages->setProperty("dashboardBadge", 4);
    layout->addWidget(messages);

    auto* notify = new QtMaterial::QtMaterialIconButton(
        dashboardIcon(
            QStringLiteral("bell"),
            materialColor(QtMaterial::ColorRole::OnSurfaceVariant),
            8),
        m_topBar);
    notify->setAccessibleName(QStringLiteral("Notifications"));
    notify->setRequiresAccessibleName(true);
    notify->setToolTip(QStringLiteral("Notifications"));
    notify->setCursor(Qt::PointingHandCursor);
    notify->setProperty("dashboardIconName", QStringLiteral("bell"));
    notify->setProperty("dashboardBadge", 8);
    layout->addWidget(notify);

    m_themeButton = new QtMaterial::QtMaterialIconButton(
        dashboardIcon(
            QtMaterial::ThemeManager::instance().theme().isDark()
                ? QStringLiteral("sun")
                : QStringLiteral("moon"),
            materialColor(QtMaterial::ColorRole::OnSurfaceVariant)),
        m_topBar);
    m_themeButton->setAccessibleName(QStringLiteral("Toggle light/dark mode"));
    m_themeButton->setRequiresAccessibleName(true);
    m_themeButton->setToolTip(QStringLiteral("Toggle light/dark mode"));
    m_themeButton->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_themeButton);

    auto* avatar = new QLabel(QStringLiteral("JD"), m_topBar);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setFixedSize(32, 32);
    avatar->setObjectName(QStringLiteral("dashboardTopAvatar"));
    avatar->setProperty("dashboardCompactOptional", true);
    layout->addWidget(avatar);

    auto* account = new QToolButton(m_topBar);
    account->setText(QStringLiteral("John Doe"));
    account->setIcon(dashboardIcon(
        QStringLiteral("chevron"),
        materialColor(QtMaterial::ColorRole::OnSurfaceVariant)));
    account->setIconSize(QSize(16, 16));
    account->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    account->setCursor(Qt::PointingHandCursor);
    account->setObjectName(QStringLiteral("topBarAccount"));
    account->setProperty("dashboardCompactOptional", true);
    account->setMinimumWidth(94);
    account->setFixedHeight(36);
    layout->addWidget(account);

    connect(m_menuButton, &QAbstractButton::clicked, this, [this]() {
        if (m_navigationDrawer) {
            m_navigationDrawer->open();
        }
    });
    connect(m_search, &QtMaterial::QtMaterialSearchBar::textChanged, this, &DashboardWindow::applyFilter);
    connect(m_themeButton, &QAbstractButton::clicked, this, []() {
        auto options = QtMaterial::ThemeManager::instance().options();
        options.mode = options.mode == QtMaterial::ThemeMode::Dark
            ? QtMaterial::ThemeMode::Light
            : QtMaterial::ThemeMode::Dark;
        QtMaterial::ThemeManager::instance().setThemeOptions(options);
    });
    connect(messages, &QAbstractButton::clicked, this, [this]() {
        showMessage(QStringLiteral("4 unread messages."));
    });
    connect(notify, &QAbstractButton::clicked, this, [this]() {
        showMessage(QStringLiteral("8 notifications waiting for review."));
    });

    return m_topBar;
}

QWidget* DashboardWindow::createDashboardPage()
{
    auto* page = new QWidget;
    page->setObjectName(QStringLiteral("dashboardContent"));
    m_contentHost = page;

    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 18, 24, 30);
    layout->setSpacing(16);

    auto* headingHost = new QWidget(page);
    auto* headingGrid = new QGridLayout(headingHost);
    headingGrid->setContentsMargins(0, 0, 0, 0);
    headingGrid->setHorizontalSpacing(18);
    auto* quickTitle = makeLabel(QStringLiteral("Quick Statistics"), headingHost, 2.0, false);
    auto* revenueTitle = makeLabel(QStringLiteral("Total Revenue"), headingHost, 2.0, false);
    headingGrid->addWidget(quickTitle, 0, 0, 1, 2);
    headingGrid->addWidget(revenueTitle, 0, 2);
    headingGrid->setColumnStretch(0, 1);
    headingGrid->setColumnStretch(1, 1);
    headingGrid->setColumnStretch(2, 1);
    layout->addWidget(headingHost);

    layout->addWidget(createQuickStatistics());

    auto* charts = new QWidget(page);
    m_chartGrid = new QGridLayout(charts);
    m_chartGrid->setContentsMargins(0, 0, 0, 0);
    m_chartGrid->setHorizontalSpacing(18);
    m_chartGrid->setVerticalSpacing(18);
    m_statisticsCard = createStatisticsCard();
    m_earningsCard = createEarningsCard();
    layout->addWidget(charts);

    layout->addWidget(createLowerHighlights());
    layout->addWidget(createOrdersCard());
    layout->addStretch(1);

    connect(m_yearCombo, &QComboBox::currentTextChanged, this, [this](const QString&) { applyPeriod(); });
    connect(m_monthCombo, &QComboBox::currentTextChanged, this, [this](const QString&) { applyPeriod(); });

    return page;
}


QWidget* DashboardWindow::createAnalyticsPage()
{
    QVBoxLayout* layout = nullptr;
    auto* page = makePageShell(
        QStringLiteral("Analytics"),
        QStringLiteral("Monitor acquisition, engagement and conversion across the product."),
        &layout);

    auto* metricsHost = new QWidget(page);
    auto* metrics = new QGridLayout(metricsHost);
    metrics->setContentsMargins(0, 0, 0, 0);
    metrics->setHorizontalSpacing(14);
    metrics->setVerticalSpacing(14);

    const auto addStat = [page, metrics](
        int column,
        const QString& title,
        const QString& value,
        const QString& note) {
        auto* card = new QtMaterial::QtMaterialCard(page);
        card->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
        card->setMinimumHeight(116);
        auto* cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(16, 14, 16, 14);
        cardLayout->setSpacing(3);
        auto* titleLabel = makeLabel(title, card, -1.0, false);
        titleLabel->setObjectName(QStringLiteral("metricTitle"));
        cardLayout->addWidget(titleLabel);
        cardLayout->addWidget(makeLabel(value, card, 6.0, true));
        auto* noteLabel = makeLabel(note, card, -2.0, false);
        noteLabel->setObjectName(QStringLiteral("positiveDelta"));
        cardLayout->addWidget(noteLabel);
        metrics->addWidget(card, 0, column);
    };

    addStat(0, QStringLiteral("Sessions"), QStringLiteral("24.8K"), QStringLiteral("+18.2% this month"));
    addStat(1, QStringLiteral("Page views"), QStringLiteral("87.4K"), QStringLiteral("+11.6% this month"));
    addStat(2, QStringLiteral("Conversion"), QStringLiteral("4.82%"), QStringLiteral("+0.7 pts"));
    addStat(3, QStringLiteral("Avg. session"), QStringLiteral("4m 12s"), QStringLiteral("+24 sec"));
    layout->addWidget(metricsHost);

    auto* analyticsGridHost = new QWidget(page);
    auto* analyticsGrid = new QGridLayout(analyticsGridHost);
    analyticsGrid->setContentsMargins(0, 0, 0, 0);
    analyticsGrid->setHorizontalSpacing(18);
    analyticsGrid->setVerticalSpacing(18);

    auto* trendCard = new QtMaterial::QtMaterialCard(page);
    trendCard->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    trendCard->setMinimumHeight(350);
    auto* trendLayout = new QVBoxLayout(trendCard);
    trendLayout->setContentsMargins(18, 16, 18, 14);
    trendLayout->addWidget(makeLabel(QStringLiteral("Audience trend"), trendCard, 2.0, false));
    trendLayout->addWidget(makeLabel(
        QStringLiteral("Unique visitors by month"),
        trendCard,
        -1.0,
        false));
    auto* analyticsChart = new LineChartWidget(trendCard);
    analyticsChart->clearAccentColor();
    analyticsChart->setValues({56, 72, 68, 84, 91, 88, 106, 119, 112, 136, 128, 149});
    trendLayout->addWidget(analyticsChart, 1);
    analyticsGrid->addWidget(trendCard, 0, 0, 1, 2);

    auto* channelsCard = new QtMaterial::QtMaterialCard(page);
    channelsCard->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    channelsCard->setMinimumHeight(350);
    auto* channelsLayout = new QVBoxLayout(channelsCard);
    channelsLayout->setContentsMargins(18, 16, 18, 16);
    channelsLayout->addWidget(makeLabel(QStringLiteral("Acquisition channels"), channelsCard, 2.0, false));

    const struct {
        const char* label;
        int value;
        const char* percent;
    } channels[] = {
        {"Organic search", 84, "42%"},
        {"Direct", 62, "31%"},
        {"Social", 36, "18%"},
        {"Referral", 18, "9%"}
    };

    for (const auto& channel : channels) {
        auto* row = new QHBoxLayout;
        row->addWidget(makeLabel(QString::fromLatin1(channel.label), channelsCard, -1.0, false));
        auto* progress = new QtMaterial::QtMaterialLinearProgressIndicator(channelsCard);
        progress->setValue(static_cast<qreal>(channel.value) / 100.0);
        progress->setStatusText(
            QStringLiteral("%1: %2")
                .arg(QString::fromLatin1(channel.label))
                .arg(QString::fromLatin1(channel.percent)));
        row->addWidget(progress, 1);
        row->addWidget(makeLabel(QString::fromLatin1(channel.percent), channelsCard, -1.0, true));
        channelsLayout->addLayout(row);
    }
    channelsLayout->addStretch(1);
    analyticsGrid->addWidget(channelsCard, 0, 2);
    analyticsGrid->setColumnStretch(0, 1);
    analyticsGrid->setColumnStretch(1, 1);
    analyticsGrid->setColumnStretch(2, 1);
    layout->addWidget(analyticsGridHost);
    layout->addStretch(1);
    return page;
}

QWidget* DashboardWindow::createOrdersPage()
{
    QVBoxLayout* layout = nullptr;
    auto* page = makePageShell(
        QStringLiteral("Orders"),
        QStringLiteral("Manage recent purchases, payment status and fulfilment."),
        &layout);

    auto* filters = new QHBoxLayout;
    auto* statusGroup = new QButtonGroup(page);
    statusGroup->setExclusive(true);

    const struct {
        const char* label;
        const char* status;
    } filterItems[] = {
        {"All  30", "All"},
        {"Paid  16", "Paid"},
        {"Pending  8", "Pending"},
        {"Refunded  6", "Refunded"}
    };

    for (int i = 0; i < 4; ++i) {
        auto* chip = new QtMaterial::QtMaterialChip(
            QString::fromLatin1(filterItems[i].label),
            page);
        chip->setVariant(QtMaterial::ChipVariant::Assist);
        chip->setCheckable(true);
        chip->setChecked(i == 0);
        chip->setProperty(
            "dashboardOrderStatus",
            QString::fromLatin1(filterItems[i].status));
        statusGroup->addButton(chip, i);
        filters->addWidget(chip);

        connect(chip, &QAbstractButton::clicked, this, [this, chip]() {
            m_orderStatusFilter =
                chip->property("dashboardOrderStatus").toString();
            if (m_ordersPagination) {
                m_ordersPagination->setPage(1);
            }
            applyFilter(m_search ? m_search->text() : QString());
        });
    }
    filters->addStretch(1);
    layout->addLayout(filters);

    auto* tableCard = new QtMaterial::QtMaterialCard(page);
    tableCard->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    tableCard->setMinimumHeight(480);
    auto* cardLayout = new QVBoxLayout(tableCard);
    cardLayout->setContentsMargins(18, 16, 18, 18);

    auto* header = new QHBoxLayout;
    header->addWidget(makeLabel(QStringLiteral("All orders"), tableCard, 2.0, false));
    header->addStretch(1);
    auto* exportButton = new QtMaterial::QtMaterialFilledTonalButton(
        QStringLiteral("Export CSV"),
        tableCard);
    header->addWidget(exportButton);
    cardLayout->addLayout(header);

    m_ordersPage = new QtMaterial::QtMaterialTable(tableCard);
    m_ordersPage->setDense(false);
    m_ordersPage->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ordersPage->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ordersPage->verticalHeader()->setVisible(false);
    m_ordersPage->setSortingEnabled(true);

    m_ordersPageModel = new QStandardItemModel(30, 5, m_ordersPage);
    m_ordersPageModel->setHorizontalHeaderLabels({
        QStringLiteral("Order"),
        QStringLiteral("Customer"),
        QStringLiteral("Product"),
        QStringLiteral("Amount"),
        QStringLiteral("Status")
    });

    const QStringList customers = {
        QStringLiteral("Alice Martin"),
        QStringLiteral("John Smith"),
        QStringLiteral("Emma Dupont"),
        QStringLiteral("Noah Bernard"),
        QStringLiteral("Lina Robert"),
        QStringLiteral("Lucas Petit"),
        QStringLiteral("Mia Leroy"),
        QStringLiteral("Leo Garcia")
    };
    const QStringList products = {
        QStringLiteral("Design system"),
        QStringLiteral("Widget pack"),
        QStringLiteral("Enterprise license"),
        QStringLiteral("Theme pack"),
        QStringLiteral("Support plan"),
        QStringLiteral("Component pack")
    };

    for (int row = 0; row < 30; ++row) {
        const QString status =
            row % 5 == 3
                ? QStringLiteral("Refunded")
                : (row % 3 == 1
                    ? QStringLiteral("Pending")
                    : QStringLiteral("Paid"));
        const int amount = 180 + ((row * 137) % 1420);

        m_ordersPageModel->setItem(
            row,
            0,
            new QStandardItem(QStringLiteral("#%1").arg(1042 - row)));
        m_ordersPageModel->setItem(
            row,
            1,
            new QStandardItem(customers.at(row % customers.size())));
        m_ordersPageModel->setItem(
            row,
            2,
            new QStandardItem(products.at(row % products.size())));
        m_ordersPageModel->setItem(
            row,
            3,
            new QStandardItem(QStringLiteral("€%1").arg(amount)));
        m_ordersPageModel->setItem(
            row,
            4,
            new QStandardItem(status));
    }

    m_ordersPage->setModel(m_ordersPageModel);
    m_ordersPage->setItemDelegateForColumn(4, new StatusBadgeDelegate(m_ordersPage));
    m_ordersPage->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_ordersPage->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_ordersPage->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_ordersPage->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_ordersPage->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    cardLayout->addWidget(m_ordersPage, 1);

    m_ordersPagination = new QtMaterial::QtMaterialPagination(tableCard);
    m_ordersPagination->setPageSizeOptions({10, 25, 50});
    m_ordersPagination->setPageSize(10);
    m_ordersPagination->setTotalCount(m_ordersPageModel->rowCount());
    cardLayout->addWidget(m_ordersPagination);

    layout->addWidget(tableCard);

    const auto refreshOrdersPage = [this]() {
        applyFilter(m_search ? m_search->text() : QString());
    };

    connect(
        m_ordersPagination,
        &QtMaterial::QtMaterialPagination::pageChanged,
        this,
        [refreshOrdersPage](int) { refreshOrdersPage(); });
    connect(
        m_ordersPagination,
        &QtMaterial::QtMaterialPagination::pageSizeChanged,
        this,
        [refreshOrdersPage](int) { refreshOrdersPage(); });
    connect(
        m_ordersPage->horizontalHeader(),
        &QHeaderView::sortIndicatorChanged,
        this,
        [refreshOrdersPage](int, Qt::SortOrder) { refreshOrdersPage(); });

    connect(exportButton, &QAbstractButton::clicked, this, [this]() {
        showMessage(QStringLiteral("Order export prepared."));
    });
    connect(m_ordersPage, &QAbstractItemView::doubleClicked, this, [this](const QModelIndex& index) {
        showOrderDetailsForModel(index.row(), m_ordersPageModel);
    });

    refreshOrdersPage();

    layout->addStretch(1);
    return page;
}

QWidget* DashboardWindow::createCustomersPage()
{
    QVBoxLayout* layout = nullptr;
    auto* page = makePageShell(
        QStringLiteral("Customers"),
        QStringLiteral("CRM overview with account health, activity and lifetime value."),
        &layout);

    auto* summary = new QGridLayout;
    const struct {
        const char* title;
        const char* value;
        const char* note;
    } customerStats[] = {
        {"Total customers", "8,249", "+7.4%"},
        {"New this month", "486", "+12.1%"},
        {"Active accounts", "7,812", "94.7%"},
        {"Avg. lifetime value", "€1,284", "+5.3%"}
    };

    for (int i = 0; i < 4; ++i) {
        auto* card = new QtMaterial::QtMaterialCard(page);
        card->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
        auto* cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(16, 14, 16, 14);
        cardLayout->addWidget(makeLabel(QString::fromLatin1(customerStats[i].title), card, -1.0, false));
        cardLayout->addWidget(makeLabel(QString::fromLatin1(customerStats[i].value), card, 5.0, true));
        auto* note = makeLabel(QString::fromLatin1(customerStats[i].note), card, -2.0, false);
        note->setObjectName(QStringLiteral("positiveDelta"));
        cardLayout->addWidget(note);
        summary->addWidget(card, 0, i);
    }
    layout->addLayout(summary);

    auto* customersCard = new QtMaterial::QtMaterialCard(page);
    customersCard->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    customersCard->setMinimumHeight(390);
    auto* customersLayout = new QVBoxLayout(customersCard);
    customersLayout->setContentsMargins(18, 16, 18, 18);
    customersLayout->addWidget(makeLabel(QStringLiteral("Customer directory"), customersCard, 2.0, false));

    auto* table = new QtMaterial::QtMaterialTable(customersCard);
    table->setDense(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->setVisible(false);

    auto* model = new QStandardItemModel(7, 5, table);
    model->setHorizontalHeaderLabels({
        QStringLiteral("Customer"),
        QStringLiteral("Company"),
        QStringLiteral("Plan"),
        QStringLiteral("LTV"),
        QStringLiteral("Status")
    });
    const char* customers[][5] = {
        {"Alice Martin", "Northstar", "Enterprise", "€4,820", "Active"},
        {"Emma Dupont", "Studio Nine", "Team", "€2,460", "Active"},
        {"John Smith", "Acme Labs", "Team", "€1,980", "Active"},
        {"Lina Robert", "Orbit", "Enterprise", "€5,120", "Active"},
        {"Noah Bernard", "Mono", "Starter", "€620", "At risk"},
        {"Lucas Petit", "Altitude", "Team", "€2,180", "Active"},
        {"Mia Leroy", "Fjord", "Starter", "€940", "Trial"}
    };
    for (int row = 0; row < 7; ++row) {
        for (int column = 0; column < 5; ++column) {
            model->setItem(row, column, new QStandardItem(QString::fromUtf8(customers[row][column])));
        }
    }
    table->setModel(model);
    table->setItemDelegateForColumn(4, new StatusBadgeDelegate(table));
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    customersLayout->addWidget(table, 1);
    layout->addWidget(customersCard);
    layout->addStretch(1);
    return page;
}

QWidget* DashboardWindow::createComponentsPage()
{
    QVBoxLayout* layout = nullptr;
    auto* page = makePageShell(
        QStringLiteral("Material Components"),
        QStringLiteral("A compact application view of the same widgets exposed by the component gallery."),
        &layout);

    auto* gridHost = new QWidget(page);
    auto* grid = new QGridLayout(gridHost);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(18);
    grid->setVerticalSpacing(18);

    auto* inputs = new QtMaterial::QtMaterialCard(page);
    inputs->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    auto* inputsLayout = new QVBoxLayout(inputs);
    inputsLayout->setContentsMargins(18, 16, 18, 18);
    inputsLayout->addWidget(makeLabel(QStringLiteral("Inputs"), inputs, 2.0, false));
    auto* search = new QtMaterial::QtMaterialSearchBar(inputs);
    search->setPlaceholderText(QStringLiteral("Search components"));
    search->setClearButtonVisible(true);
    inputsLayout->addWidget(search);
    auto* checkbox = new QtMaterial::QtMaterialCheckbox(inputs);
    checkbox->setText(QStringLiteral("Receive product updates"));
    checkbox->setCheckState(Qt::Checked);
    inputsLayout->addWidget(checkbox);
    inputsLayout->addStretch(1);

    auto* navigation = new QtMaterial::QtMaterialCard(page);
    navigation->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    auto* navigationLayout = new QVBoxLayout(navigation);
    navigationLayout->setContentsMargins(18, 16, 18, 18);
    navigationLayout->addWidget(makeLabel(QStringLiteral("Navigations"), navigation, 2.0, false));
    navigationLayout->addWidget(makeLabel(QStringLiteral("Dashboard  ›  Components  ›  Overview"), navigation, -1.0, false));
    navigationLayout->addWidget(makeLabel(QStringLiteral("Sidebar navigation"), navigation, -1.0, false));
    navigationLayout->addWidget(makeLabel(QStringLiteral("Command palette  Ctrl+K"), navigation, -1.0, false));
    navigationLayout->addStretch(1);

    auto* surfaces = new QtMaterial::QtMaterialCard(page);
    surfaces->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    auto* surfacesLayout = new QVBoxLayout(surfaces);
    surfacesLayout->setContentsMargins(18, 16, 18, 18);
    surfacesLayout->addWidget(makeLabel(QStringLiteral("Surfaces & actions"), surfaces, 2.0, false));
    auto* action = new QtMaterial::QtMaterialFilledTonalButton(QStringLiteral("Create report"), surfaces);
    surfacesLayout->addWidget(action);
    auto* chips = new QHBoxLayout;
    const QStringList chipNames = {
        QStringLiteral("Assist"),
        QStringLiteral("Filter"),
        QStringLiteral("Input")
    };
    for (const QString& name : chipNames) {
        auto* chip = new QtMaterial::QtMaterialChip(name, surfaces);
        chip->setVariant(QtMaterial::ChipVariant::Assist);
        chips->addWidget(chip);
    }
    chips->addStretch(1);
    surfacesLayout->addLayout(chips);
    surfacesLayout->addStretch(1);

    auto* feedback = new QtMaterial::QtMaterialCard(page);
    feedback->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    auto* feedbackLayout = new QVBoxLayout(feedback);
    feedbackLayout->setContentsMargins(18, 16, 18, 18);
    feedbackLayout->addWidget(makeLabel(QStringLiteral("Feedback & data display"), feedback, 2.0, false));
    feedbackLayout->addWidget(makeLabel(
        QStringLiteral("Dialogs, snackbars, tables and progress feedback remain interactive."),
        feedback,
        -1.0,
        false));
    auto* snackbarButton = new QtMaterial::QtMaterialFilledTonalButton(
        QStringLiteral("Show snackbar"),
        feedback);
    feedbackLayout->addWidget(snackbarButton);
    feedbackLayout->addStretch(1);

    connect(action, &QAbstractButton::clicked, this, [this]() {
        showMessage(QStringLiteral("New report created."));
    });
    connect(snackbarButton, &QAbstractButton::clicked, this, [this]() {
        showMessage(QStringLiteral("Material feedback component triggered."));
    });

    grid->addWidget(inputs, 0, 0);
    grid->addWidget(navigation, 0, 1);
    grid->addWidget(surfaces, 1, 0);
    grid->addWidget(feedback, 1, 1);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);
    layout->addWidget(gridHost);
    layout->addStretch(1);
    return page;
}


QWidget* DashboardWindow::createProfilePage()
{
    QVBoxLayout* layout = nullptr;
    auto* page = makePageShell(
        QStringLiteral("Profile"),
        QStringLiteral("Account settings built from production-style Material input and selection controls."),
        &layout);

    auto* identityCard = new QtMaterial::QtMaterialCard(page);
    identityCard->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    auto* identityLayout = new QHBoxLayout(identityCard);
    identityLayout->setContentsMargins(20, 18, 20, 18);
    identityLayout->setSpacing(16);

    auto* avatar = new QLabel(QStringLiteral("JD"), identityCard);
    avatar->setObjectName(QStringLiteral("profileHeroAvatar"));
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setFixedSize(64, 64);

    auto* identityText = new QVBoxLayout;
    identityText->setSpacing(2);
    identityText->addWidget(makeLabel(QStringLiteral("John Doe"), identityCard, 4.0, true));
    identityText->addWidget(makeLabel(QStringLiteral("Product administrator"), identityCard, -1.0, false));
    auto* verified = new QtMaterial::QtMaterialChip(QStringLiteral("Verified account"), identityCard);
    verified->setVariant(QtMaterial::ChipVariant::Assist);
    identityText->addWidget(verified, 0, Qt::AlignLeft);

    identityLayout->addWidget(avatar);
    identityLayout->addLayout(identityText, 1);
    layout->addWidget(identityCard);

    auto* contentHost = new QWidget(page);
    auto* contentGrid = new QGridLayout(contentHost);
    contentGrid->setContentsMargins(0, 0, 0, 0);
    contentGrid->setHorizontalSpacing(18);
    contentGrid->setVerticalSpacing(18);

    auto* detailsCard = new QtMaterial::QtMaterialCard(contentHost);
    detailsCard->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    auto* detailsLayout = new QVBoxLayout(detailsCard);
    detailsLayout->setContentsMargins(20, 18, 20, 20);
    detailsLayout->setSpacing(14);
    detailsLayout->addWidget(makeLabel(QStringLiteral("Personal information"), detailsCard, 2.0, false));

    auto* name = new QtMaterial::QtMaterialOutlinedTextField(detailsCard);
    name->setLabelText(QStringLiteral("Full name"));
    name->setText(QStringLiteral("John Doe"));
    name->setRequired(true);
    name->setRequiredText(QStringLiteral("Name is required."));
    name->setEndActionMode(
        QtMaterial::QtMaterialOutlinedTextField::EndActionMode::ClearText);
    detailsLayout->addWidget(name);

    auto* email = new QtMaterial::QtMaterialOutlinedTextField(detailsCard);
    email->setLabelText(QStringLiteral("Email"));
    email->setText(QStringLiteral("john.doe@example.com"));
    email->setRequired(true);
    email->setRequiredText(QStringLiteral("Email is required."));
    email->setSupportingText(QStringLiteral("Used for account notifications."));
    detailsLayout->addWidget(email);

    auto* company = new QtMaterial::QtMaterialOutlinedTextField(detailsCard);
    company->setLabelText(QStringLiteral("Company"));
    company->setText(QStringLiteral("Material Labs"));
    detailsLayout->addWidget(company);

    auto* role = new QtMaterial::QtMaterialOutlinedTextField(detailsCard);
    role->setLabelText(QStringLiteral("Role"));
    role->setText(QStringLiteral("Product administrator"));
    detailsLayout->addWidget(role);

    auto* plan = new QtMaterial::QtMaterialComboBox(detailsCard);
    plan->setLabelText(QStringLiteral("Workspace plan"));
    plan->addItems({
        QStringLiteral("Starter"),
        QStringLiteral("Professional"),
        QStringLiteral("Enterprise")
    });
    plan->setCurrentText(QStringLiteral("Professional"));
    detailsLayout->addWidget(plan);

    auto* preferencesCard = new QtMaterial::QtMaterialCard(contentHost);
    preferencesCard->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    auto* preferencesLayout = new QVBoxLayout(preferencesCard);
    preferencesLayout->setContentsMargins(20, 18, 20, 20);
    preferencesLayout->setSpacing(12);
    preferencesLayout->addWidget(makeLabel(QStringLiteral("Preferences"), preferencesCard, 2.0, false));

    auto* notifications = new QtMaterial::QtMaterialSwitch(
        QStringLiteral("Email notifications"),
        preferencesCard);
    notifications->setChecked(true);
    preferencesLayout->addWidget(notifications);

    auto* updates = new QtMaterial::QtMaterialSwitch(
        QStringLiteral("Product updates"),
        preferencesCard);
    updates->setChecked(false);
    preferencesLayout->addWidget(updates);

    preferencesLayout->addSpacing(8);
    preferencesLayout->addWidget(makeLabel(QStringLiteral("Interface density"), preferencesCard, -1.0, true));

    auto* densityGroup = new QButtonGroup(preferencesCard);
    densityGroup->setExclusive(true);
    auto* comfortable = new QtMaterial::QtMaterialRadioButton(
        QStringLiteral("Comfortable"),
        preferencesCard);
    auto* compact = new QtMaterial::QtMaterialRadioButton(
        QStringLiteral("Compact"),
        preferencesCard);
    comfortable->setChecked(true);
    densityGroup->addButton(comfortable, 0);
    densityGroup->addButton(compact, 1);
    preferencesLayout->addWidget(comfortable);
    preferencesLayout->addWidget(compact);

    preferencesLayout->addSpacing(8);
    auto* security = new QtMaterial::QtMaterialSwitch(
        QStringLiteral("Require sign-in confirmation"),
        preferencesCard);
    security->setChecked(true);
    preferencesLayout->addWidget(security);
    preferencesLayout->addStretch(1);

    contentGrid->addWidget(detailsCard, 0, 0);
    contentGrid->addWidget(preferencesCard, 0, 1);
    contentGrid->setColumnStretch(0, 2);
    contentGrid->setColumnStretch(1, 1);
    layout->addWidget(contentHost);

    auto* actions = new QHBoxLayout;
    actions->addStretch(1);
    auto* cancel = new QtMaterial::QtMaterialOutlinedButton(
        QStringLiteral("Reset"),
        page);
    auto* save = new QtMaterial::QtMaterialFilledButton(
        QStringLiteral("Save changes"),
        page);
    actions->addWidget(cancel);
    actions->addWidget(save);
    layout->addLayout(actions);

    connect(cancel, &QAbstractButton::clicked, this, [name, email, company, role, plan, notifications, updates, comfortable, security]() {
        name->setText(QStringLiteral("John Doe"));
        email->setText(QStringLiteral("john.doe@example.com"));
        company->setText(QStringLiteral("Material Labs"));
        role->setText(QStringLiteral("Product administrator"));
        plan->setCurrentText(QStringLiteral("Professional"));
        notifications->setChecked(true);
        updates->setChecked(false);
        comfortable->setChecked(true);
        security->setChecked(true);
        name->resetValidationFeedback();
        email->resetValidationFeedback();
    });

    connect(save, &QAbstractButton::clicked, this, [this, name, email]() {
        const bool nameValid = name->validateInput();
        const bool emailValid = email->validateInput();
        if (!nameValid) {
            name->showValidationError();
        }
        if (!emailValid) {
            email->showValidationError();
        }
        if (nameValid && emailValid) {
            showMessage(QStringLiteral("Profile changes saved."));
        }
    });

    connect(notifications, &QAbstractButton::toggled, this, [this](bool checked) {
        showMessage(
            checked
                ? QStringLiteral("Email notifications enabled.")
                : QStringLiteral("Email notifications disabled."));
    });

    layout->addStretch(1);
    return page;
}

QWidget* DashboardWindow::createPricingPage()
{
    QVBoxLayout* layout = nullptr;
    auto* page = makePageShell(
        QStringLiteral("Pricing"),
        QStringLiteral("A realistic pricing surface combining cards, segmented selection, chips and Material actions."),
        &layout);

    auto* billingRow = new QHBoxLayout;
    billingRow->addWidget(makeLabel(QStringLiteral("Choose a plan"), page, 2.0, false));
    billingRow->addStretch(1);

    auto* billing = new QtMaterial::QtMaterialSegmentedButton(page);
    billing->addSegment(QStringLiteral("Monthly"));
    billing->addSegment(QStringLiteral("Annual"));
    billing->setCurrentIndex(0);
    billingRow->addWidget(billing);
    layout->addLayout(billingRow);

    auto* plansHost = new QWidget(page);
    auto* plans = new QGridLayout(plansHost);
    plans->setContentsMargins(0, 0, 0, 0);
    plans->setHorizontalSpacing(18);
    plans->setVerticalSpacing(18);

    QVector<QLabel*> priceLabels;
    const struct {
        const char* name;
        const char* description;
        int monthly;
        const char* action;
        bool featured;
    } planData[] = {
        {"Starter", "For personal projects and prototypes.", 12, "Start free", false},
        {"Professional", "For teams shipping production applications.", 29, "Choose Pro", true},
        {"Enterprise", "For organizations that need scale and governance.", 79, "Contact sales", false}
    };

    for (int i = 0; i < 3; ++i) {
        auto* card = new QtMaterial::QtMaterialCard(plansHost);
        card->setVariant(
            planData[i].featured
                ? QtMaterial::QtMaterialCard::Variant::Filled
                : QtMaterial::QtMaterialCard::Variant::Elevated);
        card->setMinimumHeight(360);

        auto* cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(22, 20, 22, 20);
        cardLayout->setSpacing(10);

        auto* titleRow = new QHBoxLayout;
        titleRow->addWidget(makeLabel(
            QString::fromLatin1(planData[i].name),
            card,
            3.0,
            true));
        titleRow->addStretch(1);
        if (planData[i].featured) {
            auto* popular = new QtMaterial::QtMaterialChip(
                QStringLiteral("Most popular"),
                card);
            popular->setVariant(QtMaterial::ChipVariant::Assist);
            titleRow->addWidget(popular);
        }
        cardLayout->addLayout(titleRow);

        auto* description = makeLabel(
            QString::fromLatin1(planData[i].description),
            card,
            -1.0,
            false);
        description->setWordWrap(true);
        cardLayout->addWidget(description);

        auto* price = makeLabel(
            QStringLiteral("€%1 / month").arg(planData[i].monthly),
            card,
            7.0,
            true);
        price->setProperty("dashboardMonthlyPrice", planData[i].monthly);
        priceLabels.append(price);
        cardLayout->addWidget(price);

        cardLayout->addSpacing(8);
        const QStringList features =
            i == 0
                ? QStringList{
                    QStringLiteral("✓  3 projects"),
                    QStringLiteral("✓  Core widgets"),
                    QStringLiteral("✓  Community support")
                }
                : (i == 1
                    ? QStringList{
                        QStringLiteral("✓  Unlimited projects"),
                        QStringLiteral("✓  All Material widgets"),
                        QStringLiteral("✓  Team collaboration"),
                        QStringLiteral("✓  Priority support")
                    }
                    : QStringList{
                        QStringLiteral("✓  Everything in Professional"),
                        QStringLiteral("✓  SSO and governance"),
                        QStringLiteral("✓  Deployment assistance"),
                        QStringLiteral("✓  Dedicated support")
                    });

        for (const QString& feature : features) {
            cardLayout->addWidget(makeLabel(feature, card, -1.0, false));
        }

        cardLayout->addStretch(1);

        QAbstractButton* action = nullptr;
        if (planData[i].featured) {
            action = new QtMaterial::QtMaterialFilledButton(
                QString::fromLatin1(planData[i].action),
                card);
        } else {
            action = new QtMaterial::QtMaterialOutlinedButton(
                QString::fromLatin1(planData[i].action),
                card);
        }
        cardLayout->addWidget(action);

        const QString planName = QString::fromLatin1(planData[i].name);
        connect(action, &QAbstractButton::clicked, this, [this, planName]() {
            auto* dialog = new QtMaterial::QtMaterialDialog(this);
            dialog->setAttribute(Qt::WA_DeleteOnClose, true);
            dialog->setTitleText(QStringLiteral("%1 selected").arg(planName));
            dialog->setSupportingText(
                QStringLiteral("This demo action shows how a pricing flow can hand off to a Material dialog."));
            dialog->open();
        });

        plans->addWidget(card, 0, i);
        plans->setColumnStretch(i, 1);
    }

    layout->addWidget(plansHost);

    auto* optionsCard = new QtMaterial::QtMaterialCard(page);
    optionsCard->setVariant(QtMaterial::QtMaterialCard::Variant::Outlined);
    auto* optionsLayout = new QHBoxLayout(optionsCard);
    optionsLayout->setContentsMargins(20, 14, 20, 14);

    auto* support = new QtMaterial::QtMaterialSwitch(
        QStringLiteral("Include priority onboarding"),
        optionsCard);
    support->setChecked(false);
    optionsLayout->addWidget(support);
    optionsLayout->addStretch(1);
    optionsLayout->addWidget(makeLabel(
        QStringLiteral("Cancel anytime • no hidden fees"),
        optionsCard,
        -1.0,
        false));
    layout->addWidget(optionsCard);

    connect(
        billing,
        &QtMaterial::QtMaterialSegmentedButton::currentIndexChanged,
        this,
        [priceLabels](int index) {
            const bool annual = index == 1;
            for (QLabel* price : priceLabels) {
                const int monthly =
                    price->property("dashboardMonthlyPrice").toInt();
                const int displayed = annual
                    ? qRound(monthly * 0.8)
                    : monthly;
                price->setText(
                    annual
                        ? QStringLiteral("€%1 / month · billed annually").arg(displayed)
                        : QStringLiteral("€%1 / month").arg(displayed));
            }
        });

    connect(support, &QAbstractButton::toggled, this, [this](bool checked) {
        showMessage(
            checked
                ? QStringLiteral("Priority onboarding added.")
                : QStringLiteral("Priority onboarding removed."));
    });

    layout->addStretch(1);
    return page;
}

QWidget* DashboardWindow::createApplicationStatesPage()
{
    QVBoxLayout* layout = nullptr;
    auto* page = makePageShell(
        QStringLiteral("Application States"),
        QStringLiteral("Preview common loading, empty, error, offline and ready states without leaving the demo."),
        &layout);

    auto* selector = new QtMaterial::QtMaterialSegmentedButton(page);
    selector->addSegment(QStringLiteral("Loading"));
    selector->addSegment(QStringLiteral("Empty"));
    selector->addSegment(QStringLiteral("Error"));
    selector->addSegment(QStringLiteral("Offline"));
    selector->addSegment(QStringLiteral("Ready"));
    selector->setCurrentIndex(0);
    layout->addWidget(selector, 0, Qt::AlignLeft);

    auto* states = new QStackedWidget(page);
    states->setMinimumHeight(360);

    auto makeStateCard = [states](
        const QString& title,
        const QString& description) {
        auto* card = new QtMaterial::QtMaterialCard(states);
        card->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
        auto* cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(28, 28, 28, 28);
        cardLayout->setSpacing(14);
        cardLayout->addWidget(makeLabel(title, card, 4.0, true));
        auto* body = makeLabel(description, card, -1.0, false);
        body->setWordWrap(true);
        cardLayout->addWidget(body);
        return qMakePair(card, cardLayout);
    };

    {
        const auto loadingState = makeStateCard(
            QStringLiteral("Loading dashboard data"),
            QStringLiteral("Use determinate or indeterminate progress indicators while remote data is being resolved."));
        auto* progress = new QtMaterial::QtMaterialCircularProgressIndicator(
            loadingState.first);
        progress->setMode(
            QtMaterial::QtMaterialCircularProgressIndicator::Mode::Indeterminate);
        progress->setStatusText(QStringLiteral("Loading analytics"));
        loadingState.second->addWidget(progress, 0, Qt::AlignHCenter);

        auto* linear = new QtMaterial::QtMaterialLinearProgressIndicator(
            loadingState.first);
        linear->setMode(
            QtMaterial::QtMaterialLinearProgressIndicator::Mode::Indeterminate);
        linear->setStatusText(QStringLiteral("Refreshing dashboard"));
        loadingState.second->addWidget(linear);
        loadingState.second->addStretch(1);
        states->addWidget(loadingState.first);
    }

    {
        const auto emptyState = makeStateCard(
            QStringLiteral("No orders yet"),
            QStringLiteral("Empty states should explain what happened and provide a clear next action."));
        auto* icon = new QLabel(emptyState.first);
        icon->setAlignment(Qt::AlignCenter);
        icon->setPixmap(dashboardIcon(
            QStringLiteral("orders"),
            materialColor(QtMaterial::ColorRole::Primary)).pixmap(64, 64));
        emptyState.second->addWidget(icon);

        auto* action = new QtMaterial::QtMaterialFilledButton(
            QStringLiteral("Create first order"),
            emptyState.first);
        emptyState.second->addWidget(action, 0, Qt::AlignHCenter);
        emptyState.second->addStretch(1);
        states->addWidget(emptyState.first);

        connect(action, &QAbstractButton::clicked, this, [this, selector]() {
            showMessage(QStringLiteral("A new order was created for the demo."));
            selector->setCurrentIndex(4);
        });
    }

    {
        const auto errorState = makeStateCard(
            QStringLiteral("Something went wrong"),
            QStringLiteral("Material banners can keep a recoverable problem visible without blocking the entire application."));
        auto* banner = new QtMaterialBanner(
            QStringLiteral("Unable to load analytics"),
            QStringLiteral("The service returned an unexpected response. Your local data is safe."),
            errorState.first);
        banner->setPrimaryActionText(QStringLiteral("Retry"));
        banner->setSecondaryActionText(QStringLiteral("Details"));
        banner->setDismissible(true);
        errorState.second->addWidget(banner);
        errorState.second->addStretch(1);
        states->addWidget(errorState.first);

        connect(
            banner,
            &QtMaterialBanner::primaryActionTriggered,
            this,
            [this, selector]() {
                showMessage(QStringLiteral("Retry succeeded."));
                selector->setCurrentIndex(4);
            });
        connect(
            banner,
            &QtMaterialBanner::secondaryActionTriggered,
            this,
            [this]() {
                showMessage(QStringLiteral("HTTP 503 • analytics service unavailable."));
            });
    }

    {
        const auto offlineState = makeStateCard(
            QStringLiteral("Working offline"),
            QStringLiteral("The dashboard can clearly communicate degraded connectivity while preserving local actions."));
        auto* banner = new QtMaterialBanner(
            QStringLiteral("No network connection"),
            QStringLiteral("Changes will be queued locally and synchronized when the connection returns."),
            offlineState.first);
        banner->setPrimaryActionText(QStringLiteral("Try again"));
        banner->setDismissible(false);
        offlineState.second->addWidget(banner);

        auto* reconnect = new QtMaterial::QtMaterialSwitch(
            QStringLiteral("Automatically reconnect"),
            offlineState.first);
        reconnect->setChecked(true);
        offlineState.second->addWidget(reconnect);
        offlineState.second->addStretch(1);
        states->addWidget(offlineState.first);

        connect(
            banner,
            &QtMaterialBanner::primaryActionTriggered,
            this,
            [this, selector]() {
                showMessage(QStringLiteral("Connection restored."));
                selector->setCurrentIndex(4);
            });
    }

    {
        const auto readyState = makeStateCard(
            QStringLiteral("Everything is up to date"),
            QStringLiteral("The ready state confirms that dashboard data, orders and local changes are synchronized."));
        auto* icon = new QLabel(readyState.first);
        icon->setAlignment(Qt::AlignCenter);
        icon->setPixmap(dashboardIcon(
            QStringLiteral("dashboard"),
            materialColor(QtMaterial::ColorRole::Tertiary)).pixmap(64, 64));
        readyState.second->addWidget(icon);

        auto* action = new QtMaterial::QtMaterialOutlinedButton(
            QStringLiteral("Refresh again"),
            readyState.first);
        readyState.second->addWidget(action, 0, Qt::AlignHCenter);
        readyState.second->addStretch(1);
        states->addWidget(readyState.first);

        connect(action, &QAbstractButton::clicked, this, [selector]() {
            selector->setCurrentIndex(0);
        });
    }

    connect(
        selector,
        &QtMaterial::QtMaterialSegmentedButton::currentIndexChanged,
        states,
        &QStackedWidget::setCurrentIndex);

    layout->addWidget(states);
    layout->addStretch(1);
    return page;
}


QWidget* DashboardWindow::createShowcaseSettingsPage()
{
    QVBoxLayout* layout = nullptr;
    auto* page = makePageShell(
        QStringLiteral("Showcase Settings"),
        QStringLiteral("Exercise the public theme model, contrast variants and bidirectional layout in the live dashboard."),
        &layout);

    auto* controlsHost = new QWidget(page);
    auto* controls = new QGridLayout(controlsHost);
    controls->setContentsMargins(0, 0, 0, 0);
    controls->setHorizontalSpacing(18);
    controls->setVerticalSpacing(18);

    auto* themeCard = new QtMaterial::QtMaterialCard(controlsHost);
    themeCard->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    auto* themeLayout = new QVBoxLayout(themeCard);
    themeLayout->setContentsMargins(20, 18, 20, 20);
    themeLayout->setSpacing(12);
    themeLayout->addWidget(makeLabel(QStringLiteral("Theme"), themeCard, 2.0, false));
    themeLayout->addWidget(makeLabel(
        QStringLiteral("All controls below update the same ThemeManager used by the rest of the application."),
        themeCard,
        -1.0,
        false));

    auto* seed = new QtMaterial::QtMaterialComboBox(themeCard);
    seed->setLabelText(QStringLiteral("Seed color"));
    seed->addItems({
        QStringLiteral("Indigo"),
        QStringLiteral("Material Purple"),
        QStringLiteral("Azure"),
        QStringLiteral("Amber"),
        QStringLiteral("Teal")
    });
    seed->setCurrentText(QStringLiteral("Indigo"));
    themeLayout->addWidget(seed);

    themeLayout->addWidget(makeLabel(QStringLiteral("Mode"), themeCard, -1.0, true));
    auto* mode = new QtMaterial::QtMaterialSegmentedButton(themeCard);
    mode->addSegment(QStringLiteral("Light"));
    mode->addSegment(QStringLiteral("Dark"));
    mode->setCurrentIndex(
        QtMaterial::ThemeManager::instance().theme().isDark() ? 1 : 0);
    themeLayout->addWidget(mode);

    themeLayout->addWidget(makeLabel(QStringLiteral("Contrast"), themeCard, -1.0, true));
    auto* contrast = new QtMaterial::QtMaterialSegmentedButton(themeCard);
    contrast->addSegment(QStringLiteral("Standard"));
    contrast->addSegment(QStringLiteral("Medium"));
    contrast->addSegment(QStringLiteral("High"));
    contrast->setCurrentIndex(0);
    themeLayout->addWidget(contrast);

    themeLayout->addWidget(makeLabel(QStringLiteral("Color variant"), themeCard, -1.0, true));
    auto* variant = new QtMaterial::QtMaterialSegmentedButton(themeCard);
    variant->addSegment(QStringLiteral("Tonal"));
    variant->addSegment(QStringLiteral("Expressive"));
    variant->setCurrentIndex(0);
    themeLayout->addWidget(variant);

    auto* layoutCard = new QtMaterial::QtMaterialCard(controlsHost);
    layoutCard->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    auto* layoutSettings = new QVBoxLayout(layoutCard);
    layoutSettings->setContentsMargins(20, 18, 20, 20);
    layoutSettings->setSpacing(12);
    layoutSettings->addWidget(makeLabel(QStringLiteral("Layout & accessibility"), layoutCard, 2.0, false));
    layoutSettings->addWidget(makeLabel(
        QStringLiteral("Use the same window to verify LTR/RTL mirroring and accessible control semantics."),
        layoutCard,
        -1.0,
        false));

    layoutSettings->addWidget(makeLabel(QStringLiteral("Direction"), layoutCard, -1.0, true));
    auto* direction = new QtMaterial::QtMaterialSegmentedButton(layoutCard);
    direction->addSegment(QStringLiteral("LTR"));
    direction->addSegment(QStringLiteral("RTL"));
    direction->setCurrentIndex(
        m_central && m_central->layoutDirection() == Qt::RightToLeft
            ? 1
            : 0);
    layoutSettings->addWidget(direction);

    auto* labels = new QtMaterial::QtMaterialSwitch(
        QStringLiteral("Show navigation rail labels"),
        layoutCard);
    labels->setChecked(
        m_navigationRail ? m_navigationRail->labelsVisible() : false);
    layoutSettings->addWidget(labels);

    auto* compactPreview = new QtMaterial::QtMaterialSwitch(
        QStringLiteral("Compact table rows"),
        layoutCard);
    compactPreview->setChecked(
        m_orders ? m_orders->dense() : true);
    layoutSettings->addWidget(compactPreview);

    auto* reset = new QtMaterial::QtMaterialOutlinedButton(
        QStringLiteral("Reset showcase"),
        layoutCard);
    layoutSettings->addWidget(reset, 0, Qt::AlignLeft);
    layoutSettings->addStretch(1);

    controls->addWidget(themeCard, 0, 0);
    controls->addWidget(layoutCard, 0, 1);
    controls->setColumnStretch(0, 1);
    controls->setColumnStretch(1, 1);
    layout->addWidget(controlsHost);

    auto* preview = new QtMaterial::QtMaterialCard(page);
    preview->setVariant(QtMaterial::QtMaterialCard::Variant::Outlined);
    auto* previewLayout = new QVBoxLayout(preview);
    previewLayout->setContentsMargins(20, 18, 20, 20);
    previewLayout->setSpacing(12);
    previewLayout->addWidget(makeLabel(QStringLiteral("Live color roles"), preview, 2.0, false));

    auto* swatches = new QHBoxLayout;
    swatches->setSpacing(10);

    struct Swatch {
        QLabel* box = nullptr;
        QtMaterial::ColorRole role = QtMaterial::ColorRole::Primary;
        QtMaterial::ColorRole onRole = QtMaterial::ColorRole::OnPrimary;
        const char* name = nullptr;
    };

    QVector<Swatch> previewSwatches;
    const struct {
        QtMaterial::ColorRole role;
        QtMaterial::ColorRole onRole;
        const char* name;
    } swatchData[] = {
        {QtMaterial::ColorRole::Primary, QtMaterial::ColorRole::OnPrimary, "Primary"},
        {QtMaterial::ColorRole::Secondary, QtMaterial::ColorRole::OnSecondary, "Secondary"},
        {QtMaterial::ColorRole::Tertiary, QtMaterial::ColorRole::OnTertiary, "Tertiary"},
        {QtMaterial::ColorRole::Error, QtMaterial::ColorRole::OnError, "Error"},
        {QtMaterial::ColorRole::SurfaceContainerHighest, QtMaterial::ColorRole::OnSurface, "Surface"}
    };

    for (const auto& item : swatchData) {
        auto* box = new QLabel(QString::fromLatin1(item.name), preview);
        box->setAlignment(Qt::AlignCenter);
        box->setMinimumHeight(64);
        box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        swatches->addWidget(box, 1);
        previewSwatches.append(Swatch{box, item.role, item.onRole, item.name});
    }
    previewLayout->addLayout(swatches);

    auto refreshSwatches = [previewSwatches]() {
        for (const Swatch& swatch : previewSwatches) {
            swatch.box->setStyleSheet(QStringLiteral(
                "background:%1; color:%2; border-radius:10px; font-weight:600;")
                .arg(cssColor(materialColor(swatch.role)))
                .arg(cssColor(materialColor(swatch.onRole))));
        }
    };
    refreshSwatches();

    auto applyThemeControls = [seed, mode, contrast, variant]() {
        auto options = QtMaterial::ThemeManager::instance().options();

        const QString seedName = seed->currentText();
        if (seedName == QStringLiteral("Material Purple")) {
            options.sourceColor = QColor(QStringLiteral("#6750A4"));
        } else if (seedName == QStringLiteral("Azure")) {
            options.sourceColor = QColor(QStringLiteral("#00639B"));
        } else if (seedName == QStringLiteral("Amber")) {
            options.sourceColor = QColor(QStringLiteral("#FFB300"));
        } else if (seedName == QStringLiteral("Teal")) {
            options.sourceColor = QColor(QStringLiteral("#006A60"));
        } else {
            options.sourceColor = QColor(QStringLiteral("#4455C7"));
        }

        options.mode =
            mode->currentIndex() == 1
                ? QtMaterial::ThemeMode::Dark
                : QtMaterial::ThemeMode::Light;
        options.preference =
            mode->currentIndex() == 1
                ? QtMaterial::ThemePreference::Dark
                : QtMaterial::ThemePreference::Light;

        switch (contrast->currentIndex()) {
        case 1:
            options.contrast = QtMaterial::ContrastMode::Medium;
            break;
        case 2:
            options.contrast = QtMaterial::ContrastMode::High;
            break;
        default:
            options.contrast = QtMaterial::ContrastMode::Standard;
            break;
        }

        options.variant =
            variant->currentIndex() == 1
                ? QtMaterial::ThemeVariant::Expressive
                : QtMaterial::ThemeVariant::TonalSpot;

        QtMaterial::ThemeManager::instance().setThemeOptions(options);
    };

    connect(
        seed,
        &QComboBox::currentTextChanged,
        this,
        [applyThemeControls](const QString&) {
            applyThemeControls();
        });
    connect(
        mode,
        &QtMaterial::QtMaterialSegmentedButton::currentIndexChanged,
        this,
        [applyThemeControls](int) {
            applyThemeControls();
        });
    connect(
        contrast,
        &QtMaterial::QtMaterialSegmentedButton::currentIndexChanged,
        this,
        [applyThemeControls](int) {
            applyThemeControls();
        });
    connect(
        variant,
        &QtMaterial::QtMaterialSegmentedButton::currentIndexChanged,
        this,
        [applyThemeControls](int) {
            applyThemeControls();
        });

    connect(
        direction,
        &QtMaterial::QtMaterialSegmentedButton::currentIndexChanged,
        this,
        [this](int index) {
            if (m_central) {
                m_central->setLayoutDirection(
                    index == 1
                        ? Qt::RightToLeft
                        : Qt::LeftToRight);
            }
            showMessage(
                index == 1
                    ? QStringLiteral("RTL layout enabled.")
                    : QStringLiteral("LTR layout enabled."));
        });

    connect(labels, &QAbstractButton::toggled, this, [this](bool checked) {
        if (m_navigationRail) {
            m_navigationRail->setLabelsVisible(checked);
        }
    });

    connect(compactPreview, &QAbstractButton::toggled, this, [this](bool checked) {
        if (m_orders) {
            m_orders->setDense(checked);
        }
        if (m_ordersPage) {
            m_ordersPage->setDense(checked);
        }
    });

    connect(reset, &QAbstractButton::clicked, this, [this, seed, mode, contrast, variant, direction, labels, compactPreview]() {
        seed->setCurrentText(QStringLiteral("Indigo"));
        mode->setCurrentIndex(0);
        contrast->setCurrentIndex(0);
        variant->setCurrentIndex(0);
        direction->setCurrentIndex(0);
        labels->setChecked(false);
        compactPreview->setChecked(true);

        auto options = QtMaterial::ThemeManager::instance().options();
        options.sourceColor = QColor(QStringLiteral("#4455C7"));
        options.mode = QtMaterial::ThemeMode::Light;
        options.preference = QtMaterial::ThemePreference::Light;
        options.contrast = QtMaterial::ContrastMode::Standard;
        options.variant = QtMaterial::ThemeVariant::TonalSpot;
        QtMaterial::ThemeManager::instance().setThemeOptions(options);

        if (m_central) {
            m_central->setLayoutDirection(Qt::LeftToRight);
        }
        showMessage(QStringLiteral("Showcase settings reset."));
    });

    connect(
        &QtMaterial::ThemeManager::instance(),
        &QtMaterial::ThemeManager::themeChanged,
        page,
        [refreshSwatches, mode](const QtMaterial::Theme& theme) {
            refreshSwatches();
            const int target = theme.isDark() ? 1 : 0;
            if (mode->currentIndex() != target) {
                mode->setCurrentIndex(target);
            }
        });

    layout->addWidget(preview);
    layout->addStretch(1);
    return page;
}

QWidget* DashboardWindow::createQuickStatistics()
{
    auto* host = new QWidget(m_contentHost);
    auto* outer = new QGridLayout(host);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setHorizontalSpacing(18);
    outer->setVerticalSpacing(18);

    auto* metricHost = new QWidget(host);
    m_quickGrid = new QGridLayout(metricHost);
    m_quickGrid->setContentsMargins(0, 0, 0, 0);
    m_quickGrid->setHorizontalSpacing(14);
    m_quickGrid->setVerticalSpacing(14);

    m_metrics.append(createMetricCard(
        QStringLiteral("Total Clients"),
        QStringLiteral("43"),
        QStringLiteral("+8.2%"),
        QtMaterial::ColorRole::Error,
        QStringLiteral("customers")));
    m_metrics.append(createMetricCard(
        QStringLiteral("Paid Invoices"),
        QStringLiteral("€10,600"),
        QStringLiteral("+12.4%"),
        QtMaterial::ColorRole::Primary,
        QStringLiteral("invoice")));
    m_metrics.append(createMetricCard(
        QStringLiteral("Total Projects"),
        QStringLiteral("73"),
        QStringLiteral("+5.1%"),
        QtMaterial::ColorRole::Secondary,
        QStringLiteral("projects")));
    m_metrics.append(createMetricCard(
        QStringLiteral("Open Projects"),
        QStringLiteral("33"),
        QStringLiteral("+3.7%"),
        QtMaterial::ColorRole::Tertiary,
        QStringLiteral("orders")));

    for (const MetricWidgets& metric : m_metrics) {
        m_metricCards.append(metric.card);
    }

    m_revenueSummary = createRevenueSummary();

    outer->addWidget(metricHost, 0, 0, 1, 2);
    outer->addWidget(m_revenueSummary, 0, 2);
    outer->setColumnStretch(0, 1);
    outer->setColumnStretch(1, 1);
    outer->setColumnStretch(2, 1);
    return host;
}

DashboardWindow::MetricWidgets DashboardWindow::createMetricCard(
    const QString& title,
    const QString& value,
    const QString& delta,
    QtMaterial::ColorRole iconRole,
    const QString& iconText)
{
    MetricWidgets metric;
    metric.card = new QtMaterial::QtMaterialCard(m_contentHost);
    metric.card->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    metric.card->setMinimumHeight(86);

    auto* row = new QHBoxLayout(metric.card);
    row->setContentsMargins(11, 9, 14, 9);
    row->setSpacing(12);

    auto* icon = new QLabel(metric.card);
    icon->setObjectName(QStringLiteral("metricIcon"));
    icon->setProperty("dashboardColorRole", static_cast<int>(iconRole));
    icon->setProperty("dashboardIconName", iconText);
    icon->setAlignment(Qt::AlignCenter);
    icon->setFixedSize(44, 44);
    const QColor iconColor = materialColor(iconRole);
    icon->setStyleSheet(QStringLiteral(
        "background:%1; border-radius:6px;")
        .arg(cssColor(iconColor)));
    icon->setPixmap(dashboardIcon(
        iconText,
        materialColor(QtMaterial::ColorRole::OnPrimary)).pixmap(24, 24));
    row->addWidget(icon);

    auto* text = new QVBoxLayout;
    text->setSpacing(1);
    auto* titleLabel = makeLabel(title, metric.card, -1.0, false);
    titleLabel->setObjectName(QStringLiteral("metricTitle"));
    metric.value = makeLabel(value, metric.card, 3.0, false);
    metric.delta = makeLabel(delta, metric.card, -2.0, false);
    metric.delta->setProperty("dashboardPositive", true);
    metric.delta->setVisible(false);
    text->addWidget(titleLabel);
    text->addWidget(metric.value);
    text->addWidget(metric.delta);
    row->addLayout(text, 1);

    return metric;
}

QWidget* DashboardWindow::createRevenueSummary()
{
    return new RevenueSummaryWidget(m_contentHost);
}

QtMaterial::QtMaterialCard* DashboardWindow::createStatisticsCard()
{
    auto* card = new QtMaterial::QtMaterialCard(m_contentHost);
    card->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    card->setMinimumHeight(342);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 14, 18, 12);
    layout->setSpacing(7);

    auto* titleRow = new QHBoxLayout;
    titleRow->setSpacing(6);
    titleRow->addWidget(makeLabel(QStringLiteral("Statistics"), card, 2.0, false));
    titleRow->addSpacing(14);

    const QStringList tabs = {
        QStringLiteral("Project"),
        QStringLiteral("New Clients"),
        QStringLiteral("Income")
    };
    for (int i = 0; i < tabs.size(); ++i) {
        auto* tab = new QToolButton(card);
        tab->setText(tabs.at(i));
        tab->setCheckable(true);
        tab->setAutoExclusive(true);
        tab->setChecked(i == 0);
        tab->setObjectName(QStringLiteral("chartTab"));
        titleRow->addWidget(tab);
        connect(tab, &QToolButton::clicked, this, [this, i]() {
            m_chartMetricIndex = i;
            applyPeriod();
        });
    }
    titleRow->addStretch(1);

    m_yearCombo = new QtMaterial::QtMaterialComboBox(card);
    m_yearCombo->setLabelText(QStringLiteral("Year"));
    m_yearCombo->addItems({QStringLiteral("2025"), QStringLiteral("2026"), QStringLiteral("2027")});
    m_yearCombo->setCurrentText(QStringLiteral("2026"));
    m_yearCombo->setMinimumWidth(92);
    titleRow->addWidget(m_yearCombo);

    m_monthCombo = new QtMaterial::QtMaterialComboBox(card);
    m_monthCombo->setLabelText(QStringLiteral("Month"));
    m_monthCombo->addItems({
        QStringLiteral("January"),
        QStringLiteral("March"),
        QStringLiteral("June"),
        QStringLiteral("September"),
        QStringLiteral("December")
    });
    m_monthCombo->setCurrentText(QStringLiteral("September"));
    m_monthCombo->setMinimumWidth(124);
    titleRow->addWidget(m_monthCombo);

    layout->addLayout(titleRow);

    m_lineChart = new LineChartWidget(card);
    m_lineChart->clearAccentColor();
    layout->addWidget(m_lineChart, 1);
    return card;
}

QtMaterial::QtMaterialCard* DashboardWindow::createEarningsCard()
{
    auto* card = new QtMaterial::QtMaterialCard(m_contentHost);
    card->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    card->setMinimumHeight(342);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 14, 18, 14);
    layout->setSpacing(8);
    layout->addWidget(makeLabel(QStringLiteral("Earning in Month"), card, 2.0, false));

    m_donutChart = new DonutChartWidget(card);
    layout->addWidget(m_donutChart, 1);

    auto* separator = new QFrame(card);
    separator->setFrameShape(QFrame::HLine);
    separator->setObjectName(QStringLiteral("dashboardSeparator"));
    layout->addWidget(separator);

    const struct {
        const char* label;
        const char* value;
        QtMaterial::ColorRole role;
    } legend[] = {
        {"Earning:", "€18,756", QtMaterial::ColorRole::Primary},
        {"Pending:", "€5,599", QtMaterial::ColorRole::Tertiary},
        {"Refund:", "€4,987", QtMaterial::ColorRole::Error}
    };

    for (const auto& item : legend) {
        auto* row = new QHBoxLayout;
        auto* dot = new QLabel(QStringLiteral("●"), card);
        dot->setObjectName(QStringLiteral("legendDot"));
        dot->setProperty("dashboardColorRole", static_cast<int>(item.role));
        row->addWidget(dot);
        row->addWidget(makeLabel(QString::fromLatin1(item.label), card, -1.0, false));
        row->addStretch(1);
        row->addWidget(makeLabel(QString::fromLatin1(item.value), card, -1.0, false));
        layout->addLayout(row);
    }

    return card;
}

QWidget* DashboardWindow::createLowerHighlights()
{
    auto* host = new QWidget(m_contentHost);
    auto* grid = new QGridLayout(host);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(18);
    grid->setVerticalSpacing(18);

    auto* social = new QtMaterial::QtMaterialCard(host);
    social->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    social->setMinimumHeight(250);
    auto* socialLayout = new QVBoxLayout(social);
    socialLayout->setContentsMargins(18, 14, 18, 14);
    socialLayout->setSpacing(4);
    socialLayout->addWidget(makeLabel(QStringLiteral("Social Media Advertising"), social, 2.0, false));
    socialLayout->addWidget(makeLabel(
        QStringLiteral("Campaign performance"),
        social,
        -2.0,
        false));

    auto* bars = new SocialBarsWidget(social);
    socialLayout->addWidget(bars, 1);

    auto* tasks = new QtMaterial::QtMaterialCard(host);
    tasks->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    tasks->setMinimumHeight(250);
    auto* tasksLayout = new QVBoxLayout(tasks);
    tasksLayout->setContentsMargins(18, 14, 18, 14);
    tasksLayout->setSpacing(8);

    auto* tasksHeader = new QHBoxLayout;
    tasksHeader->addWidget(makeLabel(QStringLiteral("Today's Tasks"), tasks, 2.0, false));
    tasksHeader->addStretch(1);

    auto* createTask = new QToolButton(tasks);
    createTask->setText(QStringLiteral("Create Task"));
    createTask->setObjectName(QStringLiteral("linkButton"));
    tasksHeader->addWidget(createTask);

    auto* viewAll = new QToolButton(tasks);
    viewAll->setText(QStringLiteral("View All"));
    viewAll->setObjectName(QStringLiteral("linkButton"));
    tasksHeader->addWidget(viewAll);
    tasksLayout->addLayout(tasksHeader);

    auto* content = new QWidget(tasks);
    auto* contentLayout = new QHBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(16);

    auto* checklistHost = new QWidget(content);
    auto* checklist = new QVBoxLayout(checklistHost);
    checklist->setContentsMargins(0, 4, 0, 0);
    checklist->setSpacing(10);

    const struct {
        const char* title;
        const char* detail;
        int day;
        bool done;
    } taskItems[] = {
        {"Send the Billing Agreement", "Scheduled on 24 Sep, 2026", 24, false},
        {"Send over all the documentation", "Scheduled on 24 Sep, 2026", 24, false},
        {"Review dashboard accessibility", "Scheduled on 22 Sep, 2026", 22, true}
    };

    for (const auto& item : taskItems) {
        auto* row = new QWidget(checklistHost);
        row->setObjectName(QStringLiteral("dashboardTaskRow"));
        row->setProperty(
            "dashboardTaskDate",
            QDate(2026, 9, item.day));
        auto* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(8);

        auto* check = new QtMaterial::QtMaterialCheckbox(row);
        check->setFixedWidth(28);
        check->setCheckState(item.done ? Qt::Checked : Qt::Unchecked);
        rowLayout->addWidget(check, 0, Qt::AlignTop);

        auto* copy = new QVBoxLayout;
        copy->setSpacing(1);
        auto* title = makeLabel(QString::fromLatin1(item.title), row, -1.0, false);
        auto* detail = makeLabel(QString::fromLatin1(item.detail), row, -2.0, false);
        detail->setObjectName(QStringLiteral("taskDetail"));
        copy->addWidget(title);
        copy->addWidget(detail);
        rowLayout->addLayout(copy, 1);

        checklist->addWidget(row);
    }
    auto* emptyTasks = makeLabel(
        QStringLiteral("No tasks for the selected day."),
        checklistHost,
        -1.0,
        false);
    emptyTasks->setObjectName(QStringLiteral("taskDetail"));
    emptyTasks->setVisible(false);
    checklist->addWidget(emptyTasks);
    checklist->addStretch(1);

    auto* calendar = new QCalendarWidget(content);
    calendar->setObjectName(QStringLiteral("dashboardCalendar"));
    calendar->setGridVisible(false);
    calendar->setNavigationBarVisible(true);
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    calendar->setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
    calendar->setCurrentPage(2026, 9);
    calendar->setSelectedDate(QDate(2026, 9, 24));
    calendar->setMinimumWidth(220);
    calendar->setMaximumWidth(300);
    calendar->setMinimumHeight(188);
    calendar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    contentLayout->addWidget(checklistHost, 3);
    contentLayout->addWidget(calendar, 2);
    tasksLayout->addWidget(content, 1);

    connect(createTask, &QToolButton::clicked, this, [this]() {
        showMessage(QStringLiteral("Task creation action triggered."));
    });
    connect(viewAll, &QToolButton::clicked, this, [this]() {
        showMessage(QStringLiteral("All tasks are already visible in this demo."));
    });
    connect(calendar, &QCalendarWidget::clicked, this, [this, checklistHost, emptyTasks](const QDate& date) {
        const auto taskRows =
            checklistHost->findChildren<QWidget*>(
                QStringLiteral("dashboardTaskRow"));
        int visibleTasks = 0;
        for (QWidget* row : taskRows) {
            const QDate taskDate =
                row->property("dashboardTaskDate").toDate();
            const bool visible = taskDate == date;
            row->setVisible(visible);
            if (visible) {
                ++visibleTasks;
            }
        }
        emptyTasks->setVisible(visibleTasks == 0);
        showMessage(
            visibleTasks == 0
                ? QStringLiteral("No tasks on %1").arg(
                    date.toString(QStringLiteral("dd MMM yyyy")))
                : QStringLiteral("%1 task(s) on %2")
                    .arg(visibleTasks)
                    .arg(date.toString(QStringLiteral("dd MMM yyyy"))));
    });

    grid->addWidget(social, 0, 0);
    grid->addWidget(tasks, 0, 1);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 2);
    return host;
}

QtMaterial::QtMaterialCard* DashboardWindow::createOrdersCard()
{
    auto* card = new QtMaterial::QtMaterialCard(m_contentHost);
    card->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    card->setMinimumHeight(330);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 16, 18, 18);
    layout->setSpacing(10);

    auto* header = new QHBoxLayout;
    header->addWidget(makeLabel(QStringLiteral("Recent Orders"), card, 2.0, false));
    header->addStretch(1);
    auto* exportButton = new QtMaterial::QtMaterialFilledTonalButton(QStringLiteral("Export report"), card);
    header->addWidget(exportButton);
    layout->addLayout(header);

    m_orders = new QtMaterial::QtMaterialTable(card);
    m_orders->setDense(true);
    m_orders->setAlternatingRowColors(false);
    m_orders->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_orders->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_orders->horizontalHeader()->setStretchLastSection(true);
    m_orders->verticalHeader()->setVisible(false);
    m_orders->setMinimumHeight(240);
    layout->addWidget(m_orders, 1);

    populateOrders();

    connect(exportButton, &QAbstractButton::clicked, this, [this]() {
        showMessage(QStringLiteral("Report export queued."));
    });
    connect(m_orders, &QtMaterial::QtMaterialTable::rowActivated, this, &DashboardWindow::showOrderDetails);
    connect(m_orders, &QAbstractItemView::doubleClicked, this, [this](const QModelIndex& index) {
        showOrderDetails(index.row());
    });

    return card;
}

void DashboardWindow::populateOrders()
{
    m_ordersModel = new QStandardItemModel(6, 5, m_orders);
    m_ordersModel->setHorizontalHeaderLabels({
        QStringLiteral("Order"),
        QStringLiteral("Customer"),
        QStringLiteral("Product"),
        QStringLiteral("Amount"),
        QStringLiteral("Status")
    });

    struct Order {
        const char* id;
        const char* customer;
        const char* product;
        const char* amount;
        const char* status;
    };
    static const Order orders[] = {
        {"#1042", "Alice Martin", "Design system", "€842", "Paid"},
        {"#1041", "John Smith", "Widget pack", "€392", "Pending"},
        {"#1040", "Emma Dupont", "Enterprise license", "€1,240", "Paid"},
        {"#1039", "Noah Bernard", "Theme pack", "€184", "Refunded"},
        {"#1038", "Lina Robert", "Support plan", "€640", "Paid"},
        {"#1037", "Lucas Petit", "Component pack", "€512", "Pending"}
    };

    for (int row = 0; row < 6; ++row) {
        m_ordersModel->setItem(row, 0, new QStandardItem(QString::fromLatin1(orders[row].id)));
        m_ordersModel->setItem(row, 1, new QStandardItem(QString::fromLatin1(orders[row].customer)));
        m_ordersModel->setItem(row, 2, new QStandardItem(QString::fromLatin1(orders[row].product)));
        m_ordersModel->setItem(row, 3, new QStandardItem(QString::fromLatin1(orders[row].amount)));
        m_ordersModel->setItem(row, 4, new QStandardItem(QString::fromLatin1(orders[row].status)));
    }

    m_orders->setModel(m_ordersModel);
    m_orders->setItemDelegateForColumn(4, new StatusBadgeDelegate(m_orders));
    m_orders->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_orders->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_orders->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_orders->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_orders->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
}

void DashboardWindow::populateCommandPalette()
{
    auto* model = new QStandardItemModel(m_commandPalette);
    model->setHorizontalHeaderLabels({QStringLiteral("Command")});
    model->appendRow(new QStandardItem(QStringLiteral("Open dashboard")));
    model->appendRow(new QStandardItem(QStringLiteral("Open analytics")));
    model->appendRow(new QStandardItem(QStringLiteral("Open orders")));
    model->appendRow(new QStandardItem(QStringLiteral("Open customers")));
    model->appendRow(new QStandardItem(QStringLiteral("Open components")));
    model->appendRow(new QStandardItem(QStringLiteral("Open profile")));
    model->appendRow(new QStandardItem(QStringLiteral("Open pricing")));
    model->appendRow(new QStandardItem(QStringLiteral("Open application states")));
    model->appendRow(new QStandardItem(QStringLiteral("Open showcase settings")));
    m_commandPalette->setSourceModel(model);
}

void DashboardWindow::applyPeriod()
{
    if (!m_lineChart || !m_yearCombo || !m_monthCombo) {
        return;
    }

    const int yearOffset = m_yearCombo->currentText().toInt() - 2025;
    const int monthOffset = m_monthCombo->currentIndex();
    const int delta = yearOffset * 5 + monthOffset * 2;

    QVector<qreal> values;
    QtMaterial::ColorRole chartRole = QtMaterial::ColorRole::Primary;

    switch (m_chartMetricIndex) {
    case 1:
        values = {
            28.0, 37.0, 34.0, 49.0, 44.0, 58.0,
            55.0, 72.0, 69.0, 83.0, 78.0, 91.0
        };
        chartRole = QtMaterial::ColorRole::Tertiary;
        break;
    case 2:
        values = {
            62.0, 84.0, 73.0, 105.0, 92.0, 127.0,
            116.0, 151.0, 143.0, 168.0, 161.0, 188.0
        };
        chartRole = QtMaterial::ColorRole::Secondary;
        break;
    default:
        values = {
            42.0, 70.0, 50.0, 96.0, 66.0, 118.0,
            90.0, 138.0, 112.0, 158.0, 132.0, 174.0
        };
        break;
    }

    for (qreal& value : values) {
        value += delta;
    }
    m_lineChart->setAccentColor(materialColor(chartRole));
    m_lineChart->setValues(values);

    if (m_metrics.size() >= 4) {
        m_metrics.at(0).value->setText(
            QString::number(43 + delta));
        m_metrics.at(1).value->setText(
            QStringLiteral("€%1").arg(10600 + delta * 135));
        m_metrics.at(2).value->setText(
            QString::number(73 + delta / 2));
        m_metrics.at(3).value->setText(
            QString::number(33 + delta / 3));
    }

    if (m_donutChart) {
        m_donutChart->setValue(qBound(55, 62 + delta / 2, 78));
    }
}

void DashboardWindow::applyFilter(const QString& text)
{
    const QString needle = text.trimmed();

    const auto matchesText =
        [&needle](QStandardItemModel* model, int row) {
            if (needle.isEmpty()) {
                return true;
            }
            for (int column = 0; column < model->columnCount(); ++column) {
                const QStandardItem* item = model->item(row, column);
                if (item
                    && item->text().contains(
                        needle,
                        Qt::CaseInsensitive)) {
                    return true;
                }
            }
            return false;
        };

    if (m_orders && m_ordersModel) {
        for (int row = 0; row < m_ordersModel->rowCount(); ++row) {
            m_orders->setRowHidden(
                row,
                !matchesText(m_ordersModel, row));
        }
    }

    if (!m_ordersPage || !m_ordersPageModel) {
        return;
    }

    QVector<int> matchingRows;
    matchingRows.reserve(m_ordersPageModel->rowCount());

    for (int row = 0; row < m_ordersPageModel->rowCount(); ++row) {
        const bool textMatch =
            matchesText(m_ordersPageModel, row);
        const QStandardItem* statusItem =
            m_ordersPageModel->item(row, 4);
        const bool statusMatch =
            m_orderStatusFilter == QStringLiteral("All")
            || (statusItem
                && statusItem->text() == m_orderStatusFilter);

        if (textMatch && statusMatch) {
            matchingRows.append(row);
        }
        m_ordersPage->setRowHidden(row, true);
    }

    if (!m_ordersPagination) {
        for (int row : matchingRows) {
            m_ordersPage->setRowHidden(row, false);
        }
        return;
    }

    m_ordersPagination->setTotalCount(matchingRows.size());

    const int pageSize = m_ordersPagination->pageSize();
    const int pageCount =
        pageSize > 0
            ? qMax(1, (matchingRows.size() + pageSize - 1) / pageSize)
            : 1;

    if (m_ordersPagination->page() > pageCount) {
        m_ordersPagination->setPage(pageCount);
    }

    const int first =
        (m_ordersPagination->page() - 1) * pageSize;
    const int last =
        qMin(first + pageSize, matchingRows.size());

    for (int index = first; index < last; ++index) {
        m_ordersPage->setRowHidden(
            matchingRows.at(index),
            false);
    }
}

void DashboardWindow::applyThemeChrome()
{
    const auto& theme = QtMaterial::ThemeManager::instance().theme();
    const QColor background = materialColor(QtMaterial::ColorRole::Background);
    const QColor surface = materialColor(QtMaterial::ColorRole::Surface);
    const QColor surfaceVariant = materialColor(QtMaterial::ColorRole::SurfaceContainerLow);
    const QColor onSurface = materialColor(QtMaterial::ColorRole::OnSurface);
    const QColor onSurfaceVariant = materialColor(QtMaterial::ColorRole::OnSurfaceVariant);
    const QColor outline = materialColor(QtMaterial::ColorRole::OutlineVariant);
    const QColor primary = materialColor(QtMaterial::ColorRole::Primary);
    const QColor onPrimary = materialColor(QtMaterial::ColorRole::OnPrimary);
    const QColor positive = materialColor(QtMaterial::ColorRole::Tertiary);

    const QColor sidebar =
        theme.isDark()
            ? materialColor(QtMaterial::ColorRole::SurfaceContainerLow)
            : materialColor(QtMaterial::ColorRole::InverseSurface);
    const QColor sidebarText =
        theme.isDark()
            ? materialColor(QtMaterial::ColorRole::OnSurface)
            : materialColor(QtMaterial::ColorRole::InverseOnSurface);
    QColor sidebarMuted = sidebarText;
    sidebarMuted.setAlpha(165);
    QColor sidebarHover = sidebar;
    sidebarHover = theme.isDark()
        ? sidebarHover.lighter(120)
        : sidebarHover.lighter(112);
    QColor sidebarProfile = sidebar;
    sidebarProfile = theme.isDark()
        ? sidebarProfile.darker(112)
        : sidebarProfile.darker(108);

    if (m_central) {
        QPalette palette = m_central->palette();
        palette.setColor(QPalette::Window, background);
        palette.setColor(QPalette::Base, surface);
        palette.setColor(QPalette::WindowText, onSurface);
        palette.setColor(QPalette::Text, onSurface);
        m_central->setPalette(palette);
        m_central->setAutoFillBackground(true);
    }

    if (m_sidebar) {
        m_sidebar->setStyleSheet(QStringLiteral(
            "#dashboardSidebar, #dashboardNavContent { background:%1; }"
            "#dashboardBrandLogo { background:%2; color:%3; border-radius:18px; font-weight:700; }"
            "#dashboardBrandTitle, #dashboardProfileName { color:%4; }"
            "#dashboardBrandSubtitle, #dashboardProfileRole, #dashboardSectionLabel { color:%5; }"
            "QToolButton#dashboardNavButton { color:%4; background:transparent; border:0;"
            " text-align:left; padding:0 15px; border-radius:6px; font-size:12px; }"
            "QToolButton#dashboardNavButton:hover { background:%6; color:%4; }"
            "QToolButton#dashboardNavButton:checked { background:%2; color:%3; font-weight:600;"
            " border-left:3px solid %2; padding-left:12px; }"
            "#dashboardProfile { background:%7; border-top:1px solid %6; }"
            "#dashboardProfileAvatar { background:%2; color:%3; border-radius:16px; font-weight:700; }"
            "QScrollArea#dashboardNavScroll { background:transparent; border:0; }"
            "QScrollArea#dashboardNavScroll > QWidget > QWidget { background:transparent; }"
            "QScrollArea#dashboardNavScroll QScrollBar:vertical { background:%1; width:6px; margin:0; }"
            "QScrollArea#dashboardNavScroll QScrollBar::handle:vertical { background:%6;"
            " border-radius:3px; min-height:32px; }"
            "QScrollArea#dashboardNavScroll QScrollBar::add-line:vertical,"
            " QScrollArea#dashboardNavScroll QScrollBar::sub-line:vertical { height:0; }")
            .arg(cssColor(sidebar))
            .arg(cssColor(primary))
            .arg(cssColor(onPrimary))
            .arg(cssColor(sidebarText))
            .arg(cssColor(sidebarMuted))
            .arg(cssColor(sidebarHover))
            .arg(cssColor(sidebarProfile)));

        const auto navButtons =
            m_sidebar->findChildren<QToolButton*>(QStringLiteral("dashboardNavButton"));
        for (QToolButton* button : navButtons) {
            const QString iconName =
                button->property("dashboardIconName").toString();
            if (!iconName.isEmpty()) {
                button->setIcon(dashboardIcon(iconName, sidebarText));
            }
        }
    }

    if (m_navigationDrawer) {
        m_navigationDrawer->setStyleSheet(QStringLiteral(
            "#dashboardNavigationDrawer { color:%1; }"
            "QToolButton#dashboardNavButton { color:%2; background:transparent; border:0;"
            " text-align:left; padding:0 15px; border-radius:8px; font-size:13px; }"
            "QToolButton#dashboardNavButton:hover { background:%3; }"
            "QToolButton#dashboardNavButton:checked { background:%4; color:%5; font-weight:600; }")
            .arg(cssColor(onSurface))
            .arg(cssColor(onSurfaceVariant))
            .arg(cssColor(surfaceVariant))
            .arg(cssColor(materialColor(QtMaterial::ColorRole::PrimaryContainer)))
            .arg(cssColor(materialColor(QtMaterial::ColorRole::OnPrimaryContainer))));

        const auto drawerButtons =
            m_navigationDrawer->findChildren<QToolButton*>(
                QStringLiteral("dashboardNavButton"));
        for (QToolButton* button : drawerButtons) {
            const QString iconName =
                button->property("dashboardIconName").toString();
            if (!iconName.isEmpty()) {
                button->setIcon(dashboardIcon(iconName, onSurfaceVariant));
            }
        }
    }

    if (m_contentHost) {
        QPalette palette = m_contentHost->palette();
        palette.setColor(QPalette::Window, surfaceVariant);
        palette.setColor(QPalette::Base, surfaceVariant);
        palette.setColor(QPalette::WindowText, onSurface);
        palette.setColor(QPalette::Text, onSurface);
        m_contentHost->setPalette(palette);
        m_contentHost->setAutoFillBackground(true);
    }

    if (m_scroll && m_scroll->viewport()) {
        QPalette palette = m_scroll->viewport()->palette();
        palette.setColor(QPalette::Window, surfaceVariant);
        palette.setColor(QPalette::Base, surfaceVariant);
        m_scroll->viewport()->setPalette(palette);
        m_scroll->viewport()->setAutoFillBackground(true);
    }

    if (m_topBar) {
        m_topBar->setStyleSheet(QStringLiteral(
            "#dashboardTopBar { background:%1; border-bottom:1px solid %2; }"
            "QToolButton#topBarButton { background:transparent; color:%3; border:0;"
            " border-radius:7px; font-weight:600; }"
            "QToolButton#topBarButton:hover { background:%4; }"
            "QToolButton#topBarAccount { background:transparent; color:%3; border:0;"
            " padding:0 4px; font-size:12px; }"
            "QToolButton#topBarAccount:hover { color:%5; }"
            "#dashboardTopAvatar { background:%6; color:%7; border-radius:16px;"
            " font-weight:700; font-size:10px; }")
            .arg(cssColor(surface))
            .arg(cssColor(outline))
            .arg(cssColor(onSurface))
            .arg(cssColor(surfaceVariant))
            .arg(cssColor(primary))
            .arg(cssColor(materialColor(QtMaterial::ColorRole::PrimaryContainer)))
            .arg(cssColor(materialColor(QtMaterial::ColorRole::OnPrimaryContainer))));

        const auto actionButtons = m_topBar->findChildren<QAbstractButton*>();
        for (QAbstractButton* button : actionButtons) {
            const QString iconName =
                button->property("dashboardIconName").toString();
            if (iconName.isEmpty()) {
                continue;
            }
            const int badge = button->property("dashboardBadge").isValid()
                ? button->property("dashboardBadge").toInt()
                : -1;
            button->setIcon(dashboardIcon(iconName, onSurfaceVariant, badge));
        }
    }

    if (m_themeButton) {
        m_themeButton->setIcon(dashboardIcon(
            theme.isDark()
                ? QStringLiteral("sun")
                : QStringLiteral("moon"),
            onSurfaceVariant));
    }

    if (m_search && m_search->lineEdit()) {
        m_search->lineEdit()->setStyleSheet(QStringLiteral(
            "QLineEdit { background:%1; color:%2; border:1px solid %3;"
            " border-radius:7px; padding:7px 10px; }")
            .arg(cssColor(surfaceVariant))
            .arg(cssColor(onSurface))
            .arg(cssColor(outline)));
    }

    const QString applicationChrome = QStringLiteral(
        "QToolButton#chartTab { background:transparent; border:0; color:%1;"
        " padding:5px 8px; }"
        "QToolButton#chartTab:checked { color:%2; border-bottom:2px solid %2; }"
        "QToolButton#linkButton { background:transparent; border:0; color:%2;"
        " padding:3px 5px; }"
        "QLabel#taskDetail { color:%1; }"
        "#profileHeroAvatar { background:%2; color:%6; border-radius:32px;"
        " font-weight:700; font-size:16px; }"
        "QFrame#dashboardSeparator { color:%3; }"
        "QCalendarWidget#dashboardCalendar { background:%4; border:0; }"
        "QCalendarWidget#dashboardCalendar QToolButton { color:%5; background:transparent;"
        " border:0; padding:3px; }"
        "QCalendarWidget#dashboardCalendar QSpinBox { color:%5; background:%4; border:0; }"
        "QCalendarWidget#dashboardCalendar QAbstractItemView { background:%4; color:%5;"
        " selection-background-color:%2; selection-color:%6; border:0; outline:0; }")
        .arg(cssColor(onSurfaceVariant))
        .arg(cssColor(primary))
        .arg(cssColor(outline))
        .arg(cssColor(surface))
        .arg(cssColor(onSurface))
        .arg(cssColor(onPrimary));

    if (m_pages) {
        const QString pageStyle = QStringLiteral(
            "QWidget#dashboardContent { background:%1; color:%2; }"
            "QLabel#pageSubtitle { color:%3; }")
            .arg(cssColor(surfaceVariant))
            .arg(cssColor(onSurface))
            .arg(cssColor(onSurfaceVariant));
        m_pages->setStyleSheet(pageStyle + applicationChrome);

        QPalette pagesPalette = m_pages->palette();
        pagesPalette.setColor(QPalette::Window, surfaceVariant);
        pagesPalette.setColor(QPalette::Base, surfaceVariant);
        pagesPalette.setColor(QPalette::WindowText, onSurface);
        pagesPalette.setColor(QPalette::Text, onSurface);
        m_pages->setPalette(pagesPalette);
        m_pages->setAutoFillBackground(true);
    }

    if (m_pageSubtitle) {
        QPalette palette = m_pageSubtitle->palette();
        palette.setColor(QPalette::WindowText, onSurfaceVariant);
        m_pageSubtitle->setPalette(palette);
    }

    const auto metricIcons =
        m_contentHost
            ? m_contentHost->findChildren<QLabel*>(QStringLiteral("metricIcon"))
            : QList<QLabel*>();
    for (QLabel* icon : metricIcons) {
        const auto role = static_cast<QtMaterial::ColorRole>(
            icon->property("dashboardColorRole").toInt());
        const QString iconName =
            icon->property("dashboardIconName").toString();

        QtMaterial::ColorRole foregroundRole = QtMaterial::ColorRole::OnPrimary;
        if (role == QtMaterial::ColorRole::Secondary) {
            foregroundRole = QtMaterial::ColorRole::OnSecondary;
        } else if (role == QtMaterial::ColorRole::Tertiary) {
            foregroundRole = QtMaterial::ColorRole::OnTertiary;
        } else if (role == QtMaterial::ColorRole::Error) {
            foregroundRole = QtMaterial::ColorRole::OnError;
        }

        icon->setStyleSheet(QStringLiteral(
            "background:%1; border-radius:6px;")
            .arg(cssColor(materialColor(role))));
        icon->setPixmap(dashboardIcon(
            iconName,
            materialColor(foregroundRole)).pixmap(24, 24));
    }

    for (const MetricWidgets& metric : m_metrics) {
        QPalette deltaPalette = metric.delta->palette();
        deltaPalette.setColor(QPalette::WindowText, positive);
        metric.delta->setPalette(deltaPalette);
    }

    if (m_pages) {
        const auto positiveLabels =
            m_pages->findChildren<QLabel*>(QStringLiteral("positiveDelta"));
        for (QLabel* label : positiveLabels) {
            QPalette palette = label->palette();
            palette.setColor(QPalette::WindowText, positive);
            label->setPalette(palette);
        }

        const auto legendDots =
            m_pages->findChildren<QLabel*>(QStringLiteral("legendDot"));
        for (QLabel* dot : legendDots) {
            const auto role = static_cast<QtMaterial::ColorRole>(
                dot->property("dashboardColorRole").toInt());
            QPalette palette = dot->palette();
            palette.setColor(QPalette::WindowText, materialColor(role));
            dot->setPalette(palette);
        }
    }

    if (m_orders) {
        m_orders->viewport()->update();
    }
    if (m_ordersPage) {
        m_ordersPage->viewport()->update();
    }
    if (m_revenueSummary) {
        m_revenueSummary->update();
    }
    if (m_contentHost) {
        const auto bars =
            m_contentHost->findChildren<QWidget*>(
                QStringLiteral("dashboardSocialBars"));
        for (QWidget* bar : bars) {
            bar->update();
        }
    }
}

void DashboardWindow::updateResponsiveLayout()
{
    if (!m_quickGrid || !m_chartGrid || !m_scroll) {
        return;
    }

    const int windowWidth = width();
    const int available = m_scroll->viewport()->width();

    const bool desktopNavigation = windowWidth >= 1200;
    const bool tabletNavigation =
        windowWidth >= 800 && windowWidth < 1200;
    const bool compactNavigation = windowWidth < 800;

    const bool compactMetrics = available < 980;
    const bool stackedCharts = available < 900;

    if (m_sidebar) {
        m_sidebar->setVisible(desktopNavigation);
    }
    if (m_navigationRail) {
        m_navigationRail->setVisible(tabletNavigation);
        m_navigationRail->setLabelsVisible(false);
    }
    if (m_menuButton) {
        m_menuButton->setVisible(compactNavigation);
    }
    if (m_navigationDrawer
        && !compactNavigation
        && m_navigationDrawer->isOpen()) {
        m_navigationDrawer->closeDrawer();
    }

    if (m_search) {
        m_search->setVisible(windowWidth >= 760);
    }
    if (m_breadcrumb) {
        m_breadcrumb->setVisible(windowWidth >= 700);
    }
    if (m_topBar) {
        const auto optionalWidgets = m_topBar->findChildren<QWidget*>();
        for (QWidget* widget : optionalWidgets) {
            if (widget->property("dashboardCompactOptional").toBool()) {
                widget->setVisible(windowWidth >= 980);
            }
        }
    }

    if (compactMetrics != m_compactMetrics || m_quickGrid->count() == 0) {
        for (auto* card : m_metricCards) {
            clearGridPosition(m_quickGrid, card);
        }

        for (int i = 0; i < m_metricCards.size(); ++i) {
            if (compactMetrics) {
                m_quickGrid->addWidget(m_metricCards.at(i), i, 0);
            } else {
                m_quickGrid->addWidget(m_metricCards.at(i), i / 2, i % 2);
            }
        }
        m_compactMetrics = compactMetrics;
    }

    if (stackedCharts != m_stackedCharts || m_chartGrid->count() == 0) {
        clearGridPosition(m_chartGrid, m_statisticsCard);
        clearGridPosition(m_chartGrid, m_earningsCard);

        if (stackedCharts) {
            m_chartGrid->addWidget(m_statisticsCard, 0, 0);
            m_chartGrid->addWidget(m_earningsCard, 1, 0);
        } else {
            m_chartGrid->addWidget(m_statisticsCard, 0, 0, 1, 2);
            m_chartGrid->addWidget(m_earningsCard, 0, 2);
            m_chartGrid->setColumnStretch(0, 1);
            m_chartGrid->setColumnStretch(1, 1);
            m_chartGrid->setColumnStretch(2, 1);
        }
        m_stackedCharts = stackedCharts;
    }
}

void DashboardWindow::setCurrentSection(int index)
{
    const int maximumIndex = m_pages ? m_pages->count() - 1 : m_navButtons.size() - 1;
    index = qBound(0, index, maximumIndex);

    if (index >= 0 && index < m_navButtons.size()) {
        m_navButtons.at(index)->setChecked(true);
    }
    if (m_pages) {
        m_pages->setCurrentIndex(index);
    }
    if (m_navigationRail && m_navigationRail->currentIndex() != index) {
        m_navigationRail->setCurrentIndex(index);
    }
    if (m_navigationDrawer) {
        const auto drawerButtons =
            m_navigationDrawer->findChildren<QToolButton*>(
                QStringLiteral("dashboardNavButton"));
        for (QToolButton* button : drawerButtons) {
            const int buttonIndex =
                button->property("dashboardDrawerIndex").toInt();
            button->setChecked(buttonIndex == index);
        }
    }

    static const char* titles[] = {
        "Dashboard",
        "Analytics",
        "Orders",
        "Customers",
        "Components",
        "Profile",
        "Pricing",
        "Application States",
        "Showcase Settings"
    };
    const QString title = QString::fromLatin1(titles[index]);

    if (m_breadcrumb) {
        m_breadcrumb->setItems({
            QStringLiteral("Application"),
            title
        });
        m_breadcrumb->setCurrentIndex(1);
    }
    if (m_search) {
        static const char* placeholders[] = {
            "Search dashboard...",
            "Search analytics...",
            "Search orders...",
            "Search customers...",
            "Search components...",
            "Search profile...",
            "Search pricing...",
            "Search application states...",
            "Search showcase settings..."
        };
        m_search->setPlaceholderText(QString::fromLatin1(placeholders[index]));
    }
}

void DashboardWindow::showOrderDetails(int row)
{
    showOrderDetailsForModel(row, m_ordersModel);
}

void DashboardWindow::showOrderDetailsForModel(
    int row,
    QStandardItemModel* model)
{
    if (!model || row < 0 || row >= model->rowCount()) {
        return;
    }

    auto* dialog = new QtMaterial::QtMaterialDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    dialog->setTitleText(QStringLiteral("Order %1").arg(model->item(row, 0)->text()));
    dialog->setSupportingText(
        QStringLiteral("%1 • %2 • %3 • %4")
            .arg(model->item(row, 1)->text())
            .arg(model->item(row, 2)->text())
            .arg(model->item(row, 3)->text())
            .arg(model->item(row, 4)->text()));
    dialog->open();
}

void DashboardWindow::showMessage(const QString& text)
{
    if (!m_snackbarHost) {
        return;
    }

    QtMaterial::SnackbarRequest request;
    request.text = text;
    request.duration = QtMaterial::SnackbarDuration::Short;
    request.showDismissButton = true;
    m_snackbarHost->showMessage(request, true);
}
