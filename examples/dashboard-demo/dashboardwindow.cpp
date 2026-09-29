#include "dashboardwindow.h"

#include "dashboardcharts.h"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QFont>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLinearGradient>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QProgressBar>
#include <QResizeEvent>
#include <QScrollArea>
#include <QShortcut>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>

#include <algorithm>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/inputs/qtmaterialsearchbar.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"
#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"
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

QIcon simpleIcon(const QString& glyph, const QColor& color)
{
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QFont font = painter.font();
    font.setBold(true);
    font.setPointSize(12);
    painter.setFont(font);
    painter.setPen(color);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, glyph);
    return QIcon(pixmap);
}

QToolButton* makeNavButton(
    const QString& text,
    const QString& glyph,
    QWidget* parent)
{
    auto* button = new QToolButton(parent);
    button->setText(text);
    button->setIcon(simpleIcon(glyph, QColor(QStringLiteral("#aeb3c2"))));
    button->setIconSize(QSize(22, 22));
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setCheckable(true);
    button->setAutoExclusive(true);
    button->setCursor(Qt::PointingHandCursor);
    button->setMinimumHeight(42);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    button->setStyleSheet(QStringLiteral(
        "QToolButton { color:#c7cbd7; background:transparent; border:0;"
        " text-align:left; padding:0 16px; border-radius:8px; font-size:13px; }"
        "QToolButton:hover { background:#323543; color:#ffffff; }"
        "QToolButton:checked { background:#343847; color:#ffffff; font-weight:600;"
        " border-left:3px solid #67b7ff; padding-left:13px; }"));
    return button;
}

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
        setMinimumHeight(156);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QRectF box = rect();
        box.adjust(0.5, 0.5, -0.5, -0.5);

        QLinearGradient background(box.topLeft(), box.bottomRight());
        background.setColorAt(0.0, QColor(QStringLiteral("#3346a8")));
        background.setColorAt(1.0, QColor(QStringLiteral("#293a91")));
        painter.setPen(Qt::NoPen);
        painter.setBrush(background);
        painter.drawRoundedRect(box, 10.0, 10.0);

        painter.setPen(QColor(QStringLiteral("#ffffff")));
        QFont titleFont = font();
        titleFont.setPointSizeF(titleFont.pointSizeF() + 1.0);
        titleFont.setBold(false);
        painter.setFont(titleFont);
        painter.drawText(QRectF(20, 16, width() - 40, 24), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Total Revenue"));

        QFont valueFont = font();
        valueFont.setPointSizeF(valueFont.pointSizeF() + 8.0);
        valueFont.setBold(true);
        painter.setFont(valueFont);
        painter.drawText(QRectF(20, 47, width() - 40, 34), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("€216,759"));

        QFont labelFont = font();
        labelFont.setPointSizeF(std::max<qreal>(8.0, labelFont.pointSizeF() - 1.0));
        painter.setFont(labelFont);
        painter.setPen(QColor(255, 255, 255, 175));
        painter.drawText(QRectF(20, 83, 110, 22), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("YTD Revenue"));

        painter.setPen(QColor(QStringLiteral("#ffffff")));
        QFont smallValue = font();
        smallValue.setPointSizeF(smallValue.pointSizeF() + 3.0);
        smallValue.setBold(true);
        painter.setFont(smallValue);
        painter.drawText(QRectF(20, 113, 44, 26), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("49"));
        painter.drawText(QRectF(74, 113, 44, 26), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("09"));

        painter.setPen(QColor(255, 255, 255, 150));
        painter.setFont(labelFont);
        painter.drawText(QRectF(20, 136, 44, 18), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Clients"));
        painter.drawText(QRectF(74, 136, 60, 18), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Countries"));

        if (width() < 210) {
            return;
        }

        const QRectF chart(width() * 0.43, 72, width() * 0.52, 66);
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
        painter.setPen(QPen(QColor(QStringLiteral("#ffd95a")), 3.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    }
};

QFrame* makeSectionLabel(const QString& text, QWidget* parent)
{
    auto* host = new QFrame(parent);
    auto* layout = new QHBoxLayout(host);
    layout->setContentsMargins(16, 11, 16, 4);
    auto* label = new QLabel(text.toUpper(), host);
    QFont font = label->font();
    font.setPointSizeF(std::max<qreal>(8.0, font.pointSizeF() - 1.0));
    font.setLetterSpacing(QFont::AbsoluteSpacing, 0.6);
    label->setFont(font);
    label->setStyleSheet(QStringLiteral("color:#7f8596;"));
    layout->addWidget(label);
    return host;
}

} // namespace

DashboardWindow::DashboardWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Qt Material 3 - Dashboard Showcase"));
    setMinimumSize(820, 640);

    auto options = QtMaterial::ThemeManager::instance().options();
    options.sourceColor = QColor(QStringLiteral("#4455c7"));
    QtMaterial::ThemeManager::instance().setThemeOptions(options);

    m_central = new QWidget(this);
    auto* shell = new QHBoxLayout(m_central);
    shell->setContentsMargins(0, 0, 0, 0);
    shell->setSpacing(0);

    m_sidebar = createSidebar();
    shell->addWidget(m_sidebar);

    auto* right = new QWidget(m_central);
    auto* rightLayout = new QVBoxLayout(right);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    rightLayout->addWidget(createTopBar());

    m_scroll = new QScrollArea(right);
    m_scroll->setWidgetResizable(true);
    m_scroll->setFrameShape(QFrame::NoFrame);
    m_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_contentHost = createDashboardPage();
    m_scroll->setWidget(m_contentHost);
    rightLayout->addWidget(m_scroll, 1);

    shell->addWidget(right, 1);
    setCentralWidget(m_central);

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
            if (m_themeButton) {
                m_themeButton->setText(
                    QtMaterial::ThemeManager::instance().theme().isDark()
                        ? QStringLiteral("☀")
                        : QStringLiteral("☾"));
            }
        });

    applyThemeChrome();
    applyPeriod();
    updateResponsiveLayout();
}

DashboardWindow::~DashboardWindow() = default;

void DashboardWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    updateResponsiveLayout();
}

QWidget* DashboardWindow::createSidebar()
{
    auto* sidebar = new QWidget(m_central);
    sidebar->setObjectName(QStringLiteral("dashboardSidebar"));
    sidebar->setFixedWidth(236);
    sidebar->setStyleSheet(QStringLiteral(
        "#dashboardSidebar { background:#262936; }"));

    auto* layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(0, 0, 0, 12);
    layout->setSpacing(2);

    auto* brand = new QWidget(sidebar);
    brand->setFixedHeight(78);
    auto* brandLayout = new QHBoxLayout(brand);
    brandLayout->setContentsMargins(18, 16, 14, 12);
    brandLayout->setSpacing(11);

    auto* logo = new QLabel(QStringLiteral("M3"), brand);
    logo->setAlignment(Qt::AlignCenter);
    logo->setFixedSize(38, 38);
    logo->setStyleSheet(QStringLiteral(
        "background:#56a8f5; color:white; border-radius:19px; font-weight:700;"));

    auto* brandText = new QVBoxLayout;
    brandText->setSpacing(0);
    auto* brandTitle = makeLabel(QStringLiteral("Material 3"), brand, 1.0, true);
    auto* brandSubtitle = makeLabel(QStringLiteral("Qt Widgets"), brand, -1.0, false);
    brandTitle->setStyleSheet(QStringLiteral("color:#ffffff;"));
    brandSubtitle->setStyleSheet(QStringLiteral("color:#8e95a8;"));
    brandText->addWidget(brandTitle);
    brandText->addWidget(brandSubtitle);

    brandLayout->addWidget(logo);
    brandLayout->addLayout(brandText, 1);
    layout->addWidget(brand);

    layout->addWidget(makeSectionLabel(QStringLiteral("Application"), sidebar));

    struct NavItem {
        const char* label;
        const char* glyph;
    };
    static const NavItem mainItems[] = {
        {"Dashboard", "D"},
        {"Analytics", "A"},
        {"Orders", "O"},
        {"Customers", "C"},
        {"Components", "W"}
    };

    auto* group = new QButtonGroup(sidebar);
    group->setExclusive(true);
    for (int i = 0; i < 5; ++i) {
        auto* button = makeNavButton(
            QString::fromLatin1(mainItems[i].label),
            QString::fromLatin1(mainItems[i].glyph),
            sidebar);
        group->addButton(button, i);
        m_navButtons.append(button);
        layout->addWidget(button);
        connect(button, &QToolButton::clicked, this, [this, i]() { setCurrentSection(i); });
    }
    m_navButtons.first()->setChecked(true);

    layout->addWidget(makeSectionLabel(QStringLiteral("System"), sidebar));

    auto* themeStudio = makeNavButton(QStringLiteral("Theme studio"), QStringLiteral("T"), sidebar);
    themeStudio->setCheckable(false);
    layout->addWidget(themeStudio);
    connect(themeStudio, &QToolButton::clicked, this, [this]() {
        showMessage(QStringLiteral("Theme Studio is available as a separate example."));
    });

    auto* settings = makeNavButton(QStringLiteral("Settings"), QStringLiteral("S"), sidebar);
    settings->setCheckable(false);
    layout->addWidget(settings);
    connect(settings, &QToolButton::clicked, this, [this]() { setCurrentSection(4); });

    layout->addStretch(1);

    auto* profile = new QFrame(sidebar);
    profile->setStyleSheet(QStringLiteral(
        "QFrame { background:#20232e; border-top:1px solid #343847; }"));
    auto* profileLayout = new QHBoxLayout(profile);
    profileLayout->setContentsMargins(16, 13, 16, 13);
    auto* avatar = new QLabel(QStringLiteral("Q"), profile);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setFixedSize(34, 34);
    avatar->setStyleSheet(QStringLiteral(
        "background:#4455c7; color:white; border-radius:17px; font-weight:700;"));
    auto* user = new QVBoxLayout;
    user->setSpacing(0);
    auto* name = makeLabel(QStringLiteral("QtMaterial Demo"), profile, -1.0, true);
    auto* role = makeLabel(QStringLiteral("Desktop showcase"), profile, -2.0, false);
    name->setStyleSheet(QStringLiteral("color:#ffffff;"));
    role->setStyleSheet(QStringLiteral("color:#858b9d;"));
    user->addWidget(name);
    user->addWidget(role);
    profileLayout->addWidget(avatar);
    profileLayout->addLayout(user, 1);
    layout->addWidget(profile);

    return sidebar;
}

QWidget* DashboardWindow::createTopBar()
{
    m_topBar = new QFrame(m_central);
    m_topBar->setObjectName(QStringLiteral("dashboardTopBar"));
    m_topBar->setFixedHeight(66);

    auto* layout = new QHBoxLayout(m_topBar);
    layout->setContentsMargins(22, 10, 18, 10);
    layout->setSpacing(10);

    auto* breadcrumb = makeLabel(QStringLiteral("Dashboard  /  Analytics"), m_topBar, -1.0, false);
    breadcrumb->setObjectName(QStringLiteral("dashboardBreadcrumb"));
    layout->addWidget(breadcrumb);
    layout->addStretch(1);

    m_search = new QtMaterial::QtMaterialSearchBar(m_topBar);
    m_search->setPlaceholderText(QStringLiteral("Search orders..."));
    m_search->setClearButtonVisible(true);
    m_search->setFixedWidth(240);
    m_search->setFixedHeight(40);
    layout->addWidget(m_search);

    auto* language = new QToolButton(m_topBar);
    language->setText(QStringLiteral("EN"));
    language->setCursor(Qt::PointingHandCursor);
    language->setObjectName(QStringLiteral("topBarButton"));
    language->setFixedSize(44, 38);
    layout->addWidget(language);

    auto* notify = new QToolButton(m_topBar);
    notify->setText(QStringLiteral("•"));
    notify->setToolTip(QStringLiteral("Notifications"));
    notify->setCursor(Qt::PointingHandCursor);
    notify->setObjectName(QStringLiteral("topBarButton"));
    notify->setFixedSize(40, 38);
    layout->addWidget(notify);

    m_themeButton = new QToolButton(m_topBar);
    m_themeButton->setText(
        QtMaterial::ThemeManager::instance().theme().isDark()
            ? QStringLiteral("☀")
            : QStringLiteral("☾"));
    m_themeButton->setToolTip(QStringLiteral("Toggle light/dark mode"));
    m_themeButton->setCursor(Qt::PointingHandCursor);
    m_themeButton->setObjectName(QStringLiteral("topBarButton"));
    m_themeButton->setFixedSize(40, 38);
    layout->addWidget(m_themeButton);

    connect(m_search, &QtMaterial::QtMaterialSearchBar::textChanged, this, &DashboardWindow::applyFilter);
    connect(m_themeButton, &QToolButton::clicked, this, []() {
        auto options = QtMaterial::ThemeManager::instance().options();
        options.mode = options.mode == QtMaterial::ThemeMode::Dark
            ? QtMaterial::ThemeMode::Light
            : QtMaterial::ThemeMode::Dark;
        QtMaterial::ThemeManager::instance().setThemeOptions(options);
    });
    connect(notify, &QToolButton::clicked, this, [this]() {
        showMessage(QStringLiteral("No new notifications."));
    });

    return m_topBar;
}

QWidget* DashboardWindow::createDashboardPage()
{
    auto* page = new QWidget;
    page->setObjectName(QStringLiteral("dashboardContent"));
    m_contentHost = page;

    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(26, 22, 26, 32);
    layout->setSpacing(18);

    auto* titleRow = new QWidget(page);
    auto* titleLayout = new QHBoxLayout(titleRow);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(10);

    auto* titleText = new QVBoxLayout;
    titleText->setSpacing(2);
    m_pageTitle = makeLabel(QStringLiteral("Dashboard"), titleRow, 7.0, true);
    m_pageSubtitle = makeLabel(
        QStringLiteral("A polished application-style showcase for Qt Material 3 widgets."),
        titleRow,
        -1.0,
        false);
    titleText->addWidget(m_pageTitle);
    titleText->addWidget(m_pageSubtitle);
    titleLayout->addLayout(titleText, 1);

    m_yearCombo = new QComboBox(titleRow);
    m_yearCombo->addItems({QStringLiteral("2025"), QStringLiteral("2026"), QStringLiteral("2027")});
    m_yearCombo->setCurrentText(QStringLiteral("2026"));
    m_yearCombo->setObjectName(QStringLiteral("dashboardCombo"));
    m_yearCombo->setFixedWidth(90);
    titleLayout->addWidget(m_yearCombo);

    m_monthCombo = new QComboBox(titleRow);
    m_monthCombo->addItems({
        QStringLiteral("January"),
        QStringLiteral("March"),
        QStringLiteral("June"),
        QStringLiteral("September"),
        QStringLiteral("December")
    });
    m_monthCombo->setCurrentText(QStringLiteral("September"));
    m_monthCombo->setObjectName(QStringLiteral("dashboardCombo"));
    m_monthCombo->setFixedWidth(118);
    titleLayout->addWidget(m_monthCombo);

    layout->addWidget(titleRow);

    auto* sectionHeader = new QHBoxLayout;
    sectionHeader->addWidget(makeLabel(QStringLiteral("Quick Statistics"), page, 2.0, false));
    sectionHeader->addStretch(1);
    layout->addLayout(sectionHeader);

    layout->addWidget(createQuickStatistics());

    auto* chartHeader = new QHBoxLayout;
    chartHeader->addWidget(makeLabel(QStringLiteral("Statistics"), page, 2.0, false));
    chartHeader->addStretch(1);
    layout->addLayout(chartHeader);

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
        QColor(QStringLiteral("#ef655b")),
        QStringLiteral("C")));
    m_metrics.append(createMetricCard(
        QStringLiteral("Paid Invoices"),
        QStringLiteral("€10,600"),
        QStringLiteral("+12.4%"),
        QColor(QStringLiteral("#4898e8")),
        QStringLiteral("€")));
    m_metrics.append(createMetricCard(
        QStringLiteral("Total Projects"),
        QStringLiteral("73"),
        QStringLiteral("+5.1%"),
        QColor(QStringLiteral("#505bc4")),
        QStringLiteral("P")));
    m_metrics.append(createMetricCard(
        QStringLiteral("Open Projects"),
        QStringLiteral("33"),
        QStringLiteral("+3.7%"),
        QColor(QStringLiteral("#41a094")),
        QStringLiteral("O")));

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
    const QColor& iconColor,
    const QString& iconText)
{
    MetricWidgets metric;
    metric.card = new QtMaterial::QtMaterialCard(m_contentHost);
    metric.card->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    metric.card->setMinimumHeight(104);

    auto* row = new QHBoxLayout(metric.card);
    row->setContentsMargins(12, 12, 16, 12);
    row->setSpacing(12);

    auto* icon = new QLabel(iconText, metric.card);
    icon->setAlignment(Qt::AlignCenter);
    icon->setFixedSize(48, 48);
    icon->setStyleSheet(QStringLiteral(
        "background:%1; color:white; border-radius:6px; font-size:17px; font-weight:700;")
        .arg(cssColor(iconColor)));
    row->addWidget(icon);

    auto* text = new QVBoxLayout;
    text->setSpacing(1);
    auto* titleLabel = makeLabel(title, metric.card, -1.0, false);
    titleLabel->setObjectName(QStringLiteral("metricTitle"));
    metric.value = makeLabel(value, metric.card, 4.0, false);
    metric.delta = makeLabel(delta, metric.card, -2.0, false);
    metric.delta->setProperty("dashboardPositive", true);
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
    card->setMinimumHeight(352);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 16, 18, 14);
    layout->setSpacing(8);

    auto* titleRow = new QHBoxLayout;
    titleRow->addWidget(makeLabel(QStringLiteral("Statistics"), card, 2.0, false));
    titleRow->addStretch(1);

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
    }
    layout->addLayout(titleRow);

    m_lineChart = new LineChartWidget(card);
    m_lineChart->setAccentColor(QColor(QStringLiteral("#ee6c63")));
    layout->addWidget(m_lineChart, 1);
    return card;
}

QtMaterial::QtMaterialCard* DashboardWindow::createEarningsCard()
{
    auto* card = new QtMaterial::QtMaterialCard(m_contentHost);
    card->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    card->setMinimumHeight(352);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 16, 18, 16);
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
        const char* color;
    } legend[] = {
        {"Earning:", "€18,756", "#e65146"},
        {"Pending:", "€5,599", "#49a36f"},
        {"Refund:", "€4,987", "#3c8dcc"}
    };

    for (const auto& item : legend) {
        auto* row = new QHBoxLayout;
        auto* dot = new QLabel(QStringLiteral("●"), card);
        dot->setStyleSheet(QStringLiteral("color:%1;").arg(QString::fromLatin1(item.color)));
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
    social->setMinimumHeight(210);
    auto* socialLayout = new QVBoxLayout(social);
    socialLayout->setContentsMargins(18, 16, 18, 16);
    socialLayout->addWidget(makeLabel(QStringLiteral("Social Media Advertising"), social, 2.0, false));

    const struct {
        const char* label;
        int value;
        const char* color;
    } channels[] = {
        {"Search", 82, "#5359bd"},
        {"Social", 42, "#4aa2df"},
        {"Referral", 68, "#ef6c63"}
    };
    for (const auto& channel : channels) {
        auto* row = new QHBoxLayout;
        row->addWidget(makeLabel(QString::fromLatin1(channel.label), social, -1.0, false));
        auto* progress = new QProgressBar(social);
        progress->setRange(0, 100);
        progress->setValue(channel.value);
        progress->setTextVisible(false);
        progress->setFixedHeight(10);
        progress->setStyleSheet(QStringLiteral(
            "QProgressBar { background:#e9ebf2; border:0; border-radius:5px; }"
            "QProgressBar::chunk { background:%1; border-radius:5px; }")
            .arg(QString::fromLatin1(channel.color)));
        row->addWidget(progress, 1);
        row->addWidget(makeLabel(QStringLiteral("%1%").arg(channel.value), social, -1.0, true));
        socialLayout->addLayout(row);
    }
    socialLayout->addStretch(1);

    auto* tasks = new QtMaterial::QtMaterialCard(host);
    tasks->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    tasks->setMinimumHeight(210);
    auto* tasksLayout = new QVBoxLayout(tasks);
    tasksLayout->setContentsMargins(18, 16, 18, 16);
    auto* tasksHeader = new QHBoxLayout;
    tasksHeader->addWidget(makeLabel(QStringLiteral("Today's Tasks"), tasks, 2.0, false));
    tasksHeader->addStretch(1);
    auto* viewAll = new QToolButton(tasks);
    viewAll->setText(QStringLiteral("View all"));
    viewAll->setObjectName(QStringLiteral("linkButton"));
    tasksHeader->addWidget(viewAll);
    tasksLayout->addLayout(tasksHeader);

    const QStringList taskNames = {
        QStringLiteral("Send the billing agreement"),
        QStringLiteral("Send over the documentation"),
        QStringLiteral("Review dashboard accessibility")
    };
    for (int i = 0; i < taskNames.size(); ++i) {
        auto* check = new QtMaterial::QtMaterialCheckbox(tasks);
        check->setText(taskNames.at(i));
        if (i == 2) {
            check->setCheckState(Qt::Checked);
        }
        tasksLayout->addWidget(check);
    }
    tasksLayout->addStretch(1);

    connect(viewAll, &QToolButton::clicked, this, [this]() {
        showMessage(QStringLiteral("All tasks are already visible in this demo."));
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

    QVector<qreal> values = {
        42.0, 70.0, 50.0, 96.0, 66.0, 118.0,
        90.0, 138.0, 112.0, 158.0, 132.0, 174.0
    };
    for (qreal& value : values) {
        value += delta;
    }
    m_lineChart->setValues(values);

    if (m_donutChart) {
        m_donutChart->setValue(qBound(55, 62 + delta / 2, 78));
    }
}

void DashboardWindow::applyFilter(const QString& text)
{
    if (!m_orders || !m_ordersModel) {
        return;
    }

    const QString needle = text.trimmed();
    for (int row = 0; row < m_ordersModel->rowCount(); ++row) {
        bool match = needle.isEmpty();
        for (int column = 0; !match && column < m_ordersModel->columnCount(); ++column) {
            const QStandardItem* item = m_ordersModel->item(row, column);
            match = item && item->text().contains(needle, Qt::CaseInsensitive);
        }
        m_orders->setRowHidden(row, !match);
    }
}

void DashboardWindow::applyThemeChrome()
{
    const QColor background = materialColor(QtMaterial::ColorRole::Background);
    const QColor surface = materialColor(QtMaterial::ColorRole::Surface);
    const QColor surfaceVariant = materialColor(QtMaterial::ColorRole::SurfaceContainerLow);
    const QColor onSurface = materialColor(QtMaterial::ColorRole::OnSurface);
    const QColor onSurfaceVariant = materialColor(QtMaterial::ColorRole::OnSurfaceVariant);
    const QColor outline = materialColor(QtMaterial::ColorRole::OutlineVariant);
    const QColor positive = QColor(QStringLiteral("#3c9a63"));

    if (m_central) {
        QPalette palette = m_central->palette();
        palette.setColor(QPalette::Window, background);
        palette.setColor(QPalette::Base, surface);
        palette.setColor(QPalette::WindowText, onSurface);
        palette.setColor(QPalette::Text, onSurface);
        m_central->setPalette(palette);
        m_central->setAutoFillBackground(true);
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
            "#dashboardBreadcrumb { color:%3; }"
            "QToolButton#topBarButton { background:transparent; color:%4; border:0;"
            " border-radius:7px; font-weight:600; }"
            "QToolButton#topBarButton:hover { background:%5; }")
            .arg(cssColor(surface))
            .arg(cssColor(outline))
            .arg(cssColor(onSurfaceVariant))
            .arg(cssColor(onSurface))
            .arg(cssColor(surfaceVariant)));
    }

    if (m_search && m_search->lineEdit()) {
        m_search->lineEdit()->setStyleSheet(QStringLiteral(
            "QLineEdit { background:%1; color:%2; border:1px solid %3;"
            " border-radius:7px; padding:7px 10px; }")
            .arg(cssColor(surfaceVariant))
            .arg(cssColor(onSurface))
            .arg(cssColor(outline)));
    }

    const QString comboStyle = QStringLiteral(
        "QComboBox#dashboardCombo { background:%1; color:%2; border:1px solid %3;"
        " border-radius:6px; padding:6px 10px; }"
        "QComboBox#dashboardCombo::drop-down { border:0; width:18px; }"
        "QToolButton#chartTab { background:transparent; border:0; color:%4;"
        " padding:6px 8px; }"
        "QToolButton#chartTab:checked { color:%5; border-bottom:2px solid %5; }"
        "QToolButton#linkButton { background:transparent; border:0; color:%5; }"
        "QFrame#dashboardSeparator { color:%3; }")
        .arg(cssColor(surface))
        .arg(cssColor(onSurface))
        .arg(cssColor(outline))
        .arg(cssColor(onSurfaceVariant))
        .arg(cssColor(materialColor(QtMaterial::ColorRole::Primary)));
    if (m_contentHost) {
        m_contentHost->setStyleSheet(comboStyle);
    }

    if (m_pageSubtitle) {
        QPalette palette = m_pageSubtitle->palette();
        palette.setColor(QPalette::WindowText, onSurfaceVariant);
        m_pageSubtitle->setPalette(palette);
    }

    for (const MetricWidgets& metric : m_metrics) {
        QPalette deltaPalette = metric.delta->palette();
        deltaPalette.setColor(QPalette::WindowText, positive);
        metric.delta->setPalette(deltaPalette);
    }
}

void DashboardWindow::updateResponsiveLayout()
{
    if (!m_quickGrid || !m_chartGrid || !m_scroll) {
        return;
    }

    const int available = m_scroll->viewport()->width();
    const bool compactMetrics = available < 980;
    const bool stackedCharts = available < 900;

    if (m_sidebar) {
        m_sidebar->setVisible(width() >= 920);
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
    index = qBound(0, index, m_navButtons.size() - 1);
    if (index >= 0 && index < m_navButtons.size()) {
        m_navButtons.at(index)->setChecked(true);
    }

    static const char* titles[] = {
        "Dashboard", "Analytics", "Orders", "Customers", "Components"
    };
    const QString title = QString::fromLatin1(titles[index]);
    if (m_pageTitle) {
        m_pageTitle->setText(title);
    }
    if (index != 0) {
        showMessage(QStringLiteral("%1 selected - dashboard data remains visible in the showcase.").arg(title));
    }
}

void DashboardWindow::showOrderDetails(int row)
{
    if (!m_ordersModel || row < 0 || row >= m_ordersModel->rowCount()) {
        return;
    }

    auto* dialog = new QtMaterial::QtMaterialDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    dialog->setTitleText(QStringLiteral("Order %1").arg(m_ordersModel->item(row, 0)->text()));
    dialog->setSupportingText(
        QStringLiteral("%1 • %2 • %3 • %4")
            .arg(m_ordersModel->item(row, 1)->text())
            .arg(m_ordersModel->item(row, 2)->text())
            .arg(m_ordersModel->item(row, 3)->text())
            .arg(m_ordersModel->item(row, 4)->text()));
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
