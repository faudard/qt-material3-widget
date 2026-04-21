#include <QtTest>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

class tst_ThemeSerializer : public QObject
{
    Q_OBJECT

private slots:
    void roundTripsCompleteTheme();
    void supportsPartialOverlayJson();
};

void tst_ThemeSerializer::roundTripsCompleteTheme()
{
    QtMaterial::ThemeBuilder builder;
    QtMaterial::Theme theme = builder.buildDarkFromSeed(QColor("#6750A4"));

    theme.shapes().setRadius(QtMaterial::ShapeRole::Large, 24);

    QtMaterial::TypographyStyle bodyLarge = theme.typography().style(QtMaterial::TypeRole::BodyLarge);
    bodyLarge.lineHeight = 28.0;
    theme.typography().setStyle(QtMaterial::TypeRole::BodyLarge, bodyLarge);

    QtMaterial::MotionStyle medium2 = theme.motion().style(QtMaterial::MotionToken::Medium2);
    medium2.durationMs = 333;
    medium2.easing = QEasingCurve(QEasingCurve::InOutQuad);
    theme.motion().setStyle(QtMaterial::MotionToken::Medium2, medium2);

    theme.stateLayer().hoverOpacity = 0.23;

    const QByteArray json = QtMaterial::ThemeSerializer::toJson(theme);

    bool ok = false;
    QString error;
    const QtMaterial::Theme roundTripped = QtMaterial::ThemeSerializer::fromJson(json, &ok, &error);

    QVERIFY2(ok, qPrintable(error));
    QCOMPARE(roundTripped.mode(), QtMaterial::ThemeMode::Dark);
    QCOMPARE(roundTripped.options().sourceColor, QColor("#6750A4"));
    QCOMPARE(roundTripped.shapes().radius(QtMaterial::ShapeRole::Large), 24);
    QCOMPARE(roundTripped.typography().style(QtMaterial::TypeRole::BodyLarge).lineHeight, 28.0);
    QCOMPARE(roundTripped.motion().style(QtMaterial::MotionToken::Medium2).durationMs, 333);
    QCOMPARE(roundTripped.motion().style(QtMaterial::MotionToken::Medium2).easing.type(),
             QEasingCurve(QEasingCurve::InOutQuad).type());
    QCOMPARE(roundTripped.stateLayer().hoverOpacity, 0.23);
}

void tst_ThemeSerializer::supportsPartialOverlayJson()
{
    const QByteArray json = R"json(
{
  "formatVersion": 1,
  "options": {
    "sourceColor": "#ff6750a4",
    "mode": "light",
    "contrast": "standard"
  },
  "colorScheme": {
    "Primary": "#ff112233"
  },
  "shapes": {
    "Medium": 20
  }
}
)json";

    bool ok = false;
    QString error;
    const QtMaterial::Theme theme = QtMaterial::ThemeSerializer::fromJson(json, &ok, &error);

    QVERIFY2(ok, qPrintable(error));
    QCOMPARE(theme.mode(), QtMaterial::ThemeMode::Light);
    QCOMPARE(theme.colorScheme().color(QtMaterial::ColorRole::Primary), QColor("#ff112233"));
    QCOMPARE(theme.shapes().radius(QtMaterial::ShapeRole::Medium), 20);
    QVERIFY(theme.colorScheme().contains(QtMaterial::ColorRole::Surface));
}

QTEST_MAIN(tst_ThemeSerializer)
#include "tst_themeserializer.moc"
