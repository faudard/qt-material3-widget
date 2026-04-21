#include <QTemporaryDir>
#include <QtTest>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

class tst_ThemeSerializer : public QObject {
    Q_OBJECT

private slots:
    void roundTripsThemeJson();
    void writesAndReadsFile();
    void rejectsInvalidJson();
};

void tst_ThemeSerializer::roundTripsThemeJson()
{
    QtMaterial::ThemeBuilder builder;
    QtMaterial::Theme theme = builder.buildDarkFromSeed(QColor("#00639B"));

    theme.shapes().setRadius(QtMaterial::ShapeRole::Medium, 14);
    theme.stateLayer().hoverOpacity = 0.11;
    theme.stateLayer().focusOpacity = 0.13;
    theme.stateLayer().pressOpacity = 0.15;

    const QByteArray json = QtMaterial::ThemeSerializer::toJson(theme);

    bool ok = false;
    QString error;
    const QtMaterial::Theme restored = QtMaterial::ThemeSerializer::fromJson(json, &ok, &error);

    QVERIFY2(ok, qPrintable(error));
    QCOMPARE(restored.mode(), theme.mode());
    QCOMPARE(restored.contrastMode(), theme.contrastMode());
    QCOMPARE(restored.options().sourceColor, theme.options().sourceColor);
    QCOMPARE(restored.options().useMaterialColorUtilities, theme.options().useMaterialColorUtilities);

    QCOMPARE(restored.colorScheme().color(QtMaterial::ColorRole::Primary),
             theme.colorScheme().color(QtMaterial::ColorRole::Primary));
    QCOMPARE(restored.colorScheme().color(QtMaterial::ColorRole::Surface),
             theme.colorScheme().color(QtMaterial::ColorRole::Surface));

    QVERIFY(restored.typography().contains(QtMaterial::TypeRole::BodyLarge));
    QVERIFY(theme.typography().contains(QtMaterial::TypeRole::BodyLarge));
    QCOMPARE(restored.typography().style(QtMaterial::TypeRole::BodyLarge).font.pointSize(),
             theme.typography().style(QtMaterial::TypeRole::BodyLarge).font.pointSize());
    QCOMPARE(restored.typography().style(QtMaterial::TypeRole::BodyLarge).font.weight(),
             theme.typography().style(QtMaterial::TypeRole::BodyLarge).font.weight());
    QCOMPARE(restored.typography().style(QtMaterial::TypeRole::BodyLarge).lineHeight,
             theme.typography().style(QtMaterial::TypeRole::BodyLarge).lineHeight);

    QCOMPARE(restored.shapes().radius(QtMaterial::ShapeRole::Medium),
             theme.shapes().radius(QtMaterial::ShapeRole::Medium));

    QCOMPARE(restored.elevations().style(QtMaterial::ElevationRole::Level3).shadowBlur,
             theme.elevations().style(QtMaterial::ElevationRole::Level3).shadowBlur);
    QCOMPARE(restored.elevations().style(QtMaterial::ElevationRole::Level3).tonalOverlayOpacity,
             theme.elevations().style(QtMaterial::ElevationRole::Level3).tonalOverlayOpacity);

    QCOMPARE(restored.motion().style(QtMaterial::MotionToken::Medium2).durationMs,
             theme.motion().style(QtMaterial::MotionToken::Medium2).durationMs);
    QCOMPARE(restored.motion().style(QtMaterial::MotionToken::Medium2).easing.type(),
             theme.motion().style(QtMaterial::MotionToken::Medium2).easing.type());

    QCOMPARE(restored.stateLayer().color, theme.stateLayer().color);
    QCOMPARE(restored.stateLayer().hoverOpacity, theme.stateLayer().hoverOpacity);
    QCOMPARE(restored.stateLayer().focusOpacity, theme.stateLayer().focusOpacity);
    QCOMPARE(restored.stateLayer().pressOpacity, theme.stateLayer().pressOpacity);
}

void tst_ThemeSerializer::writesAndReadsFile()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme theme = builder.buildLightFromSeed(QColor("#6750A4"));

    const QString path = dir.filePath(QStringLiteral("theme.json"));
    QString error;
    QVERIFY2(QtMaterial::ThemeSerializer::writeToFile(theme, path, &error), qPrintable(error));

    QtMaterial::Theme restored;
    QVERIFY2(QtMaterial::ThemeSerializer::readFromFile(path, &restored, &error), qPrintable(error));

    QCOMPARE(restored.mode(), theme.mode());
    QCOMPARE(restored.colorScheme().color(QtMaterial::ColorRole::Primary),
             theme.colorScheme().color(QtMaterial::ColorRole::Primary));
}

void tst_ThemeSerializer::rejectsInvalidJson()
{
    bool ok = true;
    QString error;
    const QtMaterial::Theme theme = QtMaterial::ThemeSerializer::fromJson(QByteArray("{not valid json}"), &ok, &error);

    Q_UNUSED(theme);
    QVERIFY(!ok);
    QVERIFY(!error.isEmpty());
}

QTEST_MAIN(tst_ThemeSerializer)
#include "tst_themeserializer.moc"
