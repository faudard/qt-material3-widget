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
    void lenient_rejectsLegacyShape();
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

void tst_ThemeSerializerReadModes::lenient_rejectsLegacyShape()
{
    QJsonObject legacy;
    legacy.insert(QStringLiteral("formatVersion"), 1);
    legacy.insert(QStringLiteral("options"), QJsonObject{});
    legacy.insert(QStringLiteral("colorScheme"), QJsonObject{});

    bool ok = true;
    QString error;
    ThemeSerializer::fromJsonObject(
        legacy, ThemeReadMode::Lenient, &ok, &error);

    QVERIFY(!ok);
    QVERIFY(error.contains(QStringLiteral("canonical source/resolved shape")));
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
