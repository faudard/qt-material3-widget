#include <QtTest/QtTest>

#include <QCoreApplication>
#include <QListView>

#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"

namespace {

constexpr int kCommandCount = 100000;

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

} // namespace

class benchmark_CommandPalette100k : public QObject
{
    Q_OBJECT

private slots:
    void providerSnapshot100000()
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

    void fuzzyRefresh100000()
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
            for (int i = 0; i < 24; ++i) {
                palette.setQuery(
                    QStringLiteral("resource-%1").arg((i * 4093) % kCommandCount));
                palette.refreshProviders();
                QVERIFY(!provider.requests.isEmpty());
                provider.publish(provider.requests.last(), commands);
                QCoreApplication::processEvents();
            }
        }

        QVERIFY(results->model()->rowCount() > 0);
    }
};

QTEST_MAIN(benchmark_CommandPalette100k)
#include "benchmark_command_palette_100k.moc"
