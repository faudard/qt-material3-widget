#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QtTest>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

using namespace QtMaterial;

class tst_ThemeSerializerReadModes : public QObject {
    Q_OBJECT

private slots:
    void strict_rejectsUnknownRootKeys();
    void upgradeIfPossible_acceptsV1Theme();
    void lenient_acceptsUnknownRootKeys();
};

void tst_ThemeSerializerReadModes::strict_rejectsUnknownRootKeys()
{
    ThemeBuilder builder;
    const Theme theme = builder.build(ThemeOptions{});
    QJsonObject object = ThemeSerializer::toJsonObject(theme);
    object.insert(QStringLiteral("unexpected"), true);

    bool ok = true;
    QString error;
    ThemeSerializer::fromJsonObject(object, ThemeReadMode::Strict, &ok, &error);

    QVERIFY(!ok);
    QVERIFY(error.contains(QStringLiteral("Unknown key")));
}

void tst_ThemeSerializerReadModes::upgradeIfPossible_acceptsV1Theme()
{
    QJsonObject v1;
    v1.insert(QStringLiteral("formatVersion"), 1);

    QJsonObject options;
    options.insert(QStringLiteral("sourceColor"), QStringLiteral("#ff6750a4"));
    options.insert(QStringLiteral("mode"), QStringLiteral("Dark"));
    options.insert(QStringLiteral("contrast"), QStringLiteral("High"));
    options.insert(QStringLiteral("expressive"), false);
    options.insert(QStringLiteral("useMaterialColorUtilities"), false);
    v1.insert(QStringLiteral("options"), options);

    QJsonObject colorScheme;
    colorScheme.insert(QStringLiteral("Primary"), QStringLiteral("#ff6750a4"));
    colorScheme.insert(QStringLiteral("OnSurface"), QStringLiteral("#ffe6e1e5"));
    v1.insert(QStringLiteral("colorScheme"), colorScheme);

    bool ok = false;
    QString error;
    const Theme theme = ThemeSerializer::fromJsonObject(v1, ThemeReadMode::UpgradeIfPossible, &ok, &error);

    QVERIFY2(ok, qPrintable(error));
    QCOMPARE(theme.mode(), ThemeMode::Dark);
    QCOMPARE(theme.contrastMode(), ContrastMode::High);
    QCOMPARE(theme.options().sourceColor, QColor(QStringLiteral("#ff6750a4")));
    QCOMPARE(theme.colorScheme().color(ColorRole::Primary), QColor(QStringLiteral("#ff6750a4")));
}

void tst_ThemeSerializerReadModes::lenient_acceptsUnknownRootKeys()
{
    ThemeBuilder builder;
    const Theme theme = builder.build(ThemeOptions{});
    QJsonObject object = ThemeSerializer::toJsonObject(theme);
    object.insert(QStringLiteral("futureField"), QStringLiteral("ignored"));

    bool ok = false;
    QString error;
    const Theme restored = ThemeSerializer::fromJsonObject(object, ThemeReadMode::Lenient, &ok, &error);

    QVERIFY2(ok, qPrintable(error));
    QCOMPARE(restored.mode(), theme.mode());
}

QTEST_MAIN(tst_ThemeSerializerReadModes)
#include "tst_themeserializer_readmodes.moc"
