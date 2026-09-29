#include "dashboardwindow.h"

#include "dashboardcharts.h"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QApplication>
#include <QFont>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QPalette>
#include <QResizeEvent>
#include <QScrollArea>
#include <QShortcut>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStyle>
#include <QVBoxLayout>

#include <algorithm>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/inputs/qtmaterialsearchbar.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"
#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"
#include "qtmaterial/widgets/surfaces/qtmaterialsnackbarhost.h"
#include "qtmaterial/widgets/surfaces/qtmaterialtopappbar.h"

namespace {

QColor materialColor(QtMaterial::ColorRole role)
{
    return QtMaterial::ThemeManager::instance().theme().colorScheme().color(role);
}

QLabel* makeLabel(const QString& text, QWidget* parent, qreal pointDelta = 0.0, bool bold = false)
{
    auto* label = new QLabel(text, parent);
    QFont font = label->font();
    font.setPointSizeF(std::max<qreal>(8.0, font.pointSizeF() + pointDelta));
    font.setBold(bold);
    label->setFont(font);
    return label;
}

void clearGridPosition(QGridLayout* layout, QWidget* widget)
{
    if (layout && widget) {
        layout->removeWidget(widget);
    }
}

} // namespace

DashboardWindow::DashboardWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Qt Material 3 - Analytics Dashboard"));
    setMinimumSize(760, 620);

    m_central = new QWidget(this);
    auto* shell = new QVBoxLayout(m_central);
    shell->setContentsMargins(0, 0, 0, 0);
    shell->setSpacing(0);

    auto* appBar = new QtMaterialTopAppBar(QStringLiteral("Material Analytics"), m_central);
    appBar->setElevated(true);
    appBar->setNavigationIcon(style()->standardIcon(QStyle::SP_DesktopIcon));
    appBar->setNavigationAccessibleName(QStringLiteral("Dashboard home"));
    shell->addWidget(appBar);

    auto* body = new QWidget(m_central);
    auto* bodyLayout = new QHBoxLayout(body);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(0);

    m_navigation = new QtMaterial::QtMaterialNavigationRail(body);
    m_navigation->addDestination(QStringLiteral("Overview"), style()->standardIcon(QStyle::SP_ComputerIcon));
    m_navigation->addDestination(QStringLiteral("Analytics"), style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    m_navigation->addDestination(QStringLiteral("Orders"), style()->standardIcon(QStyle::SP_FileDialogListView));
    m_navigation->addDestination(QStringLiteral("Customers"), style()->standardIcon(QStyle::SP_DirHomeIcon));
    m_navigation->addDestination(QStringLiteral("Settings"), style()->standardIcon(QStyle::SP_FileDialogContentsView));
    m_navigation->setCurrentIndex(0);
    bodyLayout->addWidget(m_navigation);

    m_scroll = new QScrollArea(body);
    m_scroll->setWidgetResizable(true);
    m_scroll->setFrameShape(QFrame::NoFrame);
    m_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_contentHost = createOverviewPage();
    m_scroll->setWidget(m_contentHost);
    bodyLayout->addWidget(m_scroll, 1);

    shell->addWidget(body, 1);
    setCentralWidget(m_central);

    m_snackbarHost = new QtMaterial::QtMaterialSnackbarHost(m_central, this);
    m_commandPalette = new QtMaterial::QtMaterialCommandPalette(this);
    populateCommandPalette();

    auto* commandShortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+K")), this);
    connect(commandShortcut, &QShortcut::activated, m_commandPalette, &QDialog::open);

    connect(
        m_navigation,
        &QtMaterial::QtMaterialNavigationRail::currentIndexChanged,
        this,
        [this](int index) {
            static const char* titles[] = {
                "Overview", "Analytics", "Orders", "Customers", "Settings"
            };
            const QString title = QString::fromLatin1(titles[qBound(0, index, 4)]);
            m_pageTitle->setText(title);
            if (index != 0) {
                showMessage(QStringLiteral("%1 selected - overview data kept visible for this demo.").arg(title));
            }
        });

    connect(
        m_commandPalette,
        &QtMaterial::QtMaterialCommandPalette::commandActivated,
        this,
        [this](const QModelIndex& index) {
            if (!index.isValid()) {
                return;
            }
            const int destination = index.row();
            if (destination >= 0 && destination < m_navigation->count()) {
                m_navigation->setCurrentIndex(destination);
            }
        });

    connect(
        &QtMaterial::ThemeManager::instance(),
        &QtMaterial::ThemeManager::themeChanged,
        this,
        [this](const QtMaterial::Theme&) { applyThemeChrome(); });

    applyThemeChrome();
    applyPeriod(1);
    updateResponsiveLayout();
}

DashboardWindow::~DashboardWindow() = default;

void DashboardWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    updateResponsiveLayout();
}

QWidget* DashboardWindow::createHeader()
{
    auto* header = new QWidget(m_contentHost);
    auto* layout = new QHBoxLayout(header);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(16);

    auto* textColumn = new QVBoxLayout;
    textColumn->setSpacing(2);
    m_pageTitle = makeLabel(QStringLiteral("Overview"), header, 9.0, true);
    m_pageSubtitle = makeLabel(QStringLiteral("A real-world Material 3 dashboard built entirely with Qt Widgets."), header, 0.0, false);
    textColumn->addWidget(m_pageTitle);
    textColumn->addWidget(m_pageSubtitle);
    layout->addLayout(textColumn, 1);

    m_search = new QtMaterial::QtMaterialSearchBar(header);
    m_search->setPlaceholderText(QStringLiteral("Search orders..."));
    m_search->setClearButtonVisible(true);
    m_search->setMinimumWidth(240);
    layout->addWidget(m_search);

    auto* themeSwitch = new QtMaterial::QtMaterialSegmentedButton(header);
    themeSwitch->addSegment(QStringLiteral("Light"));
    themeSwitch->addSegment(QStringLiteral("Dark"));
    themeSwitch->setCurrentIndex(QtMaterial::ThemeManager::instance().theme().isDark() ? 1 : 0);
    layout->addWidget(themeSwitch);

    connect(m_search, &QtMaterial::QtMaterialSearchBar::textChanged, this, &DashboardWindow::applyFilter);
    connect(
        themeSwitch,
        &QtMaterial::QtMaterialSegmentedButton::currentIndexChanged,
        this,
        [](int index) {
            auto options = QtMaterial::ThemeManager::instance().options();
            options.mode = index == 0 ? QtMaterial::ThemeMode::Light : QtMaterial::ThemeMode::Dark;
            QtMaterial::ThemeManager::instance().setThemeOptions(options);
        });

    return header;
}

QWidget* DashboardWindow::createOverviewPage()
{
    auto* page = new QWidget;
    m_contentHost = page;
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(28, 24, 28, 32);
    layout->setSpacing(20);

    layout->addWidget(createHeader());

    auto* metricRowHeader = new QWidget(page);
    auto* metricHeaderLayout = new QHBoxLayout(metricRowHeader);
    metricHeaderLayout->setContentsMargins(0, 0, 0, 0);
    metricHeaderLayout->addWidget(makeLabel(QStringLiteral("Quick statistics"), metricRowHeader, 2.0, true));
    metricHeaderLayout->addStretch(1);

    m_period = new QtMaterial::QtMaterialSegmentedButton(metricRowHeader);
    m_period->addSegment(QStringLiteral("7 days"));
    m_period->addSegment(QStringLiteral("30 days"));
    m_period->addSegment(QStringLiteral("Year"));
    m_period->setCurrentIndex(1);
    metricHeaderLayout->addWidget(m_period);
    layout->addWidget(metricRowHeader);

    auto* metricHost = new QWidget(page);
    m_metricGrid = new QGridLayout(metricHost);
    m_metricGrid->setContentsMargins(0, 0, 0, 0);
    m_metricGrid->setHorizontalSpacing(16);
    m_metricGrid->setVerticalSpacing(16);

    m_metrics.append(createMetricCard(QStringLiteral("Monthly revenue"), QStringLiteral("€120,728"), QStringLiteral("+12.4%"), true));
    m_metrics.append(createMetricCard(QStringLiteral("Net revenue"), QStringLiteral("€100,601"), QStringLiteral("+8.6%"), true));
    m_metrics.append(createMetricCard(QStringLiteral("Orders"), QStringLiteral("1,482"), QStringLiteral("+4.2%"), true));
    m_metrics.append(createMetricCard(QStringLiteral("Conversion"), QStringLiteral("12.8%"), QStringLiteral("-0.7%"), false));
    for (const MetricWidgets& metric : m_metrics) {
        m_metricCards.append(metric.card);
    }
    layout->addWidget(metricHost);

    auto* chartsHost = new QWidget(page);
    m_chartGrid = new QGridLayout(chartsHost);
    m_chartGrid->setContentsMargins(0, 0, 0, 0);
    m_chartGrid->setHorizontalSpacing(16);
    m_chartGrid->setVerticalSpacing(16);
    m_revenueCard = createRevenueCard();
    m_trafficCard = createTrafficCard();
    layout->addWidget(chartsHost);

    layout->addWidget(createOrdersCard());
    layout->addStretch(1);

    connect(m_period, &QtMaterial::QtMaterialSegmentedButton::currentIndexChanged, this, &DashboardWindow::applyPeriod);
    return page;
}

DashboardWindow::MetricWidgets DashboardWindow::createMetricCard(
    const QString& title,
    const QString& value,
    const QString& delta,
    bool positive)
{
    MetricWidgets metric;
    metric.card = new QtMaterial::QtMaterialCard(m_contentHost);
    metric.card->setVariant(QtMaterial::QtMaterialCard::Variant::Filled);
    metric.card->setMinimumHeight(138);

    auto* layout = new QVBoxLayout(metric.card);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(6);
    layout->addWidget(makeLabel(title, metric.card, 0.0, false));
    metric.value = makeLabel(value, metric.card, 10.0, true);
    layout->addWidget(metric.value);
    metric.delta = makeLabel(delta + QStringLiteral(" in selected period"), metric.card, -1.0, true);
    metric.delta->setProperty("dashboardPositive", positive);
    layout->addWidget(metric.delta);
    layout->addStretch(1);
    return metric;
}

QtMaterial::QtMaterialCard* DashboardWindow::createRevenueCard()
{
    auto* card = new QtMaterial::QtMaterialCard(m_contentHost);
    card->setVariant(QtMaterial::QtMaterialCard::Variant::Elevated);
    card->setMinimumHeight(330);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 18, 20, 16);
    layout->setSpacing(8);

    auto* titleRow = new QHBoxLayout;
    titleRow->addWidget(makeLabel(QStringLiteral("Revenue trend"), card, 2.0, true));
    titleRow->addStretch(1);
    auto* chip = new QtMaterial::QtMaterialChip(QStringLiteral("Live"), card);
    chip->setVariant(QtMaterial::ChipVariant::Assist);
    titleRow->addWidget(chip);
    layout->addLayout(titleRow);

    layout->addWidget(makeLabel(QStringLiteral("Monthly recurring revenue"), card, -1.0, false));
    m_lineChart = new LineChartWidget(card);
    layout->addWidget(m_lineChart, 1);
    return card;
}

QtMaterial::QtMaterialCard* DashboardWindow::createTrafficCard()
{
    auto* card = new QtMaterial::QtMaterialCard(m_contentHost);
    card->setVariant(QtMaterial::QtMaterialCard::Variant::Outlined);
    card->setMinimumHeight(330);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 18, 20, 18);
    layout->setSpacing(8);
    layout->addWidget(makeLabel(QStringLiteral("Traffic sources"), card, 2.0, true));
    layout->addWidget(makeLabel(QStringLiteral("Desktop share"), card, -1.0, false));
    m_donutChart = new DonutChartWidget(card);
    layout->addWidget(m_donutChart, 1);

    auto* legend = new QLabel(QStringLiteral("Desktop 67%  •  Mobile 24%  •  Tablet 9%"), card);
    legend->setAlignment(Qt::AlignCenter);
    legend->setWordWrap(true);
    layout->addWidget(legend);
    return card;
}

QtMaterial::QtMaterialCard* DashboardWindow::createOrdersCard()
{
    auto* card = new QtMaterial::QtMaterialCard(m_contentHost);
    card->setVariant(QtMaterial::QtMaterialCard::Variant::Filled);
    card->setMinimumHeight(360);

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 18, 20, 18);
    layout->setSpacing(12);

    auto* header = new QHBoxLayout;
    header->addWidget(makeLabel(QStringLiteral("Recent orders"), card, 2.0, true));
    header->addStretch(1);
    auto* exportButton = new QtMaterial::QtMaterialFilledTonalButton(QStringLiteral("Export report"), card);
    header->addWidget(exportButton);
    layout->addLayout(header);

    m_orders = new QtMaterial::QtMaterialTable(card);
    m_orders->setDense(false);
    m_orders->setAlternatingRowColors(false);
    m_orders->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_orders->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_orders->horizontalHeader()->setStretchLastSection(true);
    m_orders->verticalHeader()->setVisible(false);
    m_orders->setMinimumHeight(250);
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
    model->appendRow(new QStandardItem(QStringLiteral("Open overview")));
    model->appendRow(new QStandardItem(QStringLiteral("Open analytics")));
    model->appendRow(new QStandardItem(QStringLiteral("Open orders")));
    model->appendRow(new QStandardItem(QStringLiteral("Open customers")));
    model->appendRow(new QStandardItem(QStringLiteral("Open settings")));
    m_commandPalette->setSourceModel(model);
}

void DashboardWindow::applyPeriod(int index)
{
    const int bounded = qBound(0, index, 2);
    static const QString revenue[] = {
        QStringLiteral("€31,420"), QStringLiteral("€120,728"), QStringLiteral("€1.42M")
    };
    static const QString net[] = {
        QStringLiteral("€26,118"), QStringLiteral("€100,601"), QStringLiteral("€1.19M")
    };
    static const QString orders[] = {
        QStringLiteral("386"), QStringLiteral("1,482"), QStringLiteral("17,903")
    };
    static const QString conversion[] = {
        QStringLiteral("13.6%"), QStringLiteral("12.8%"), QStringLiteral("14.1%")
    };

    const QString values[] = {revenue[bounded], net[bounded], orders[bounded], conversion[bounded]};
    for (int i = 0; i < m_metrics.size(); ++i) {
        m_metrics[i].value->setText(values[i]);
    }

    static const QVector<qreal> series[] = {
        {78, 86, 82, 96, 91, 108, 104, 116, 112, 127, 121, 136},
        {42, 58, 51, 76, 68, 92, 83, 111, 99, 128, 118, 142},
        {35, 52, 64, 73, 88, 97, 112, 121, 134, 151, 163, 182}
    };
    m_lineChart->setValues(series[bounded]);
    m_donutChart->setValue(bounded == 0 ? 63 : (bounded == 1 ? 67 : 71));
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
    const QColor surface = materialColor(QtMaterial::ColorRole::Surface);
    const QColor onSurface = materialColor(QtMaterial::ColorRole::OnSurface);
    const QColor onSurfaceVariant = materialColor(QtMaterial::ColorRole::OnSurfaceVariant);
    const QColor positive = materialColor(QtMaterial::ColorRole::Tertiary);
    const QColor negative = materialColor(QtMaterial::ColorRole::Error);

    QPalette palette = m_central->palette();
    palette.setColor(QPalette::Window, surface);
    palette.setColor(QPalette::Base, surface);
    palette.setColor(QPalette::WindowText, onSurface);
    palette.setColor(QPalette::Text, onSurface);
    palette.setColor(QPalette::ButtonText, onSurface);
    m_central->setPalette(palette);
    m_central->setAutoFillBackground(true);

    if (m_contentHost) {
        m_contentHost->setPalette(palette);
        m_contentHost->setAutoFillBackground(true);
    }
    if (m_scroll && m_scroll->viewport()) {
        m_scroll->viewport()->setPalette(palette);
        m_scroll->viewport()->setAutoFillBackground(true);
    }

    if (m_pageSubtitle) {
        QPalette subtitlePalette = m_pageSubtitle->palette();
        subtitlePalette.setColor(QPalette::WindowText, onSurfaceVariant);
        m_pageSubtitle->setPalette(subtitlePalette);
    }

    for (const MetricWidgets& metric : m_metrics) {
        QPalette deltaPalette = metric.delta->palette();
        const bool isPositive = metric.delta->property("dashboardPositive").toBool();
        deltaPalette.setColor(QPalette::WindowText, isPositive ? positive : negative);
        metric.delta->setPalette(deltaPalette);
    }

    update();
}

void DashboardWindow::updateResponsiveLayout()
{
    if (!m_metricGrid || !m_chartGrid) {
        return;
    }

    const int available = m_scroll ? m_scroll->viewport()->width() : width();
    const bool compactMetrics = available < 980;
    const bool stackedCharts = available < 860;

    if (compactMetrics != m_compactMetrics || m_metricGrid->count() == 0) {
        for (auto* card : m_metricCards) {
            clearGridPosition(m_metricGrid, card);
        }
        if (compactMetrics) {
            for (int i = 0; i < m_metricCards.size(); ++i) {
                m_metricGrid->addWidget(m_metricCards.at(i), i / 2, i % 2);
            }
        } else {
            for (int i = 0; i < m_metricCards.size(); ++i) {
                m_metricGrid->addWidget(m_metricCards.at(i), 0, i);
            }
        }
        m_compactMetrics = compactMetrics;
    }

    if (stackedCharts != m_stackedCharts || m_chartGrid->count() == 0) {
        clearGridPosition(m_chartGrid, m_revenueCard);
        clearGridPosition(m_chartGrid, m_trafficCard);
        if (stackedCharts) {
            m_chartGrid->addWidget(m_revenueCard, 0, 0);
            m_chartGrid->addWidget(m_trafficCard, 1, 0);
        } else {
            m_chartGrid->addWidget(m_revenueCard, 0, 0, 1, 2);
            m_chartGrid->addWidget(m_trafficCard, 0, 2);
            m_chartGrid->setColumnStretch(0, 1);
            m_chartGrid->setColumnStretch(1, 1);
            m_chartGrid->setColumnStretch(2, 1);
        }
        m_stackedCharts = stackedCharts;
    }

    m_navigation->setLabelsVisible(available >= 1040);
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
