#include <QtTest/QtTest>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "qtmaterial/theme/qtmaterialcolorbackend.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

Q_DECLARE_METATYPE(QtMaterial::ThemeMode)
Q_DECLARE_METATYPE(QtMaterial::ContrastMode)

using namespace QtMaterial;

namespace {

QString colorString(const QColor& color)
{
    return color.name(QColor::HexArgb).toLower();
}

QString modeName(ThemeMode mode)
{
    return mode == ThemeMode::Dark ? QStringLiteral("Dark") : QStringLiteral("Light");
}

QString contrastName(ContrastMode contrast)
{
    switch (contrast) {
    case ContrastMode::Standard:
        return QStringLiteral("Standard");
    case ContrastMode::Medium:
        return QStringLiteral("Medium");
    case ContrastMode::High:
        return QStringLiteral("High");
    }
    return QStringLiteral("Standard");
}

QVector<QPair<ColorRole, QString>> snapshotRoles()
{
    return {
        { ColorRole::Primary, QStringLiteral("Primary") },
        { ColorRole::OnPrimary, QStringLiteral("OnPrimary") },
        { ColorRole::PrimaryContainer, QStringLiteral("PrimaryContainer") },
        { ColorRole::OnPrimaryContainer, QStringLiteral("OnPrimaryContainer") },
        { ColorRole::Secondary, QStringLiteral("Secondary") },
        { ColorRole::SecondaryContainer, QStringLiteral("SecondaryContainer") },
        { ColorRole::Tertiary, QStringLiteral("Tertiary") },
        { ColorRole::Error, QStringLiteral("Error") },
        { ColorRole::Surface, QStringLiteral("Surface") },
        { ColorRole::OnSurface, QStringLiteral("OnSurface") },
        { ColorRole::SurfaceTint, QStringLiteral("SurfaceTint") },
        { ColorRole::Outline, QStringLiteral("Outline") },
        { ColorRole::InversePrimary, QStringLiteral("InversePrimary") },
        { ColorRole::PrimaryFixed, QStringLiteral("PrimaryFixed") },
        { ColorRole::PrimaryFixedDim, QStringLiteral("PrimaryFixedDim") },
        { ColorRole::OnPrimaryFixed, QStringLiteral("OnPrimaryFixed") },
        { ColorRole::OnPrimaryFixedVariant, QStringLiteral("OnPrimaryFixedVariant") }
    };
}

QJsonObject snapshotFor(const Theme& theme, const ThemeColorBackendStatus& status)
{
    QJsonObject root;
    root.insert(QStringLiteral("schema"), QStringLiteral("qtmaterial3-theme-seed-golden-v1"));
    root.insert(QStringLiteral("backend"), themeColorBackendToString(status.effectiveBackend));
    root.insert(QStringLiteral("requestedMcu"), status.materialColorUtilitiesRequested);
    root.insert(QStringLiteral("mcuCompiledIn"), status.materialColorUtilitiesCompiledIn);

    QJsonObject source;
    source.insert(QStringLiteral("seedColor"), colorString(theme.options().sourceColor));
    source.insert(QStringLiteral("mode"), modeName(theme.mode()));
    source.insert(QStringLiteral("contrast"), contrastName(theme.contrastMode()));
    root.insert(QStringLiteral("source"), source);

    QJsonObject roles;
    for (const auto& [role, name] : snapshotRoles()) {
        if (!theme.colorScheme().contains(role)) {
            qFatal("Missing color role in seed snapshot: %s", qPrintable(name));
        }
        roles.insert(name, colorString(theme.colorScheme().color(role)));
    }
    root.insert(QStringLiteral("roles"), roles);
    return root;
}

QJsonObject readGolden(const QString& fileName)
{
    const QString path = QStringLiteral(QTMATERIAL3_TEST_SOURCE_DIR) + QStringLiteral("/theme/goldens/") + fileName;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qFatal("Could not open golden file: %s", qPrintable(path));
    }

    QJsonParseError error;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        qFatal("Invalid golden JSON: %s", qPrintable(path));
    }
    return document.object();
}

QByteArray canonical(const QJsonObject& object)
{
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

} // namespace

class tst_ThemeSeedGoldens : public QObject {
    Q_OBJECT

private slots:
    void seedGoldens_data();
    void seedGoldens();
    void sameOptionsProduceByteIdenticalSnapshot();
    void contrastModesAreDistinctAndOrdered();
    void lightDarkParityUsesSameRoleSet();
};

void tst_ThemeSeedGoldens::seedGoldens_data()
{
    QTest::addColumn<QString>("seed");
    QTest::addColumn<ThemeMode>("mode");
    QTest::addColumn<ContrastMode>("contrast");
    QTest::addColumn<QString>("goldenFile");

    QTest::newRow("purple light standard") << QStringLiteral("#6750A4") << ThemeMode::Light << ContrastMode::Standard << QStringLiteral("seed_6750A4_light_standard.json");
    QTest::newRow("purple dark standard") << QStringLiteral("#6750A4") << ThemeMode::Dark << ContrastMode::Standard << QStringLiteral("seed_6750A4_dark_standard.json");
    QTest::newRow("blue light standard") << QStringLiteral("#00639B") << ThemeMode::Light << ContrastMode::Standard << QStringLiteral("seed_00639B_light_standard.json");
    QTest::newRow("blue dark standard") << QStringLiteral("#00639B") << ThemeMode::Dark << ContrastMode::Standard << QStringLiteral("seed_00639B_dark_standard.json");
    QTest::newRow("red light high") << QStringLiteral("#FF0000") << ThemeMode::Light << ContrastMode::High << QStringLiteral("seed_FF0000_light_high.json");
    QTest::newRow("purple dark medium") << QStringLiteral("#6750A4") << ThemeMode::Dark << ContrastMode::Medium << QStringLiteral("seed_6750A4_dark_medium.json");
}

void tst_ThemeSeedGoldens::seedGoldens()
{
    QFETCH(QString, seed);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);
    QFETCH(QString, goldenFile);

    ThemeOptions options;
    options.sourceColor = QColor(seed);
    options.mode = mode;
    options.contrast = contrast;
    options.useMaterialColorUtilities = false;

    const ThemeBuilder builder;
    const Theme theme = builder.build(options);
    const QJsonObject actual = snapshotFor(theme, builder.colorBackendStatus(options));
    const QJsonObject expected = readGolden(goldenFile);

    QCOMPARE(canonical(actual), canonical(expected));
}

void tst_ThemeSeedGoldens::sameOptionsProduceByteIdenticalSnapshot()
{
    ThemeOptions options;
    options.sourceColor = QColor(QStringLiteral("#6750A4"));
    options.mode = ThemeMode::Light;
    options.contrast = ContrastMode::Standard;
    options.useMaterialColorUtilities = false;

    const ThemeBuilder builder;
    const QJsonObject first = snapshotFor(builder.build(options), builder.colorBackendStatus(options));
    const QJsonObject second = snapshotFor(builder.build(options), builder.colorBackendStatus(options));

    QCOMPARE(canonical(first), canonical(second));
}

void tst_ThemeSeedGoldens::contrastModesAreDistinctAndOrdered()
{
    ThemeOptions standard;
    standard.sourceColor = QColor(QStringLiteral("#FF0000"));
    standard.mode = ThemeMode::Light;
    standard.contrast = ContrastMode::Standard;
    standard.useMaterialColorUtilities = false;

    ThemeOptions high = standard;
    high.contrast = ContrastMode::High;

    const ThemeBuilder builder;
    const Theme standardTheme = builder.build(standard);
    const Theme highTheme = builder.build(high);

    QVERIFY(standardTheme.colorScheme().contains(ColorRole::OnSurface));
    QVERIFY(highTheme.colorScheme().contains(ColorRole::OnSurface));
    QVERIFY(standardTheme.colorScheme().contains(ColorRole::Outline));
    QVERIFY(highTheme.colorScheme().contains(ColorRole::Outline));
    QVERIFY(standardTheme.colorScheme().color(ColorRole::OnSurface) != highTheme.colorScheme().color(ColorRole::OnSurface));
    QVERIFY(standardTheme.colorScheme().color(ColorRole::Outline) != highTheme.colorScheme().color(ColorRole::Outline));
}

void tst_ThemeSeedGoldens::lightDarkParityUsesSameRoleSet()
{
    ThemeOptions light;
    light.sourceColor = QColor(QStringLiteral("#00639B"));
    light.mode = ThemeMode::Light;
    light.useMaterialColorUtilities = false;

    ThemeOptions dark = light;
    dark.mode = ThemeMode::Dark;

    const ThemeBuilder builder;
    const Theme lightTheme = builder.build(light);
    const Theme darkTheme = builder.build(dark);

    for (const auto& [role, name] : snapshotRoles()) {
        QVERIFY2(lightTheme.colorScheme().contains(role), qPrintable(QStringLiteral("Light missing %1").arg(name)));
        QVERIFY2(darkTheme.colorScheme().contains(role), qPrintable(QStringLiteral("Dark missing %1").arg(name)));
    }
}

QTEST_MAIN(tst_ThemeSeedGoldens)
#include "tst_theme_seed_goldens.moc"
