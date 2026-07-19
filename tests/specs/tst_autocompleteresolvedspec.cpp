#include <QtTest>

#include "qtmaterial/specs/qtmaterialautocompletespecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_AutocompleteResolvedSpec : public QObject
{
    Q_OBJECT

private slots:
    void resolvesRuntimeValues();
    void resolvesComponentOverrides();
};

void tst_AutocompleteResolvedSpec::resolvesRuntimeValues()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));

    TypographyStyle input;
    input.font = QFont(QStringLiteral("Sans Serif"), 14);
    theme.typography().setStyle(TypeRole::BodyLarge, input);
    theme.shapes().setRadius(ShapeRole::ExtraSmall, 8);

    MotionStyle motion;
    motion.durationMs = 270;
    motion.easing = QEasingCurve(QEasingCurve::OutCubic);
    theme.motion().setStyle(MotionToken::Short4, motion);

    ElevationStyle elevation;
    elevation.shadowBlur = 18;
    elevation.shadowYOffset = 4;
    theme.elevations().setStyle(ElevationRole::Level2, elevation);

    const AutocompleteSpec spec =
        AutocompleteSpecResolver().autocompleteSpec(theme);

    QVERIFY(spec.hasResolvedInputFont);
    QCOMPARE(spec.inputFont, input.font);
    QVERIFY(spec.hasResolvedSuggestionFont);
    QCOMPARE(spec.inputCornerRadius, 8.0);
    QCOMPARE(spec.popupCornerRadius, 8.0);
    QVERIFY(spec.hasResolvedPopupMotionStyle);
    QCOMPARE(spec.popupMotionStyle.durationMs, 270);
    QVERIFY(spec.hasResolvedPopupElevationStyle);
    QCOMPARE(spec.popupElevationStyle.shadowBlur, 18);
}

void tst_AutocompleteResolvedSpec::resolvesComponentOverrides()
{
    Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(QStringLiteral("inputCornerRadius"), 15.0);
    tokens.custom.insert(QStringLiteral("popupCornerRadius"), 17.0);
    tokens.custom.insert(QStringLiteral("horizontalPadding"), 20);
    tokens.custom.insert(QStringLiteral("focusedOutlineWidth"), 3.0);
    tokens.custom.insert(
        QStringLiteral("inputContainerColor"),
        QStringLiteral("#123456"));
    tokens.hasStateLayer = true;
    tokens.stateLayer.pressOpacity = 0.22;

    theme.componentOverrides().setOverride(
        QStringLiteral("autocomplete"),
        tokens);

    const AutocompleteSpec spec =
        AutocompleteSpecResolver().autocompleteSpec(theme);

    QCOMPARE(spec.inputCornerRadius, 15.0);
    QCOMPARE(spec.popupCornerRadius, 17.0);
    QCOMPARE(spec.horizontalPadding, 20);
    QCOMPARE(spec.focusedOutlineWidth, 3.0);
    QCOMPARE(
        spec.inputContainerColor,
        QColor(QStringLiteral("#123456")));
    QCOMPARE(spec.pressStateLayerOpacity, 0.22);
}

QTEST_MAIN(tst_AutocompleteResolvedSpec)
#include "tst_autocompleteresolvedspec.moc"
