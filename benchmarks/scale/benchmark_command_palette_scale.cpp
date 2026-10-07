#include <QtTest/QtTest>

#include <QCoreApplication>
#include <QListView>

#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"

namespace {

constexpr int kCommandCount = 50000;

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

} // namespace

class benchmark_CommandPaletteScale : public QObject
{
    Q_OBJECT

private slots:
    void providerSnapshot50000()
    {
        const auto commands = makeCommands();
        SnapshotProvider provider;
        QtMaterial::QtMaterialCommandPalette palette;
        palette.addProvider(&provider);
        QVERIFY(!provider.requests.isEmpty());

        QBENCHMARK_ONCE {
            provider.publish(provider.requests.last(), commands);
            QCoreApplication::processEvents();
        }

        auto* results = palette.findChild<QListView*>();
        QVERIFY(results);
        QCOMPARE(results->model()->rowCount(), kCommandCount);
    }

    void fuzzyRefresh50000()
    {
        const auto commands = makeCommands();
        SnapshotProvider provider;
        QtMaterial::QtMaterialCommandPalette palette;
        palette.addProvider(&provider);
        provider.publish(provider.requests.last(), commands);
        auto* results = palette.findChild<QListView*>();
        QVERIFY(results);
        QCOMPARE(results->model()->rowCount(), kCommandCount);

        QBENCHMARK_ONCE {
            palette.setQuery(QStringLiteral("resource-49999"));
            palette.refreshProviders();
            provider.publish(provider.requests.last(), commands);
            QCoreApplication::processEvents();
        }

        QVERIFY(results->model()->rowCount() > 0);
    }
};

QTEST_MAIN(benchmark_CommandPaletteScale)
#include "benchmark_command_palette_scale.moc"
