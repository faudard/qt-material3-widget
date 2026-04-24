#include <QtTest/QtTest>
#include <QJsonDocument>
#include <QJsonObject>

#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

using namespace QtMaterial;

class tst_ThemeSerializer : public QObject
{
    Q_OBJECT

private slots:
    void roundTrip_preservesCoreStructure();
    void export_containsExpectedBlocks();
};

void tst_ThemeSerializer::roundTrip_preservesCoreStructure()
{
    ThemeOptions options;
    options.sourceColor = QColor(QStringLiteral("#0B57D0"));
    options.mode = ThemeMode::Dark;
    options.contrast = ContrastMode::Medium;
    options.expressive = true;

    ThemeBuilder builder;
    const Theme original = builder.build(options);

    QString error;
    bool ok = false;
    const QByteArray json = ThemeSerializer::toJson(original, QJsonDocument::Indented);
    const Theme restored = ThemeSerializer::fromJson(json, &ok, &error);

    QVERIFY2(ok, qPrintable(error));
    QCOMPARE(restored.options().sourceColor, original.options().sourceColor);
    QCOMPARE(restored.options().mode, original.options().mode);
    QCOMPARE(restored.options().contrast, original.options().contrast);
    QCOMPARE(restored.options().expressive, original.options().expressive);
    QCOMPARE(restored.colorScheme().color(ColorRole::Primary),
             original.colorScheme().color(ColorRole::Primary));
    QCOMPARE(restored.colorScheme().color(ColorRole::OnSurface),
             original.colorScheme().color(ColorRole::OnSurface));
}

void tst_ThemeSerializer::export_containsExpectedBlocks()
{
    ThemeBuilder builder;
    const Theme theme = builder.build(ThemeOptions {});

    const QByteArray jsonBytes = ThemeSerializer::toJson(theme, QJsonDocument::Indented);
    const QJsonDocument doc = QJsonDocument::fromJson(jsonBytes);
    QVERIFY(doc.isObject());

    const QJsonObject root = doc.object();
    QVERIFY(root.contains(QStringLiteral("formatVersion")));
    QVERIFY(root.contains(QStringLiteral("options")));
    QVERIFY(root.contains(QStringLiteral("colorScheme")));
    QVERIFY(root.contains(QStringLiteral("typography")));
    QVERIFY(root.contains(QStringLiteral("shapes")));
    QVERIFY(root.contains(QStringLiteral("elevations")));
    QVERIFY(root.contains(QStringLiteral("motion")));
    QVERIFY(root.contains(QStringLiteral("stateLayer")));
}

QTEST_MAIN(tst_ThemeSerializer)
#include "tst_themeserializer.moc"
