#include <QtTest/QtTest>

#include <QSet>
#include <type_traits>

#include "qtmaterial/foundation/qtmaterialcomponentid.h"
#include "qtmaterial/foundation/qtmaterialtokenid.h"
#include "qtmaterial/theme/qtmaterialthemetextcodec.h"
#include "qtmaterial/theme/qtmaterialtokenids.h"

class TypedTokenSystemTest : public QObject
{
    Q_OBJECT

private slots:
    void stableEncoding();
    void roleIdsAreUnique();
    void textCodecRoundTripsCanonicalNames();
};

void TypedTokenSystemTest::stableEncoding()
{
    using namespace QtMaterial;

    static_assert(TokenId::EncodingVersion == 1, "Token encoding version drift");
    static_assert(tokenId(ColorRole::Primary).raw() == 0x01000001u,
                  "Color/Primary id drift");
    static_assert(tokenId(TypeRole::BodyMedium).raw() == 0x0200000Bu,
                  "Typography/BodyMedium id drift");
    static_assert(tokenId(ShapeRole::Full).raw() == 0x03000007u,
                  "Shape/Full id drift");
    static_assert(tokenId(MotionToken::Long4).raw() == 0x0500000Cu,
                  "Motion/Long4 id drift");
    static_assert(tokenId(DensityRole::Default).raw() == 0x07000002u,
                  "Density/Default id drift");
    static_assert(tokenId(IconSizeRole::Medium).raw() == 0x08000003u,
                  "Icon/Medium id drift");

    static_assert(
        static_cast<std::uint32_t>(ComponentId::ButtonFilled)
            == 0x00000102u,
        "ComponentId drift");
    static_assert(
        static_cast<std::uint32_t>(ComponentId::Data)
            == 0x00001700u,
        "Data ComponentId drift");
    static_assert(
        static_cast<std::uint32_t>(ComponentId::Chip)
            == 0x00001A00u,
        "Chip ComponentId drift");

    QVERIFY(true);
}

void TypedTokenSystemTest::roleIdsAreUnique()
{
    using namespace QtMaterial;
    QSet<quint32> raw;

    auto insertUnique = [&raw](TokenId id) {
        QVERIFY(id.isValid());
        QVERIFY2(!raw.contains(id.raw()), "duplicate universal TokenId");
        raw.insert(id.raw());
    };

    for (ColorRole role : allColorRoles()) insertUnique(tokenId(role));
    for (TypeRole role : allTypeRoles()) insertUnique(tokenId(role));
    for (ShapeRole role : allShapeRoles()) insertUnique(tokenId(role));
    for (ElevationRole role : allElevationRoles()) insertUnique(tokenId(role));
    for (MotionToken role : allMotionTokens()) insertUnique(tokenId(role));
    for (StateLayerRole role : allStateLayerRoles()) insertUnique(tokenId(role));
    for (DensityRole role : allDensityRoles()) insertUnique(tokenId(role));
    for (IconSizeRole role : allIconSizeRoles()) insertUnique(tokenId(role));
}

void TypedTokenSystemTest::textCodecRoundTripsCanonicalNames()
{
    using namespace QtMaterial;

    const TokenId primary = tokenId(ColorRole::Primary);
    QCOMPARE(
        ThemeTextCodec::tokenIdToString(primary),
        QStringLiteral("Primary"));

    TokenId parsed;
    QVERIFY(ThemeTextCodec::tokenIdFromString(
        TokenCategory::Color,
        QStringLiteral("Primary"),
        &parsed));
    QCOMPARE(parsed.raw(), primary.raw());

    // Same textual leaf can exist in another category without collision.
    TokenId shapeSmall;
    TokenId iconSmall;
    QVERIFY(ThemeTextCodec::tokenIdFromString(
        TokenCategory::Shape, QStringLiteral("Small"), &shapeSmall));
    QVERIFY(ThemeTextCodec::tokenIdFromString(
        TokenCategory::IconSize, QStringLiteral("Small"), &iconSmall));
    QVERIFY(shapeSmall != iconSmall);
}

QTEST_MAIN(TypedTokenSystemTest)
#include "tst_typedtokens.moc"
