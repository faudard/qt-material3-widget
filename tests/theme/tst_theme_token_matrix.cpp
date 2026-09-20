#include <QtTest/QtTest>

#include <QJsonDocument>

#include "qtmaterial/theme/qtmaterialcolorbackend.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

using namespace QtMaterial;

class tst_ThemeTokenMatrix : public QObject
{
    Q_OBJECT

private slots:
    void fallbackMatrix_data();
    void fallbackMatrix();
    void mcuMatrixWhenAvailable_data();
    void mcuMatrixWhenAvailable();
};

void tst_ThemeTokenMatrix::fallbackMatrix_data()
{
    QTest::addColumn<QString>("seed");
    QTest::addColumn<ThemeMode>("mode");
    QTest::addColumn<ContrastMode>("contrast");
    QTest::addColumn<ThemeVariant>("variant");

    const QStringList seeds = {
        QStringLiteral("#6750A4"),
        QStringLiteral("#00639B"),
        QStringLiteral("#B3261E")
    };
    const QVector<ThemeMode> modes = { ThemeMode::Light, ThemeMode::Dark };
    const QVector<ContrastMode> contrasts = {
        ContrastMode::Standard,
        ContrastMode::Medium,
        ContrastMode::High
    };
    const QVector<ThemeVariant> variants = {
        ThemeVariant::TonalSpot,
        ThemeVariant::Expressive
    };

    for (const QString& seed : seeds) {
        for (ThemeMode mode : modes) {
            for (ContrastMode contrast : contrasts) {
                for (ThemeVariant variant : variants) {
                    const QString name =
                        QStringLiteral("%1-%2-%3-%4")
                            .arg(seed.mid(1))
                            .arg(mode == ThemeMode::Light ? QStringLiteral("light")
                                                          : QStringLiteral("dark"))
                            .arg(static_cast<int>(contrast))
                            .arg(variant == ThemeVariant::TonalSpot
                                     ? QStringLiteral("tonalspot")
                                     : QStringLiteral("expressive"));
                    const QByteArray row = name.toLatin1();
                    QTest::newRow(row.constData())
                        << seed << mode << contrast << variant;
                }
            }
        }
    }
}

void tst_ThemeTokenMatrix::fallbackMatrix()
{
    QFETCH(QString, seed);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);
    QFETCH(ThemeVariant, variant);

    ThemeOptions options;
    options.sourceColor = QColor(seed);
    options.mode = mode;
    options.preference =
        mode == ThemeMode::Dark ? ThemePreference::Dark : ThemePreference::Light;
    options.contrast = contrast;
    options.variant = variant;
    options.backendPolicy = ColorBackendPolicy::ForceFallback;

    const ThemeBuilder builder;
    const Theme first = builder.build(options);
    const Theme second = builder.build(options);

    QCOMPARE(first, second);
    QCOMPARE(first.options(), options);
    QCOMPARE(first.mode(), mode);
    QCOMPARE(first.contrastMode(), contrast);
    QCOMPARE(first.isDark(), mode == ThemeMode::Dark);

    const ThemeColorBackendStatus status = builder.colorBackendStatus(options);
    QCOMPARE(status.effectiveBackend, ThemeColorBackend::Fallback);

    for (ColorRole role : allColorRoles()) {
        QVERIFY(first.colorScheme().contains(role));
    }
    for (TypeRole role : allTypeRoles()) {
        QVERIFY(first.typography().contains(role));
    }
    for (ShapeRole role : allShapeRoles()) {
        QVERIFY(first.shapes().contains(role));
    }
    for (ElevationRole role : allElevationRoles()) {
        QVERIFY(first.elevations().contains(role));
    }
    for (MotionToken token : allMotionTokens()) {
        QVERIFY(first.motion().contains(token));
    }
    for (DensityRole role : allDensityRoles()) {
        QVERIFY(first.density().contains(role));
    }
    for (IconSizeRole role : allIconSizeRoles()) {
        QVERIFY(first.iconSizes().contains(role));
    }

    QCOMPARE(first.accessibility().highContrast, contrast == ContrastMode::High);
    QVERIFY(first.accessibility().minimumTextContrastRatio >= 4.5);
    QVERIFY(first.accessibility().minimumUiContrastRatio >= 3.0);

    const QByteArray serialized =
        ThemeSerializer::toJson(first, QJsonDocument::Compact);
    bool ok = false;
    QString error;
    const Theme restored =
        ThemeSerializer::fromJson(serialized, ThemeReadMode::Strict, &ok, &error);
    QVERIFY2(ok, qPrintable(error));
    QCOMPARE(restored, first);
}

void tst_ThemeTokenMatrix::mcuMatrixWhenAvailable_data()
{
    QTest::addColumn<QString>("seed");
    QTest::addColumn<ThemeMode>("mode");
    QTest::addColumn<ContrastMode>("contrast");
    QTest::addColumn<ThemeVariant>("variant");

    QTest::newRow("purple-light-tonalspot")
        << QStringLiteral("#6750A4") << ThemeMode::Light
        << ContrastMode::Standard << ThemeVariant::TonalSpot;
    QTest::newRow("purple-dark-expressive")
        << QStringLiteral("#6750A4") << ThemeMode::Dark
        << ContrastMode::Medium << ThemeVariant::Expressive;
    QTest::newRow("blue-high-expressive")
        << QStringLiteral("#00639B") << ThemeMode::Light
        << ContrastMode::High << ThemeVariant::Expressive;
}

void tst_ThemeTokenMatrix::mcuMatrixWhenAvailable()
{
    if (!ThemeBuilder::isMaterialColorUtilitiesAvailable()) {
        QSKIP("Material Color Utilities is not compiled into this build.");
    }

    QFETCH(QString, seed);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);
    QFETCH(ThemeVariant, variant);

    ThemeOptions options;
    options.sourceColor = QColor(seed);
    options.mode = mode;
    options.preference =
        mode == ThemeMode::Dark ? ThemePreference::Dark : ThemePreference::Light;
    options.contrast = contrast;
    options.variant = variant;
    options.backendPolicy = ColorBackendPolicy::ForceMaterialColorUtilities;

    const ThemeBuilder builder;
    const ThemeColorBackendStatus status = builder.colorBackendStatus(options);
    QCOMPARE(status.effectiveBackend, ThemeColorBackend::MaterialColorUtilities);

    const Theme theme = builder.build(options);
    QCOMPARE(theme.options(), options);

    bool ok = false;
    QString error;
    const Theme restored = ThemeSerializer::fromJson(
        ThemeSerializer::toJson(theme, QJsonDocument::Compact),
        ThemeReadMode::Strict, &ok, &error);
    QVERIFY2(ok, qPrintable(error));
    QCOMPARE(restored, theme);
}

QTEST_MAIN(tst_ThemeTokenMatrix)
#include "tst_theme_token_matrix.moc"
