#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialtextfieldspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_TextFieldSpecResolver : public QObject
{
    Q_OBJECT

private slots:
    void resolvesOutlinedRuntimeValues();
    void resolvesFilledRuntimeValues();
    void isolatesVariantOverrides();
    void preservesGenericOverrides();
    void remainsCompatibleWithSpecFactory();
};

void tst_TextFieldSpecResolver::resolvesOutlinedRuntimeValues()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    const TextFieldSpec spec =
        TextFieldSpecResolver()
            .outlinedTextFieldSpec(theme);

    QVERIFY(spec.containerColor.alpha() == 0);
    QCOMPARE(
        spec.focusedOutlineColor,
        theme.colorScheme().color(
            ColorRole::Primary));
    QCOMPARE(
        spec.cornerRadius,
        qreal(theme.shapes().radius(
            ShapeRole::ExtraSmall)));
    QVERIFY(spec.hasResolvedInputFont);
    QVERIFY(spec.hasResolvedLabelFont);
    QVERIFY(spec.hasResolvedSupportingFont);
    QVERIFY(spec.hasResolvedMotionStyle);
    QCOMPARE(
        spec.pressStateLayerOpacity,
        theme.stateLayer().pressOpacity);
}

void tst_TextFieldSpecResolver::resolvesFilledRuntimeValues()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildDarkFromSeed(
            QColor(QStringLiteral("#146C2E")));

    const TextFieldSpec spec =
        TextFieldSpecResolver()
            .filledTextFieldSpec(theme);

    QCOMPARE(
        spec.containerColor,
        theme.colorScheme().color(
            ColorRole::SurfaceContainerHighest));
    QVERIFY(spec.hasResolvedMotionStyle);
    QCOMPARE(
        spec.motionStyle.durationMs,
        theme.motion().style(
            MotionToken::Short4).durationMs);
}

void tst_TextFieldSpecResolver::isolatesVariantOverrides()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    ComponentTokenOverride outlined;
    outlined.custom.insert(
        QStringLiteral("horizontalPadding"),
        31);
    outlined.custom.insert(
        QStringLiteral("cornerRadius"),
        11.0);
    theme.componentOverrides().setOverride(
        QStringLiteral("textField.outlined"),
        outlined);

    TextFieldSpecResolver resolver;
    const TextFieldSpec resolvedOutlined =
        resolver.outlinedTextFieldSpec(theme);
    const TextFieldSpec resolvedFilled =
        resolver.filledTextFieldSpec(theme);

    QCOMPARE(resolvedOutlined.horizontalPadding, 31);
    QCOMPARE(resolvedOutlined.cornerRadius, 11.0);
    QVERIFY(resolvedFilled.horizontalPadding != 31);
    QVERIFY(resolvedFilled.cornerRadius != 11.0);
}

void tst_TextFieldSpecResolver::preservesGenericOverrides()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    ComponentTokenOverride generic;
    generic.custom.insert(
        QStringLiteral("supportingTopSpacing"),
        9);
    theme.componentOverrides().setOverride(
        QStringLiteral("textField"),
        generic);

    TextFieldSpecResolver resolver;
    QCOMPARE(
        resolver.outlinedTextFieldSpec(theme)
            .supportingTopSpacing,
        9);
    QCOMPARE(
        resolver.filledTextFieldSpec(theme)
            .supportingTopSpacing,
        9);
}

void tst_TextFieldSpecResolver::remainsCompatibleWithSpecFactory()
{
    ThemeBuilder builder;
    const Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    TextFieldSpecResolver resolver;

    const TextFieldSpec outlined =
        resolver.outlinedTextFieldSpec(theme);
    const TextFieldSpec legacyOutlined =
        TextFieldSpecResolver().outlinedTextFieldSpec(theme);
    QCOMPARE(
        outlined.containerColor,
        legacyOutlined.containerColor);
    QCOMPARE(
        outlined.cornerRadius,
        legacyOutlined.cornerRadius);

    const TextFieldSpec filled =
        resolver.filledTextFieldSpec(theme);
    const TextFieldSpec legacyFilled =
        TextFieldSpecResolver().filledTextFieldSpec(theme);
    QCOMPARE(
        filled.containerColor,
        legacyFilled.containerColor);
    QCOMPARE(
        filled.motionStyle.durationMs,
        legacyFilled.motionStyle.durationMs);
}

QTEST_MAIN(tst_TextFieldSpecResolver)

#include "tst_textfieldspecresolver.moc"
