#pragma once

#include <QMainWindow>
#include <QVector>

class QLabel;
class QGridLayout;
class QStandardItemModel;
class QScrollArea;

namespace QtMaterial {
class QtMaterialNavigationRail;
class QtMaterialSearchBar;
class QtMaterialSegmentedButton;
class QtMaterialSnackbarHost;
class QtMaterialTable;
class QtMaterialCommandPalette;
class QtMaterialCard;
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

    QWidget* createHeader();
    QWidget* createOverviewPage();
    MetricWidgets createMetricCard(
        const QString& title,
        const QString& value,
        const QString& delta,
        bool positive);
    QtMaterial::QtMaterialCard* createRevenueCard();
    QtMaterial::QtMaterialCard* createTrafficCard();
    QtMaterial::QtMaterialCard* createOrdersCard();

    void populateOrders();
    void populateCommandPalette();
    void applyPeriod(int index);
    void applyFilter(const QString& text);
    void applyThemeChrome();
    void updateResponsiveLayout();
    void showOrderDetails(int row);
    void showMessage(const QString& text);

    QWidget* m_central = nullptr;
    QWidget* m_contentHost = nullptr;
    QScrollArea* m_scroll = nullptr;
    QtMaterial::QtMaterialNavigationRail* m_navigation = nullptr;
    QtMaterial::QtMaterialSearchBar* m_search = nullptr;
    QtMaterial::QtMaterialSegmentedButton* m_period = nullptr;
    QtMaterial::QtMaterialTable* m_orders = nullptr;
    QStandardItemModel* m_ordersModel = nullptr;
    QtMaterial::QtMaterialSnackbarHost* m_snackbarHost = nullptr;
    QtMaterial::QtMaterialCommandPalette* m_commandPalette = nullptr;
    QGridLayout* m_metricGrid = nullptr;
    QGridLayout* m_chartGrid = nullptr;
    QVector<MetricWidgets> m_metrics;
    QVector<QtMaterial::QtMaterialCard*> m_metricCards;
    QtMaterial::QtMaterialCard* m_revenueCard = nullptr;
    QtMaterial::QtMaterialCard* m_trafficCard = nullptr;
    LineChartWidget* m_lineChart = nullptr;
    DonutChartWidget* m_donutChart = nullptr;
    QLabel* m_pageTitle = nullptr;
    QLabel* m_pageSubtitle = nullptr;
    bool m_compactMetrics = false;
    bool m_stackedCharts = false;
};
