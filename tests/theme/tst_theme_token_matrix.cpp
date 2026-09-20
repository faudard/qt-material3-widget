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

    QVERIFY(first == second);
    QVERIFY(first.options() == options);
    QCOMPARE(first.mode(), mode);
    QCOMPARE(first.contrastMode(), contrast);
    QCOMPARE(first.isDark(), mode == ThemeMode::Dark);

    const ThemeColorBackendStatus status = builder.colorBackendStatus(options);
    QCOMPARE(status.effectiveBackend, ThemeColorBackend::Fallback);

    for (int value = static_cast<int>(ColorRole::Primary);
         value <= static_cast<int>(ColorRole::Scrim);
         ++value) {
        QVERIFY(first.colorScheme().contains(static_cast<ColorRole>(value)));
    }
    for (int value = static_cast<int>(TypeRole::DisplayLarge);
         value <= static_cast<int>(TypeRole::LabelSmall);
         ++value) {
        QVERIFY(first.typography().contains(static_cast<TypeRole>(value)));
    }
    for (int value = static_cast<int>(ShapeRole::None);
         value <= static_cast<int>(ShapeRole::Full);
         ++value) {
        QVERIFY(first.shapes().contains(static_cast<ShapeRole>(value)));
    }
    for (int value = static_cast<int>(ElevationRole::Level0);
         value <= static_cast<int>(ElevationRole::Level5);
         ++value) {
        QVERIFY(first.elevations().contains(static_cast<ElevationRole>(value)));
    }
    for (int value = static_cast<int>(MotionToken::Short1);
         value <= static_cast<int>(MotionToken::Long4);
         ++value) {
        QVERIFY(first.motion().contains(static_cast<MotionToken>(value)));
    }
    for (int value = static_cast<int>(DensityRole::Compact);
         value <= static_cast<int>(DensityRole::Comfortable);
         ++value) {
        QVERIFY(first.density().contains(static_cast<DensityRole>(value)));
    }
    for (int value = static_cast<int>(IconSizeRole::ExtraSmall);
         value <= static_cast<int>(IconSizeRole::ExtraLarge);
         ++value) {
        QVERIFY(first.iconSizes().contains(static_cast<IconSizeRole>(value)));
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
    QVERIFY(restored == first);
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
    QVERIFY(theme.options() == options);

    bool ok = false;
    QString error;
    const Theme restored = ThemeSerializer::fromJson(
        ThemeSerializer::toJson(theme, QJsonDocument::Compact),
        ThemeReadMode::Strict, &ok, &error);
    QVERIFY2(ok, qPrintable(error));
    QVERIFY(restored == theme);
}

QTEST_MAIN(tst_ThemeTokenMatrix)
#include "tst_theme_token_matrix.moc"
