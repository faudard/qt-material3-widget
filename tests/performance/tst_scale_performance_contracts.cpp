#include <QtTest/QtTest>

#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QApplication>
#include <QColor>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QListView>
#include <QProcessEnvironment>
#include <QWidget>

#include <ctime>
#include <memory>
#include <vector>

#if defined(Q_OS_WIN)
#  include <windows.h>
#  include <psapi.h>
#elif defined(Q_OS_MACOS) || defined(Q_OS_MAC)
#  include <mach/mach.h>
#endif

#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/data/qtmaterialtreeview.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"

namespace {

constexpr int kScaleRows = 100000;
constexpr int kWidgetCount = 1000;
constexpr int kCommandCount = 50000;
constexpr qint64 kMiB = 1024 * 1024;

bool scaleEnabled()
{
    return QProcessEnvironment::systemEnvironment()
        .value(QStringLiteral("QTMATERIAL3_RUN_SCALE_PERF")) == QStringLiteral("1");
}

bool enforceBudgets()
{
    return QProcessEnvironment::systemEnvironment()
        .value(QStringLiteral("QTMATERIAL3_ENFORCE_PERF_BUDGETS")) == QStringLiteral("1");
}

qint64 envBudget(const char* name, qint64 defaultValue)
{
    bool ok = false;
    const qint64 value = QProcessEnvironment::systemEnvironment()
        .value(QString::fromLatin1(name)).toLongLong(&ok);
    return ok && value > 0 ? value : defaultValue;
}

qint64 residentBytes()
{
#if defined(Q_OS_WIN)
    PROCESS_MEMORY_COUNTERS_EX counters {};
    if (GetProcessMemoryInfo(
            GetCurrentProcess(),
            reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counters),
            sizeof(counters))) {
        return static_cast<qint64>(counters.WorkingSetSize);
    }
    return -1;
#elif defined(Q_OS_MACOS) || defined(Q_OS_MAC)
    mach_task_basic_info_data_t info {};
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(
            mach_task_self(),
            MACH_TASK_BASIC_INFO,
            reinterpret_cast<task_info_t>(&info),
            &count) == KERN_SUCCESS) {
        return static_cast<qint64>(info.resident_size);
    }
    return -1;
#elif defined(Q_OS_LINUX)
    QFile status(QStringLiteral("/proc/self/status"));
    if (!status.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return -1;
    }
    while (!status.atEnd()) {
        const QByteArray line = status.readLine();
        if (!line.startsWith("VmRSS:")) {
            continue;
        }
        const QList<QByteArray> parts = line.simplified().split(' ');
        if (parts.size() >= 2) {
            bool ok = false;
            const qint64 kib = parts.at(1).toLongLong(&ok);
            return ok ? kib * 1024 : -1;
        }
    }
    return -1;
#else
    return -1;
#endif
}

class Measurement
{
public:
    Measurement()
        : cpuStart_(std::clock())
        , rssStart_(residentBytes())
    {
        wall_.start();
    }

    qint64 wallMs() const
    {
        return wall_.elapsed();
    }

    qint64 cpuMs() const
    {
        const std::clock_t now = std::clock();
        if (cpuStart_ == static_cast<std::clock_t>(-1)
            || now == static_cast<std::clock_t>(-1)) {
            return -1;
        }
        return static_cast<qint64>(
            (static_cast<double>(now - cpuStart_) * 1000.0)
            / static_cast<double>(CLOCKS_PER_SEC));
    }

    qint64 rssGrowthMiB() const
    {
        const qint64 now = residentBytes();
        if (rssStart_ < 0 || now < 0) {
            return -1;
        }
        const qint64 growth = qMax<qint64>(0, now - rssStart_);
        return (growth + kMiB - 1) / kMiB;
    }

private:
    QElapsedTimer wall_;
    std::clock_t cpuStart_;
    qint64 rssStart_;
};

void reportAndVerify(
    const char* scenario,
    const Measurement& measurement,
    qint64 cpuBudgetMs,
    qint64 rssBudgetMiB)
{
    const qint64 wall = measurement.wallMs();
    const qint64 cpu = measurement.cpuMs();
    const qint64 rss = measurement.rssGrowthMiB();

    qInfo().noquote()
        << QStringLiteral(
               "%1: wall=%2 ms cpu=%3 ms rss_growth=%4 MiB "
               "budgets(cpu=%5 ms,rss=%6 MiB)%7")
               .arg(QString::fromLatin1(scenario))
               .arg(wall)
               .arg(cpu < 0 ? QStringLiteral("n/a") : QString::number(cpu))
               .arg(rss < 0 ? QStringLiteral("n/a") : QString::number(rss))
               .arg(cpuBudgetMs)
               .arg(rssBudgetMiB)
               .arg(enforceBudgets()
                        ? QStringLiteral(" [enforced]")
                        : QStringLiteral(" [reported only]"));

    if (!enforceBudgets()) {
        return;
    }

    if (cpu >= 0) {
        QVERIFY2(
            cpu <= cpuBudgetMs,
            qPrintable(
                QStringLiteral("%1 exceeded CPU budget: %2 ms > %3 ms")
                    .arg(QString::fromLatin1(scenario))
                    .arg(cpu)
                    .arg(cpuBudgetMs)));
    }
    if (rss >= 0) {
        QVERIFY2(
            rss <= rssBudgetMiB,
            qPrintable(
                QStringLiteral("%1 exceeded RSS budget: %2 MiB > %3 MiB")
                    .arg(QString::fromLatin1(scenario))
                    .arg(rss)
                    .arg(rssBudgetMiB)));
    }
}

class LargeTableModel final : public QAbstractTableModel
{
public:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : kScaleRows;
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : 8;
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (!index.isValid()) {
            return {};
        }
        if (role == Qt::DisplayRole || role == Qt::AccessibleTextRole) {
            return QStringLiteral("R%1 C%2").arg(index.row()).arg(index.column());
        }
        return {};
    }
};

class LargeFlatTreeModel final : public QAbstractItemModel
{
public:
    QModelIndex index(
        int row,
        int column,
        const QModelIndex& parent = QModelIndex()) const override
    {
        if (parent.isValid() || row < 0 || row >= kScaleRows || column != 0) {
            return {};
        }
        return createIndex(row, column);
    }

    QModelIndex parent(const QModelIndex&) const override
    {
        return {};
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : kScaleRows;
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : 1;
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (!index.isValid()) {
            return {};
        }
        if (role == Qt::DisplayRole || role == Qt::AccessibleTextRole) {
            return QStringLiteral("Node %1").arg(index.row());
        }
        return {};
    }
};

class SnapshotProvider final : public QtMaterial::QtMaterialCommandProvider
{
public:
    QList<quint64> requests;

    void requestCommands(const QString&, quint64 requestId) override
    {
        requests.push_back(requestId);
    }

    void activateCommand(const QString&) override
    {
    }

    void publish(
        quint64 requestId,
        const QList<QtMaterial::QtMaterialCommand>& commands)
    {
        Q_EMIT commandsReady(requestId, commands);
    }
};

QList<QtMaterial::QtMaterialCommand> makeCommands()
{
    QList<QtMaterial::QtMaterialCommand> commands;
    commands.reserve(kCommandCount);
    for (int i = 0; i < kCommandCount; ++i) {
        QtMaterial::QtMaterialCommand command;
        command.id = QStringLiteral("command.%1").arg(i, 5, 10, QLatin1Char('0'));
        command.text = QStringLiteral("Open resource %1").arg(i);
        command.secondaryText = QStringLiteral("Workspace item %1").arg(i % 1000);
        command.section = QStringLiteral("Section %1").arg(i % 32);
        command.keywords = {
            QStringLiteral("resource-%1").arg(i),
            QStringLiteral("workspace"),
            QStringLiteral("scale")
        };
        commands.push_back(command);
    }
    return commands;
}

std::vector<QtMaterial::QtMaterialFilledButton*> createButtons(QWidget* parent)
{
    std::vector<QtMaterial::QtMaterialFilledButton*> buttons;
    buttons.reserve(kWidgetCount);
    for (int i = 0; i < kWidgetCount; ++i) {
        buttons.push_back(
            new QtMaterial::QtMaterialFilledButton(
                QStringLiteral("Action %1").arg(i),
                parent));
    }
    return buttons;
}

} // namespace

class tst_ScalePerformanceContracts : public QObject
{
    Q_OBJECT

private slots:
    void dataViews100k()
    {
        if (!scaleEnabled()) {
            QSKIP("Set QTMATERIAL3_RUN_SCALE_PERF=1 to run 1.14 scale contracts.");
        }

        LargeTableModel tableModel;
        LargeFlatTreeModel treeModel;
        Measurement measurement;

        QtMaterial::QtMaterialTable table;
        table.setAttribute(Qt::WA_DontShowOnScreen);
        table.resize(1200, 720);
        table.setModel(&tableModel);
        table.show();
        const QModelIndex tableTail = tableModel.index(kScaleRows - 1, 0);
        table.setCurrentIndex(tableTail);
        table.scrollTo(tableTail, QAbstractItemView::PositionAtBottom);

        QtMaterial::QtMaterialTreeView tree;
        tree.setAttribute(Qt::WA_DontShowOnScreen);
        tree.resize(1200, 720);
        tree.setModel(&treeModel);
        tree.show();
        const QModelIndex treeTail = treeModel.index(kScaleRows - 1, 0);
        tree.setCurrentIndex(treeTail);
        tree.scrollTo(treeTail, QAbstractItemView::PositionAtBottom);

        QApplication::processEvents();

        reportAndVerify(
            "dataViews100k",
            measurement,
            envBudget("QTMATERIAL3_SCALE_DATA_VIEWS_CPU_BUDGET_MS", 3000),
            envBudget("QTMATERIAL3_SCALE_DATA_VIEWS_RSS_BUDGET_MIB", 96));
    }

    void widgetCreation1000()
    {
        if (!scaleEnabled()) {
            QSKIP("Set QTMATERIAL3_RUN_SCALE_PERF=1 to run 1.14 scale contracts.");
        }

        Measurement measurement;
        QWidget host;
        host.setAttribute(Qt::WA_DontShowOnScreen);
        const auto buttons = createButtons(&host);
        QCOMPARE(static_cast<int>(buttons.size()), kWidgetCount);
        QApplication::processEvents();

        reportAndVerify(
            "widgetCreation1000",
            measurement,
            envBudget("QTMATERIAL3_SCALE_WIDGET_CREATE_CPU_BUDGET_MS", 5000),
            envBudget("QTMATERIAL3_SCALE_WIDGET_CREATE_RSS_BUDGET_MIB", 192));
    }

    void globalThemeSwitch1000()
    {
        if (!scaleEnabled()) {
            QSKIP("Set QTMATERIAL3_RUN_SCALE_PERF=1 to run 1.14 scale contracts.");
        }

        QWidget host;
        host.setAttribute(Qt::WA_DontShowOnScreen);
        const auto buttons = createButtons(&host);
        Q_UNUSED(buttons);
        host.show();
        QApplication::processEvents();

        auto& manager = QtMaterial::ThemeManager::instance();
        const QtMaterial::Theme original = manager.theme();
        manager.applySeedColor(
            QColor(QStringLiteral("#6750A4")),
            QtMaterial::ThemeMode::Light);
        QApplication::processEvents();

        Measurement measurement;
        manager.applySeedColor(
            QColor(QStringLiteral("#00639B")),
            QtMaterial::ThemeMode::Dark);
        QApplication::processEvents();

        reportAndVerify(
            "globalThemeSwitch1000",
            measurement,
            envBudget("QTMATERIAL3_SCALE_THEME_SWITCH_CPU_BUDGET_MS", 3000),
            envBudget("QTMATERIAL3_SCALE_THEME_SWITCH_RSS_BUDGET_MIB", 64));

        manager.setTheme(original);
    }

    void commandPalette50000()
    {
        if (!scaleEnabled()) {
            QSKIP("Set QTMATERIAL3_RUN_SCALE_PERF=1 to run 1.14 scale contracts.");
        }

        const auto commands = makeCommands();
        SnapshotProvider provider;
        QtMaterial::QtMaterialCommandPalette palette;
        palette.addProvider(&provider);
        QVERIFY(!provider.requests.isEmpty());

        Measurement measurement;
        provider.publish(provider.requests.last(), commands);
        auto* results = palette.findChild<QListView*>();
        QVERIFY(results);
        QCOMPARE(results->model()->rowCount(), kCommandCount);

        palette.setQuery(QStringLiteral("resource-49999"));
        palette.refreshProviders();
        QVERIFY(!provider.requests.isEmpty());
        provider.publish(provider.requests.last(), commands);
        QCoreApplication::processEvents();
        QVERIFY(results->model()->rowCount() > 0);

        reportAndVerify(
            "commandPalette50000",
            measurement,
            envBudget("QTMATERIAL3_SCALE_COMMAND_PALETTE_CPU_BUDGET_MS", 12000),
            envBudget("QTMATERIAL3_SCALE_COMMAND_PALETTE_RSS_BUDGET_MIB", 512));
    }

    void expressiveMotion1000()
    {
        if (!scaleEnabled()) {
            QSKIP("Set QTMATERIAL3_RUN_SCALE_PERF=1 to run 1.14 scale contracts.");
        }

        QtMaterial::ThemeOptions options;
        options.sourceColor = QColor(QStringLiteral("#6750A4"));
        options.variant = QtMaterial::ThemeVariant::Expressive;
        options.motionScheme = QtMaterial::MotionScheme::Expressive;
        const QtMaterial::Theme theme = QtMaterial::ThemeBuilder().build(options);

        Measurement measurement;
        std::vector<std::unique_ptr<QtMaterial::QtMaterialTransitionController>> controllers;
        controllers.reserve(kWidgetCount);
        for (int i = 0; i < kWidgetCount; ++i) {
            auto controller =
                std::make_unique<QtMaterial::QtMaterialTransitionController>();
            controller->applyMotionToken(
                theme,
                QtMaterial::MotionToken::SpatialDefault);
            controllers.push_back(std::move(controller));
        }

        for (int pass = 0; pass < 20; ++pass) {
            const qreal target = (pass % 2 == 0) ? 1.0 : 0.0;
            for (auto& controller : controllers) {
                controller->startTo(target);
                controller->finish();
            }
        }

        reportAndVerify(
            "expressiveMotion1000",
            measurement,
            envBudget("QTMATERIAL3_SCALE_EXPRESSIVE_MOTION_CPU_BUDGET_MS", 3000),
            envBudget("QTMATERIAL3_SCALE_EXPRESSIVE_MOTION_RSS_BUDGET_MIB", 128));
    }

    void repeatedLifecycle10x1000()
    {
        if (!scaleEnabled()) {
            QSKIP("Set QTMATERIAL3_RUN_SCALE_PERF=1 to run 1.14 scale contracts.");
        }

        QApplication::processEvents();
        Measurement measurement;

        for (int cycle = 0; cycle < 10; ++cycle) {
            auto* host = new QWidget;
            host->setAttribute(Qt::WA_DontShowOnScreen);
            const auto buttons = createButtons(host);
            QCOMPARE(static_cast<int>(buttons.size()), kWidgetCount);
            delete host;
            QApplication::processEvents();
        }

        reportAndVerify(
            "repeatedLifecycle10x1000",
            measurement,
            envBudget("QTMATERIAL3_SCALE_LIFECYCLE_CPU_BUDGET_MS", 15000),
            envBudget("QTMATERIAL3_SCALE_LIFECYCLE_RSS_BUDGET_MIB", 128));
    }
};

QTEST_MAIN(tst_ScalePerformanceContracts)
#include "tst_scale_performance_contracts.moc"
