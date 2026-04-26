#include <QtTest/QtTest>

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcessEnvironment>

#include "qtmaterial/theme/qtmaterialthemeserializer.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_ThemeMcuGoldens : public QObject {
    Q_OBJECT

private slots:
    void mcuSeedGoldens_data();
    void mcuSeedGoldens();
};

static void skipIfNoMcu()
{
    if (!ThemeBuilder::isMaterialColorUtilitiesAvailable()) {
        QSKIP("Material Color Utilities backend is not compiled into this build.");
    }
}

static ThemeOptions optionsFor(const QString& seed, ThemeMode mode, ContrastMode contrast, bool expressive)
{
    ThemeOptions options;
    options.sourceColor = QColor(seed);
    options.mode = mode;
    options.contrast = contrast;
    options.expressive = expressive;
    options.useMaterialColorUtilities = true;
    return options;
}

static QString modeName(ThemeMode mode)
{
    return mode == ThemeMode::Dark ? QStringLiteral("dark") : QStringLiteral("light");
}

static QString contrastName(ContrastMode contrast)
{
    switch (contrast) {
    case ContrastMode::Standard:
        return QStringLiteral("standard");
    case ContrastMode::Medium:
        return QStringLiteral("medium");
    case ContrastMode::High:
        return QStringLiteral("high");
    }
    return QStringLiteral("standard");
}

static QString goldenPath(const QString& seed, ThemeMode mode, ContrastMode contrast, bool expressive)
{
    QString normalizedSeed = seed;
    normalizedSeed.remove(QLatin1Char('#'));
    const QString variant = expressive ? QStringLiteral("expressive") : QStringLiteral("tonalspot");
    const QString fileName = QStringLiteral("seed_%1_%2_%3_%4.json")
        .arg(normalizedSeed, modeName(mode), contrastName(contrast), variant);
    return QStringLiteral(QTMATERIAL3_TEST_SOURCE_DIR) + QStringLiteral("/theme/goldens/mcu/") + fileName;
}

static QByteArray canonicalJsonForTheme(const Theme& theme)
{
    QJsonObject object = ThemeSerializer::toJsonObject(theme);

    // Golden files must be deterministic. The schema allows timestamp, but seed
    // regression snapshots intentionally omit it.
    if (object.contains(QStringLiteral("metadata"))) {
        QJsonObject metadata = object.value(QStringLiteral("metadata")).toObject();
        metadata.remove(QStringLiteral("timestamp"));
        object.insert(QStringLiteral("metadata"), metadata);
    }

    return QJsonDocument(object).toJson(QJsonDocument::Indented);
}

void tst_ThemeMcuGoldens::mcuSeedGoldens_data()
{
    QTest::addColumn<QString>("seed");
    QTest::addColumn<ThemeMode>("mode");
    QTest::addColumn<ContrastMode>("contrast");
    QTest::addColumn<bool>("expressive");

    QTest::newRow("6750A4-light-standard-tonalspot") << QStringLiteral("#6750A4") << ThemeMode::Light << ContrastMode::Standard << false;
    QTest::newRow("6750A4-dark-standard-tonalspot") << QStringLiteral("#6750A4") << ThemeMode::Dark << ContrastMode::Standard << false;
    QTest::newRow("00639B-light-standard-tonalspot") << QStringLiteral("#00639B") << ThemeMode::Light << ContrastMode::Standard << false;
    QTest::newRow("00639B-dark-standard-tonalspot") << QStringLiteral("#00639B") << ThemeMode::Dark << ContrastMode::Standard << false;
    QTest::newRow("FF0000-light-high-tonalspot") << QStringLiteral("#FF0000") << ThemeMode::Light << ContrastMode::High << false;
    QTest::newRow("6750A4-dark-medium-tonalspot") << QStringLiteral("#6750A4") << ThemeMode::Dark << ContrastMode::Medium << false;
    QTest::newRow("6750A4-light-standard-expressive") << QStringLiteral("#6750A4") << ThemeMode::Light << ContrastMode::Standard << true;
}

void tst_ThemeMcuGoldens::mcuSeedGoldens()
{
    skipIfNoMcu();

    QFETCH(QString, seed);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);
    QFETCH(bool, expressive);

    ThemeBuilder builder;
    const Theme theme = builder.build(optionsFor(seed, mode, contrast, expressive));
    const QByteArray actual = canonicalJsonForTheme(theme);
    const QString path = goldenPath(seed, mode, contrast, expressive);

    const bool updateGoldens = QProcessEnvironment::systemEnvironment()
        .value(QStringLiteral("QTMATERIAL3_UPDATE_GOLDENS")) == QStringLiteral("1");

    if (updateGoldens) {
        QDir().mkpath(QFileInfo(path).absolutePath());
        QFile out(path);
        QVERIFY2(out.open(QIODevice::WriteOnly | QIODevice::Truncate), qPrintable(out.errorString()));
        QCOMPARE(out.write(actual), qint64(actual.size()));
        return;
    }

    QFile in(path);
    if (!in.exists()) {
        QSKIP(qPrintable(QStringLiteral("Missing MCU golden %1. Re-run this test with QTMATERIAL3_UPDATE_GOLDENS=1 after enabling MCU, then commit the generated file.").arg(path)));
    }

    QVERIFY2(in.open(QIODevice::ReadOnly), qPrintable(in.errorString()));
    const QByteArray expected = in.readAll();
    QCOMPARE(actual, expected);
}

QTEST_MAIN(tst_ThemeMcuGoldens)
#include "tst_theme_mcu_goldens.moc"
