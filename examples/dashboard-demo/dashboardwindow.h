#pragma once

#include <QMainWindow>
#include <QVector>

class QLabel;
class QColor;
class QGridLayout;
class QStandardItemModel;
class QScrollArea;
class QFrame;
class QToolButton;
class QComboBox;
class QStackedWidget;

namespace QtMaterial {
class QtMaterialCard;
class QtMaterialCommandPalette;
class QtMaterialSearchBar;
class QtMaterialSnackbarHost;
class QtMaterialTable;
}

class DonutChartWidget;
class LineChartWidget;

class DashboardWindow final : public QMainWindow
{
    Q_OBJECT
public:
    explicit DashboardWindow(QWidget* parent = nullptr);
    ~DashboardWindow() override;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    struct MetricWidgets {
        QtMaterial::QtMaterialCard* card = nullptr;
        QLabel* value = nullptr;
        QLabel* delta = nullptr;
    };

    QWidget* createSidebar();
    QWidget* createTopBar();
    QWidget* createDashboardPage();
    QWidget* createAnalyticsPage();
    QWidget* createOrdersPage();
    QWidget* createCustomersPage();
    QWidget* createComponentsPage();
    QWidget* createQuickStatistics();
    QWidget* createLowerHighlights();
    MetricWidgets createMetricCard(
        const QString& title,
        const QString& value,
        const QString& delta,
        const QColor& iconColor,
        const QString& iconText);
    QWidget* createRevenueSummary();
    QtMaterial::QtMaterialCard* createStatisticsCard();
    QtMaterial::QtMaterialCard* createEarningsCard();
    QtMaterial::QtMaterialCard* createOrdersCard();

    void populateOrders();
    void populateCommandPalette();
    void applyPeriod();
    void applyFilter(const QString& text);
    void applyThemeChrome();
    void updateResponsiveLayout();
    void setCurrentSection(int index);
    void showOrderDetails(int row);
    void showMessage(const QString& text);

    QWidget* m_central = nullptr;
    QWidget* m_sidebar = nullptr;
    QWidget* m_contentHost = nullptr;
    QFrame* m_topBar = nullptr;
    QScrollArea* m_scroll = nullptr;
    QStackedWidget* m_pages = nullptr;
    QtMaterial::QtMaterialSearchBar* m_search = nullptr;
    QtMaterial::QtMaterialTable* m_orders = nullptr;
    QtMaterial::QtMaterialTable* m_ordersPage = nullptr;
    QStandardItemModel* m_ordersModel = nullptr;
    QStandardItemModel* m_ordersPageModel = nullptr;
    QtMaterial::QtMaterialSnackbarHost* m_snackbarHost = nullptr;
    QtMaterial::QtMaterialCommandPalette* m_commandPalette = nullptr;
    QGridLayout* m_quickGrid = nullptr;
    QGridLayout* m_chartGrid = nullptr;
    QVector<MetricWidgets> m_metrics;
    QVector<QtMaterial::QtMaterialCard*> m_metricCards;
    QVector<QToolButton*> m_navButtons;
    QWidget* m_revenueSummary = nullptr;
    QtMaterial::QtMaterialCard* m_statisticsCard = nullptr;
    QtMaterial::QtMaterialCard* m_earningsCard = nullptr;
    LineChartWidget* m_lineChart = nullptr;
    DonutChartWidget* m_donutChart = nullptr;
    QLabel* m_pageTitle = nullptr;
    QLabel* m_pageSubtitle = nullptr;
    QLabel* m_breadcrumbLabel = nullptr;
    QComboBox* m_yearCombo = nullptr;
    QComboBox* m_monthCombo = nullptr;
    QToolButton* m_themeButton = nullptr;
    bool m_compactMetrics = false;
    bool m_stackedCharts = false;
};
