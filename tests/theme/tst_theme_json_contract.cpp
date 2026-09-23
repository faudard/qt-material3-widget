#include <QtTest/QtTest>

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

using namespace QtMaterial;

class tst_ThemeJsonContract : public QObject
{
    Q_OBJECT

private slots:
    void semanticRoundTripIsLossless();
    void compactSerializationIsDeterministic();
    void opaqueExtensionOverrideRoundTrips();
    void canonicalComponentNamesRoundTrip();
    void strictRejectsMissingVersion();
    void strictRejectsMissingMetadata();
    void strictRejectsInvalidResolvedBlockType();
    void strictRejectsUnknownComponentOverrideField();
    void validatesCommittedFixtureCorpus();
};

void tst_ThemeJsonContract::semanticRoundTripIsLossless()
{
    ThemeOptions options;
    options.sourceColor = QColor(QStringLiteral("#00639B"));
    options.mode = ThemeMode::Dark;
    options.preference = ThemePreference::Dark;
    options.contrast = ContrastMode::High;
    options.variant = ThemeVariant::Expressive;
    options.backendPolicy = ColorBackendPolicy::ForceFallback;

    Theme original = ThemeBuilder().build(options);

    ComponentTokenOverride local;
    local.colors.insert(ColorRole::Primary, QColor(QStringLiteral("#FF123456")));
    local.shapes.insert(ShapeRole::Full, 321);
    local.custom.insert(QStringLiteral("owner"), QStringLiteral("contract-test"));
    original.componentOverrides().setOverride(ComponentId::Button, local);

    bool ok = false;
    QString error;
    const Theme restored = ThemeSerializer::fromJson(
        ThemeSerializer::toJson(original, QJsonDocument::Compact),
        ThemeReadMode::Strict,
        &ok,
        &error);

    QVERIFY2(ok, qPrintable(error));
    QVERIFY(restored == original);
}

void tst_ThemeJsonContract::compactSerializationIsDeterministic()
{
    ThemeOptions options;
    options.sourceColor = QColor(QStringLiteral("#6750A4"));
    options.backendPolicy = ColorBackendPolicy::ForceFallback;
    const Theme theme = ThemeBuilder().build(options);

    const QByteArray first = ThemeSerializer::toJson(theme, QJsonDocument::Compact);
    const QByteArray second = ThemeSerializer::toJson(theme, QJsonDocument::Compact);
    QCOMPARE(first, second);

    QString error;
    QVERIFY2(
        ThemeSerializer::validateJson(first, ThemeReadMode::Strict, &error),
        qPrintable(error));
}

void tst_ThemeJsonContract::opaqueExtensionOverrideRoundTrips()
{
    Theme theme = ThemeBuilder().build(ThemeOptions{});
    QJsonObject root = ThemeSerializer::toJsonObject(theme);
    QJsonObject resolved = root.value(QStringLiteral("resolved")).toObject();
    QJsonObject overrides = resolved.value(QStringLiteral("componentOverrides")).toObject();

    QJsonObject custom;
    custom.insert(QStringLiteral("vendorToken"), 42);
    QJsonObject extension;
    extension.insert(QStringLiteral("custom"), custom);
    overrides.insert(QStringLiteral("com.example.superWidget"), extension);
    resolved.insert(QStringLiteral("componentOverrides"), overrides);
    root.insert(QStringLiteral("resolved"), resolved);

    bool ok = false;
    QString error;
    const Theme parsed =
        ThemeSerializer::fromJsonObject(root, ThemeReadMode::Strict, &ok, &error);
    QVERIFY2(ok, qPrintable(error));

    const QJsonObject emittedOverrides =
        ThemeSerializer::toJsonObject(parsed)
            .value(QStringLiteral("resolved"))
            .toObject()
            .value(QStringLiteral("componentOverrides"))
            .toObject();

    QVERIFY(emittedOverrides.contains(QStringLiteral("com.example.superWidget")));
    QCOMPARE(
        emittedOverrides.value(QStringLiteral("com.example.superWidget"))
            .toObject()
            .value(QStringLiteral("custom"))
            .toObject()
            .value(QStringLiteral("vendorToken"))
            .toInt(),
        42);
}

void tst_ThemeJsonContract::canonicalComponentNamesRoundTrip()
{
    QJsonObject root =
        ThemeSerializer::toJsonObject(ThemeBuilder().build(ThemeOptions{}));
    QJsonObject resolved = root.value(QStringLiteral("resolved")).toObject();
    QJsonObject overrides =
        resolved.value(QStringLiteral("componentOverrides")).toObject();

    QJsonObject shapes;
    shapes.insert(QStringLiteral("Full"), 777);
    QJsonObject componentOverride;
    componentOverride.insert(QStringLiteral("shapes"), shapes);
    overrides.insert(QStringLiteral("button.filled"), componentOverride);
    resolved.insert(QStringLiteral("componentOverrides"), overrides);
    root.insert(QStringLiteral("resolved"), resolved);

    bool ok = false;
    QString error;
    const Theme parsed =
        ThemeSerializer::fromJsonObject(root, ThemeReadMode::Strict, &ok, &error);
    QVERIFY2(ok, qPrintable(error));

    const QJsonObject emitted =
        ThemeSerializer::toJsonObject(parsed)
            .value(QStringLiteral("resolved"))
            .toObject()
            .value(QStringLiteral("componentOverrides"))
            .toObject();

    QVERIFY(emitted.contains(QStringLiteral("button.filled")));
}

void tst_ThemeJsonContract::strictRejectsMissingVersion()
{
    QJsonObject root = ThemeSerializer::toJsonObject(ThemeBuilder().build(ThemeOptions{}));
    root.remove(QStringLiteral("formatVersion"));

    bool ok = true;
    QString error;
    ThemeSerializer::fromJsonObject(root, ThemeReadMode::Strict, &ok, &error);
    QVERIFY(!ok);
    QVERIFY(error.contains(QStringLiteral("formatVersion")));
}

void tst_ThemeJsonContract::strictRejectsMissingMetadata()
{
    QJsonObject root = ThemeSerializer::toJsonObject(ThemeBuilder().build(ThemeOptions{}));
    root.remove(QStringLiteral("metadata"));

    bool ok = true;
    QString error;
    ThemeSerializer::fromJsonObject(root, ThemeReadMode::Strict, &ok, &error);
    QVERIFY(!ok);
    QVERIFY(error.contains(QStringLiteral("metadata")));
}

void tst_ThemeJsonContract::strictRejectsInvalidResolvedBlockType()
{
    QJsonObject root = ThemeSerializer::toJsonObject(ThemeBuilder().build(ThemeOptions{}));
    QJsonObject resolved = root.value(QStringLiteral("resolved")).toObject();
    resolved.insert(QStringLiteral("density"), QStringLiteral("invalid"));
    root.insert(QStringLiteral("resolved"), resolved);

    QString error;
    QVERIFY(!ThemeSerializer::validateJson(
        QJsonDocument(root).toJson(QJsonDocument::Compact),
        ThemeReadMode::Strict,
        &error));
    QVERIFY(error.contains(QStringLiteral("resolved.density")));
}

void tst_ThemeJsonContract::strictRejectsUnknownComponentOverrideField()
{
    QJsonObject root = ThemeSerializer::toJsonObject(ThemeBuilder().build(ThemeOptions{}));
    QJsonObject resolved = root.value(QStringLiteral("resolved")).toObject();
    QJsonObject overrides = resolved.value(QStringLiteral("componentOverrides")).toObject();
    QJsonObject bad;
    bad.insert(QStringLiteral("unknownField"), QJsonObject{});
    overrides.insert(QStringLiteral("button"), bad);
    resolved.insert(QStringLiteral("componentOverrides"), overrides);
    root.insert(QStringLiteral("resolved"), resolved);

    QString error;
    QVERIFY(!ThemeSerializer::validateJson(
        QJsonDocument(root).toJson(QJsonDocument::Compact),
        ThemeReadMode::Strict,
        &error));
    QVERIFY(error.contains(QStringLiteral("Unknown key")));
}

void tst_ThemeJsonContract::validatesCommittedFixtureCorpus()
{
    const QDir fixtureDir(
        QStringLiteral(QTMATERIAL3_TEST_SOURCE_DIR)
        + QStringLiteral("/theme/fixtures"));
    const QStringList files = fixtureDir.entryList(
        QStringList{QStringLiteral("*.json")},
        QDir::Files,
        QDir::Name);
    QVERIFY2(!files.isEmpty(), "Theme JSON fixture corpus must not be empty.");

    for (const QString& fileName : files) {
        QFile file(fixtureDir.filePath(fileName));
        QVERIFY2(file.open(QIODevice::ReadOnly), qPrintable(file.errorString()));

        bool ok = false;
        QString error;
        const Theme first = ThemeSerializer::fromJson(
            file.readAll(), ThemeReadMode::Strict, &ok, &error);
        QVERIFY2(ok, qPrintable(fileName + QStringLiteral(": ") + error));

        const QByteArray canonical =
            ThemeSerializer::toJson(first, QJsonDocument::Compact);
        const Theme second = ThemeSerializer::fromJson(
            canonical, ThemeReadMode::Strict, &ok, &error);
        QVERIFY2(ok, qPrintable(fileName + QStringLiteral(": ") + error));
        QVERIFY2(second == first, qPrintable(fileName));
    }
}

QTEST_MAIN(tst_ThemeJsonContract)
#include "tst_theme_json_contract.moc"
