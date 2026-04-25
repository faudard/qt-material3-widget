#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QtTest>

#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

using namespace QtMaterial;

class tst_ThemeSerializer : public QObject {
    Q_OBJECT

private slots:
    void roundTrip_preservesCoreStructure();
    void export_containsExpectedV2Blocks();
    void roundTrip_preservesNewTokenGroups();
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
    const Theme restored = ThemeSerializer::fromJson(json, ThemeReadMode::Strict, &ok, &error);

    QVERIFY2(ok, qPrintable(error));
    QCOMPARE(restored.options().sourceColor, original.options().sourceColor);
    QCOMPARE(restored.options().mode, original.options().mode);
    QCOMPARE(restored.options().contrast, original.options().contrast);
    QCOMPARE(restored.options().expressive, original.options().expressive);
    QCOMPARE(restored.colorScheme().color(ColorRole::Primary), original.colorScheme().color(ColorRole::Primary));
    QCOMPARE(restored.colorScheme().color(ColorRole::OnSurface), original.colorScheme().color(ColorRole::OnSurface));
}

void tst_ThemeSerializer::export_containsExpectedV2Blocks()
{
    ThemeBuilder builder;
    const Theme theme = builder.build(ThemeOptions{});

    const QByteArray jsonBytes = ThemeSerializer::toJson(theme, QJsonDocument::Indented);
    const QJsonDocument doc = QJsonDocument::fromJson(jsonBytes);
    QVERIFY(doc.isObject());

    const QJsonObject root = doc.object();
    QCOMPARE(root.value(QStringLiteral("formatVersion")).toInt(), ThemeSerializer::kCurrentFormatVersion);
    QVERIFY(root.contains(QStringLiteral("source")));
    QVERIFY(root.contains(QStringLiteral("resolved")));
    QVERIFY(root.contains(QStringLiteral("metadata")));

    const QJsonObject source = root.value(QStringLiteral("source")).toObject();
    QVERIFY(source.contains(QStringLiteral("seedColor")));
    QVERIFY(source.contains(QStringLiteral("mode")));
    QVERIFY(source.contains(QStringLiteral("contrast")));

    const QJsonObject resolved = root.value(QStringLiteral("resolved")).toObject();
    QVERIFY(resolved.contains(QStringLiteral("colorScheme")));
    QVERIFY(resolved.contains(QStringLiteral("typographyScale")));
    QVERIFY(resolved.contains(QStringLiteral("shapeScale")));
    QVERIFY(resolved.contains(QStringLiteral("elevationScale")));
    QVERIFY(resolved.contains(QStringLiteral("motionTokens")));
    QVERIFY(resolved.contains(QStringLiteral("stateLayer")));
    QVERIFY(resolved.contains(QStringLiteral("density")));
    QVERIFY(resolved.contains(QStringLiteral("iconSizes")));
    QVERIFY(resolved.contains(QStringLiteral("componentOverrides")));
}

void tst_ThemeSerializer::roundTrip_preservesNewTokenGroups()
{
    ThemeBuilder builder;
    Theme theme = builder.build(ThemeOptions{});

    ComponentTokenOverride buttonOverride;
    buttonOverride.colors.insert(ColorRole::Primary, QColor(QStringLiteral("#112233")));
    buttonOverride.shapes.insert(ShapeRole::Full, 999);
    buttonOverride.density.insert(DensityRole::Compact, -4);
    buttonOverride.iconSizes.insert(IconSizeRole::Medium, 20);
    theme.componentOverrides().setOverride(QStringLiteral("Button"), buttonOverride);

    bool ok = false;
    QString error;
    const Theme restored = ThemeSerializer::fromJson(
        ThemeSerializer::toJson(theme), ThemeReadMode::Strict, &ok, &error);

    QVERIFY2(ok, qPrintable(error));
    QCOMPARE(restored.density().value(DensityRole::Compact), theme.density().value(DensityRole::Compact));
    QCOMPARE(restored.iconSizes().size(IconSizeRole::Medium), theme.iconSizes().size(IconSizeRole::Medium));
    QVERIFY(restored.componentOverrides().contains(QStringLiteral("Button")));

    const ComponentTokenOverride restoredOverride = restored.componentOverrides().overrideFor(QStringLiteral("Button"));
    QCOMPARE(restoredOverride.colors.value(ColorRole::Primary), QColor(QStringLiteral("#112233")));
    QCOMPARE(restoredOverride.shapes.value(ShapeRole::Full), 999);
    QCOMPARE(restoredOverride.density.value(DensityRole::Compact), -4);
    QCOMPARE(restoredOverride.iconSizes.value(IconSizeRole::Medium), 20);
}

QTEST_MAIN(tst_ThemeSerializer)
#include "tst_themeserializer.moc"
