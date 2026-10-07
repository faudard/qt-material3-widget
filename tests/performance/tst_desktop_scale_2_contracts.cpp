#include <QtTest/QtTest>

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QAbstractTableModel>
#include <QApplication>
#include <QColor>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QListView>
#include <QPixmap>
#include <QPixmapCache>
#include <QProcessEnvironment>
#include <QSortFilterProxyModel>
#include <QWidget>

#include <algorithm>
#include <cmath>
#include <ctime>
#include <memory>
#include <vector>

#if defined(Q_OS_WIN)
#  include <windows.h>
#  include <psapi.h>
#elif defined(Q_OS_MACOS) || defined(Q_OS_MAC)
#  include <mach/mach.h>
#endif

#include "qtmaterial/effects/private/qtmaterialshadowcache_p.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/data/qtmaterialtreeview.h"
#include "qtmaterial/widgets/layouts/qtmaterialadaptiveshell.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"
#include "qtmaterial/widgets/surfaces/qtmaterialsidesheet.h"

namespace {

constexpr int kMillionRows = 1000000;
constexpr int kCommandCount = 100000;
constexpr int kThemeWidgetCount = 64;
constexpr qint64 kMiB = 1024 * 1024;

bool desktopScale2Enabled()
{
    return QProcessEnvironment::systemEnvironment()
        .value(QStringLiteral("QTMATERIAL3_RUN_DESKTOP_SCALE_2")) == QStringLiteral("1");
}

bool enforceBudgets()
{
    return QProcessEnvironment::systemEnvironment()
        .value(QStringLiteral("QTMATERIAL3_ENFORCE_PERF_BUDGETS")) == QStringLiteral("1");
}

int envPositiveInt(const char* name, int defaultValue)
{
    bool ok = false;
    const int value = QProcessEnvironment::systemEnvironment()
        .value(QString::fromLatin1(name)).toInt(&ok);
    return ok && value > 0 ? value : defaultValue;
}

double envPositiveDouble(const QString& name, double defaultValue)
{
    if (name.isEmpty()) {
        return defaultValue;
    }
    bool ok = false;
    const double value = QProcessEnvironment::systemEnvironment().value(name).toDouble(&ok);
    return ok && value > 0.0 ? value : defaultValue;
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

struct MeasurementResult
{
    qint64 wallMs = -1;
    qint64 cpuMs = -1;
    qint64 rssGrowthMiB = -1;
};

class Measurement
{
public:
    Measurement()
        : cpuStart_(std::clock())
        , rssStart_(residentBytes())
    {
        wall_.start();
    }

    MeasurementResult snapshot() const
    {
        MeasurementResult result;
        result.wallMs = wall_.elapsed();

        const std::clock_t now = std::clock();
        if (cpuStart_ != static_cast<std::clock_t>(-1)
            && now != static_cast<std::clock_t>(-1)) {
            result.cpuMs = static_cast<qint64>(
                (static_cast<double>(now - cpuStart_) * 1000.0)
                / static_cast<double>(CLOCKS_PER_SEC));
        }

        const qint64 rssNow = residentBytes();
        if (rssStart_ >= 0 && rssNow >= 0) {
            const qint64 growth = qMax<qint64>(0, rssNow - rssStart_);
            result.rssGrowthMiB = (growth + kMiB - 1) / kMiB;
        }
        return result;
    }

private:
    QElapsedTimer wall_;
    std::clock_t cpuStart_;
    qint64 rssStart_;
};

template <typename Function>
double timedMs(Function&& function)
{
    QElapsedTimer timer;
    timer.start();
    function();
    return static_cast<double>(timer.nsecsElapsed()) / 1000000.0;
}

double percentile(const QVector<double>& values, double fraction)
{
    if (values.isEmpty()) {
        return 0.0;
    }
    QVector<double> ordered = values;
    std::sort(ordered.begin(), ordered.end());
    const int index = qBound(
        0,
        static_cast<int>(std::ceil(fraction * ordered.size())) - 1,
        ordered.size() - 1);
    return ordered.at(index);
}

class MillionTableModel final : public QAbstractTableModel
{
public:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : kMillionRows;
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
            return QStringLiteral("row-%1 bucket-%2")
                .arg(index.row())
                .arg(index.row() % 1024, 4, 10, QLatin1Char('0'));
        }
        return {};
    }
};

class MillionFlatTreeModel final : public QAbstractItemModel
{
public:
    QModelIndex index(
        int row,
        int column,
        const QModelIndex& parent = QModelIndex()) const override
    {
        if (parent.isValid() || row < 0 || row >= kMillionRows || column != 0) {
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
        return parent.isValid() ? 0 : kMillionRows;
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
            return QStringLiteral("node-%1").arg(index.row());
        }
        return {};
    }
};

class LazyMillionTableModel final : public QAbstractTableModel
{
public:
    explicit LazyMillionTableModel(int chunkSize = 50000)
        : chunkSize_(chunkSize)
    {
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : loadedRows_;
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 : 4;
    }

    QVariant data(const QModelIndex& index, int role) const override
    {
        if (!index.isValid()) {
            return {};
        }
        if (role == Qt::DisplayRole || role == Qt::AccessibleTextRole) {
            return QStringLiteral("lazy-%1-%2").arg(index.row()).arg(index.column());
        }
        return {};
    }

    bool canFetchMore(const QModelIndex& parent) const override
    {
        return !parent.isValid() && loadedRows_ < kMillionRows;
    }

    void fetchMore(const QModelIndex& parent) override
    {
        if (parent.isValid() || loadedRows_ >= kMillionRows) {
            return;
        }
        const int count = qMin(chunkSize_, kMillionRows - loadedRows_);
        beginInsertRows(QModelIndex(), loadedRows_, loadedRows_ + count - 1);
        loadedRows_ += count;
        endInsertRows();
    }

private:
    int loadedRows_ = 0;
    int chunkSize_ = 50000;
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

    void publish(quint64 requestId, const QList<QtMaterial::QtMaterialCommand>& commands)
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
        command.id = QStringLiteral("command.%1").arg(i, 6, 10, QLatin1Char('0'));
        command.text = QStringLiteral("Open resource %1").arg(i);
        command.secondaryText = QStringLiteral("Workspace item %1").arg(i % 4096);
        command.section = QStringLiteral("Section %1").arg(i % 64);
        command.keywords = QStringList()
            << QStringLiteral("resource-%1").arg(i)
            << QStringLiteral("workspace")
            << QStringLiteral("desktop-scale");
        commands.push_back(command);
    }
    return commands;
}

void populateButtons(QWidget* host, int count)
{
    for (int i = 0; i < count; ++i) {
        new QtMaterial::QtMaterialFilledButton(
            QStringLiteral("Action %1").arg(i),
            host);
    }
}

void runTransientSurfaceCycle(QWidget& host)
{
    {
        QtMaterial::QtMaterialDialog dialog(&host);
        dialog.setTitleText(QStringLiteral("Desktop scale dialog"));
        dialog.setSupportingText(QStringLiteral("Transient surface lifecycle benchmark"));
        dialog.open();
        dialog.close();
    }
    {
        QtMaterial::QtMaterialSideSheet sheet(&host);
        sheet.setTitleText(QStringLiteral("Desktop scale side sheet"));
        sheet.setModal(true);
        sheet.open();
        sheet.closeSheet();
    }
    QCoreApplication::processEvents();
}

} // namespace

class tst_DesktopScale2Contracts : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        QFile budgetFile(QStringLiteral(QTMATERIAL3_DESKTOP_SCALE_2_BUDGET_FILE));
        QVERIFY2(
            budgetFile.open(QIODevice::ReadOnly | QIODevice::Text),
            qPrintable(QStringLiteral("Cannot open desktop-scale budget file: %1")
                           .arg(budgetFile.fileName())));

        QJsonParseError error;
        const QJsonDocument document = QJsonDocument::fromJson(budgetFile.readAll(), &error);
        QVERIFY2(
            error.error == QJsonParseError::NoError && document.isObject(),
            qPrintable(QStringLiteral("Invalid desktop-scale budget JSON: %1")
                           .arg(error.errorString())));

        budgets_ = document.object().value(QStringLiteral("scenarios")).toObject();
        QVERIFY2(!budgets_.isEmpty(), "Desktop-scale budget table has no scenarios.");
    }

    void cleanupTestCase()
    {
        const QString path = QProcessEnvironment::systemEnvironment()
            .value(QStringLiteral("QTMATERIAL3_DESKTOP_SCALE_JSON"));
        if (path.isEmpty() || results_.isEmpty()) {
            return;
        }

        const QFileInfo info(path);
        QDir().mkpath(info.absolutePath());

        QJsonObject root;
        root.insert(QStringLiteral("version"), 1);
        root.insert(QStringLiteral("suite"), QStringLiteral("desktop-scale-2"));
        root.insert(
            QStringLiteral("generatedAt"),
            QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

        QJsonArray scenarios;
        for (const QJsonObject& result : results_) {
            scenarios.append(result);
        }
        root.insert(QStringLiteral("scenarios"), scenarios);

        QFile output(path);
        QVERIFY2(
            output.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text),
            qPrintable(QStringLiteral("Cannot write desktop-scale result JSON: %1").arg(path)));
        output.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    }

    void dataViewsMillion()
    {
        if (!desktopScale2Enabled()) {
            QSKIP("Set QTMATERIAL3_RUN_DESKTOP_SCALE_2=1 to run 1.21 Desktop Scale 2.0 contracts.");
        }

        MillionTableModel tableModel;
        MillionFlatTreeModel treeModel;
        Measurement measurement;

        QtMaterial::QtMaterialTable table;
        table.setAttribute(Qt::WA_DontShowOnScreen);
        table.resize(1280, 720);
        table.setModel(&tableModel);
        table.show();

        QtMaterial::QtMaterialTreeView tree;
        tree.setAttribute(Qt::WA_DontShowOnScreen);
        tree.resize(1280, 720);
        tree.setModel(&treeModel);
        tree.show();

        QCoreApplication::processEvents();
        QCOMPARE(tableModel.rowCount(), kMillionRows);
        QCOMPARE(treeModel.rowCount(), kMillionRows);

        QVector<double> samples;
        samples.reserve(64);
        for (int i = 0; i < 64; ++i) {
            const int row = static_cast<int>(
                (static_cast<qint64>(i + 1) * 15485863LL) % kMillionRows);
            samples.push_back(timedMs([&] {
                const QModelIndex tableIndex = tableModel.index(row, 0);
                const QModelIndex treeIndex = treeModel.index(row, 0);
                table.setCurrentIndex(tableIndex);
                table.scrollTo(tableIndex, QAbstractItemView::PositionAtCenter);
                tree.setCurrentIndex(treeIndex);
                tree.scrollTo(treeIndex, QAbstractItemView::PositionAtCenter);
                QCoreApplication::processEvents();
            }));
        }

        verifyAndRecord(
            QStringLiteral("dataViewsMillion"),
            measurement.snapshot(),
            samples);
    }

    void lazyFetchMoreMillion()
    {
        if (!desktopScale2Enabled()) {
            QSKIP("Set QTMATERIAL3_RUN_DESKTOP_SCALE_2=1 to run 1.21 Desktop Scale 2.0 contracts.");
        }

        LazyMillionTableModel model;
        QtMaterial::QtMaterialTable table;
        table.setAttribute(Qt::WA_DontShowOnScreen);
        table.resize(1280, 720);
        table.setModel(&model);
        table.show();
        QCoreApplication::processEvents();

        Measurement measurement;
        QVector<double> samples;
        while (model.canFetchMore(QModelIndex())) {
            samples.push_back(timedMs([&] {
                model.fetchMore(QModelIndex());
                QCoreApplication::processEvents();
            }));
        }

        QCOMPARE(model.rowCount(), kMillionRows);
        verifyAndRecord(
            QStringLiteral("lazyFetchMoreMillion"),
            measurement.snapshot(),
            samples);
    }

    void proxyFilterMillion()
    {
        if (!desktopScale2Enabled()) {
            QSKIP("Set QTMATERIAL3_RUN_DESKTOP_SCALE_2=1 to run 1.21 Desktop Scale 2.0 contracts.");
        }

        MillionTableModel source;
        QSortFilterProxyModel proxy;
        proxy.setSourceModel(&source);
        proxy.setFilterKeyColumn(0);
        proxy.setFilterCaseSensitivity(Qt::CaseInsensitive);
        proxy.setDynamicSortFilter(false);

        QtMaterial::QtMaterialTable table;
        table.setAttribute(Qt::WA_DontShowOnScreen);
        table.resize(1280, 720);
        table.setModel(&proxy);
        table.show();
        QCoreApplication::processEvents();

        const QStringList queries = {
            QStringLiteral("bucket-0001"),
            QStringLiteral("row-999999"),
            QStringLiteral("bucket-0512"),
            QStringLiteral("row-500000"),
            QStringLiteral("bucket-0999"),
            QStringLiteral("row-42"),
            QStringLiteral("bucket-0256"),
            QStringLiteral("row-750000"),
            QStringLiteral("bucket-0768"),
            QStringLiteral("row-125000"),
            QStringLiteral("bucket-0100"),
            QStringLiteral("no-match-desktop-scale")
        };

        Measurement measurement;
        QVector<double> samples;
        samples.reserve(queries.size());
        for (const QString& query : queries) {
            samples.push_back(timedMs([&] {
                proxy.setFilterFixedString(query);
                const int matches = proxy.rowCount();
                Q_UNUSED(matches);
                QCoreApplication::processEvents();
            }));
        }

        verifyAndRecord(
            QStringLiteral("proxyFilterMillion"),
            measurement.snapshot(),
            samples);
    }

    void commandPalette100k()
    {
        if (!desktopScale2Enabled()) {
            QSKIP("Set QTMATERIAL3_RUN_DESKTOP_SCALE_2=1 to run 1.21 Desktop Scale 2.0 contracts.");
        }

        const auto commands = makeCommands();
        SnapshotProvider provider;
        QtMaterial::QtMaterialCommandPalette palette;
        palette.addProvider(&provider);
        QVERIFY(!provider.requests.isEmpty());

        auto* results = palette.findChild<QListView*>();
        QVERIFY(results);

        Measurement measurement;
        QVector<double> samples;
        samples.push_back(timedMs([&] {
            provider.publish(provider.requests.last(), commands);
            QCoreApplication::processEvents();
        }));
        QCOMPARE(results->model()->rowCount(), kCommandCount);

        for (int i = 0; i < 24; ++i) {
            const QString query = QStringLiteral("resource-%1")
                                      .arg((i * 4093) % kCommandCount);
            samples.push_back(timedMs([&] {
                palette.setQuery(query);
                palette.refreshProviders();
                QVERIFY(!provider.requests.isEmpty());
                provider.publish(provider.requests.last(), commands);
                QCoreApplication::processEvents();
                QVERIFY(results->model()->rowCount() > 0);
            }));
        }

        verifyAndRecord(
            QStringLiteral("commandPalette100k"),
            measurement.snapshot(),
            samples);
    }

    void themeChanges2000()
    {
        if (!desktopScale2Enabled()) {
            QSKIP("Set QTMATERIAL3_RUN_DESKTOP_SCALE_2=1 to run 1.21 Desktop Scale 2.0 contracts.");
        }

        QWidget host;
        host.setAttribute(Qt::WA_DontShowOnScreen);
        populateButtons(&host, kThemeWidgetCount);
        host.show();
        QCoreApplication::processEvents();

        auto& manager = QtMaterial::ThemeManager::instance();
        const QtMaterial::Theme original = manager.theme();
        manager.applySeedColor(
            QColor(QStringLiteral("#6750A4")),
            QtMaterial::ThemeMode::Light);
        QCoreApplication::processEvents();

        Measurement measurement;
        QVector<double> samples;
        samples.reserve(2000);
        for (int i = 0; i < 2000; ++i) {
            const QColor color = QColor::fromHsv((i * 37) % 360, 160, 210);
            samples.push_back(timedMs([&] {
                manager.applySeedColor(
                    color,
                    (i % 2 == 0)
                        ? QtMaterial::ThemeMode::Light
                        : QtMaterial::ThemeMode::Dark);
                QCoreApplication::processEvents();
            }));
        }

        const MeasurementResult result = measurement.snapshot();
        manager.setTheme(original);
        QCoreApplication::processEvents();

        verifyAndRecord(
            QStringLiteral("themeChanges2000"),
            result,
            samples);
    }

    void adaptiveResizeStorm5000()
    {
        if (!desktopScale2Enabled()) {
            QSKIP("Set QTMATERIAL3_RUN_DESKTOP_SCALE_2=1 to run 1.21 Desktop Scale 2.0 contracts.");
        }

        QtMaterial::QtMaterialAdaptiveShell shell;
        shell.setAttribute(Qt::WA_DontShowOnScreen);
        shell.setContentWidget(new QWidget);
        shell.setSupportingWidget(new QWidget);
        shell.resize(1000, 720);
        shell.show();
        QCoreApplication::processEvents();

        const int widths[] = {500, 599, 600, 700, 839, 840, 1000, 1199, 1200, 1599, 1600, 1700};

        Measurement measurement;
        QVector<double> samples;
        samples.reserve(5000);
        for (int i = 0; i < 5000; ++i) {
            const int width = widths[i % (sizeof(widths) / sizeof(widths[0]))];
            samples.push_back(timedMs([&] {
                shell.resize(width, 720 + (i % 3));
                QCoreApplication::processEvents();
            }));
        }

        verifyAndRecord(
            QStringLiteral("adaptiveResizeStorm5000"),
            measurement.snapshot(),
            samples);
    }

    void transientSurfaces1000()
    {
        if (!desktopScale2Enabled()) {
            QSKIP("Set QTMATERIAL3_RUN_DESKTOP_SCALE_2=1 to run 1.21 Desktop Scale 2.0 contracts.");
        }

        QWidget host;
        host.setAttribute(Qt::WA_DontShowOnScreen);
        host.resize(1024, 768);
        host.show();
        QCoreApplication::processEvents();

        Measurement measurement;
        QVector<double> samples;
        samples.reserve(500);
        for (int cycle = 0; cycle < 500; ++cycle) {
            samples.push_back(timedMs([&] {
                runTransientSurfaceCycle(host);
            }));
        }

        verifyAndRecord(
            QStringLiteral("transientSurfaces1000"),
            measurement.snapshot(),
            samples);
    }

    void shadowPixmapCache5000()
    {
        if (!desktopScale2Enabled()) {
            QSKIP("Set QTMATERIAL3_RUN_DESKTOP_SCALE_2=1 to run 1.21 Desktop Scale 2.0 contracts.");
        }

        QPixmapCache::clear();
        QtMaterial::QtMaterialShadowCache cache;
        QPixmap pixmap(64, 64);
        pixmap.setDevicePixelRatio(2.0);
        pixmap.fill(Qt::transparent);

        QStringList keys;
        keys.reserve(256);
        for (int i = 0; i < 256; ++i) {
            const QString key = cache.keyFor(
                64 + (i % 8),
                64 + (i % 5),
                8.0 + (i % 4),
                12 + (i % 6),
                i % 4,
                QColor(0, 0, 0, 40 + (i % 80)),
                2.0);
            keys.push_back(key);
            cache.insert(key, pixmap);
        }

        Measurement measurement;
        QVector<double> samples;
        samples.reserve(5000);
        for (int i = 0; i < 5000; ++i) {
            samples.push_back(timedMs([&] {
                QPixmap out;
                const bool found = cache.find(keys.at(i % keys.size()), &out);
                QVERIFY(found);
                QVERIFY(!out.isNull());
                if ((i % 64) == 0) {
                    cache.insert(keys.at((i / 64) % keys.size()), pixmap);
                }
            }));
        }

        verifyAndRecord(
            QStringLiteral("shadowPixmapCache5000"),
            measurement.snapshot(),
            samples);
    }

    void longRunMemoryCycles()
    {
        if (!desktopScale2Enabled()) {
            QSKIP("Set QTMATERIAL3_RUN_DESKTOP_SCALE_2=1 to run 1.21 Desktop Scale 2.0 contracts.");
        }

        QWidget warmHost;
        warmHost.setAttribute(Qt::WA_DontShowOnScreen);
        warmHost.resize(1024, 768);
        for (int i = 0; i < 10; ++i) {
            runTransientSurfaceCycle(warmHost);
            auto* widgetHost = new QWidget;
            widgetHost->setAttribute(Qt::WA_DontShowOnScreen);
            populateButtons(widgetHost, kThemeWidgetCount);
            delete widgetHost;
        }
        QCoreApplication::processEvents();

        const int cycles = envPositiveInt(
            "QTMATERIAL3_DESKTOP_SCALE_LONG_CYCLES",
            250);
        Measurement measurement;
        QVector<double> samples;
        samples.reserve(cycles);

        for (int cycle = 0; cycle < cycles; ++cycle) {
            samples.push_back(timedMs([&] {
                auto* host = new QWidget;
                host->setAttribute(Qt::WA_DontShowOnScreen);
                host->resize(1024, 768);
                populateButtons(host, kThemeWidgetCount);
                runTransientSurfaceCycle(*host);
                delete host;
                QCoreApplication::processEvents();
            }));
        }

        verifyAndRecord(
            QStringLiteral("longRunMemoryCycles"),
            measurement.snapshot(),
            samples);
    }

private:
    double budgetValue(
        const QJsonObject& budget,
        const QString& valueKey,
        const QString& overrideKey) const
    {
        const double defaultValue = budget.value(valueKey).toDouble(-1.0);
        const QString overrideName = budget.value(overrideKey).toString();
        return envPositiveDouble(overrideName, defaultValue);
    }

    void verifyAndRecord(
        const QString& scenario,
        const MeasurementResult& measurement,
        const QVector<double>& samples)
    {
        QVERIFY2(
            budgets_.contains(scenario),
            qPrintable(QStringLiteral("Missing budget entry for %1").arg(scenario)));

        const QJsonObject budget = budgets_.value(scenario).toObject();
        const double cpuBudgetMs = budgetValue(
            budget,
            QStringLiteral("cpuBudgetMs"),
            QStringLiteral("cpuOverride"));
        const double rssBudgetMiB = budgetValue(
            budget,
            QStringLiteral("rssGrowthBudgetMiB"),
            QStringLiteral("rssOverride"));
        const double p95BudgetMs = budgetValue(
            budget,
            QStringLiteral("p95BudgetMs"),
            QStringLiteral("p95Override"));
        const double p99BudgetMs = budgetValue(
            budget,
            QStringLiteral("p99BudgetMs"),
            QStringLiteral("p99Override"));

        QVERIFY2(cpuBudgetMs > 0.0, "CPU budget must be positive.");
        QVERIFY2(rssBudgetMiB > 0.0, "RSS budget must be positive.");
        QVERIFY2(p95BudgetMs > 0.0, "p95 budget must be positive.");
        QVERIFY2(p99BudgetMs > 0.0, "p99 budget must be positive.");

        const double p95 = percentile(samples, 0.95);
        const double p99 = percentile(samples, 0.99);

        qInfo().noquote()
            << QStringLiteral(
                   "%1: wall=%2 ms cpu=%3 ms rss_growth=%4 MiB p95=%5 ms p99=%6 ms "
                   "budgets(cpu=%7,rss=%8,p95=%9,p99=%10)%11")
                   .arg(scenario)
                   .arg(measurement.wallMs)
                   .arg(measurement.cpuMs < 0
                            ? QStringLiteral("n/a")
                            : QString::number(measurement.cpuMs))
                   .arg(measurement.rssGrowthMiB < 0
                            ? QStringLiteral("n/a")
                            : QString::number(measurement.rssGrowthMiB))
                   .arg(p95, 0, 'f', 3)
                   .arg(p99, 0, 'f', 3)
                   .arg(cpuBudgetMs, 0, 'f', 1)
                   .arg(rssBudgetMiB, 0, 'f', 1)
                   .arg(p95BudgetMs, 0, 'f', 1)
                   .arg(p99BudgetMs, 0, 'f', 1)
                   .arg(enforceBudgets()
                            ? QStringLiteral(" [enforced]")
                            : QStringLiteral(" [reported only]"));

        QJsonObject result;
        result.insert(QStringLiteral("name"), scenario);
        result.insert(QStringLiteral("wallMs"), measurement.wallMs);
        if (measurement.cpuMs >= 0) {
            result.insert(QStringLiteral("cpuMs"), measurement.cpuMs);
        }
        if (measurement.rssGrowthMiB >= 0) {
            result.insert(QStringLiteral("rssGrowthMiB"), measurement.rssGrowthMiB);
        }
        result.insert(QStringLiteral("p95Ms"), p95);
        result.insert(QStringLiteral("p99Ms"), p99);
        result.insert(QStringLiteral("sampleCount"), samples.size());
        result.insert(QStringLiteral("cpuBudgetMs"), cpuBudgetMs);
        result.insert(QStringLiteral("rssGrowthBudgetMiB"), rssBudgetMiB);
        result.insert(QStringLiteral("p95BudgetMs"), p95BudgetMs);
        result.insert(QStringLiteral("p99BudgetMs"), p99BudgetMs);
        results_.push_back(result);

        if (!enforceBudgets()) {
            return;
        }

        if (measurement.cpuMs >= 0) {
            QVERIFY2(
                static_cast<double>(measurement.cpuMs) <= cpuBudgetMs,
                qPrintable(QStringLiteral("%1 exceeded CPU budget: %2 ms > %3 ms")
                               .arg(scenario)
                               .arg(measurement.cpuMs)
                               .arg(cpuBudgetMs)));
        }
        if (measurement.rssGrowthMiB >= 0) {
            QVERIFY2(
                static_cast<double>(measurement.rssGrowthMiB) <= rssBudgetMiB,
                qPrintable(QStringLiteral("%1 exceeded RSS growth budget: %2 MiB > %3 MiB")
                               .arg(scenario)
                               .arg(measurement.rssGrowthMiB)
                               .arg(rssBudgetMiB)));
        }
        QVERIFY2(
            p95 <= p95BudgetMs,
            qPrintable(QStringLiteral("%1 exceeded p95 budget: %2 ms > %3 ms")
                           .arg(scenario)
                           .arg(p95, 0, 'f', 3)
                           .arg(p95BudgetMs, 0, 'f', 3)));
        QVERIFY2(
            p99 <= p99BudgetMs,
            qPrintable(QStringLiteral("%1 exceeded p99 budget: %2 ms > %3 ms")
                           .arg(scenario)
                           .arg(p99, 0, 'f', 3)
                           .arg(p99BudgetMs, 0, 'f', 3)));
    }

    QJsonObject budgets_;
    QVector<QJsonObject> results_;
};

QTEST_MAIN(tst_DesktopScale2Contracts)
#include "tst_desktop_scale_2_contracts.moc"
