#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/specs/qtmaterialbuttonspecresolver.h"
#include "qtmaterial/specs/qtmaterialdialogspecresolver.h"
#include "qtmaterial/specs/qtmaterialselectionspecresolver.h"
#include "qtmaterial/specs/qtmaterialsurfacespecresolver.h"
#include "qtmaterial/specs/qtmaterialtextfieldspecresolver.h"


using namespace QtMaterial;

class ComponentTokenOverridesTest : public QObject {
    Q_OBJECT

private slots:
    void textButtonOverridePreservesSemanticRoles();
    void filledButtonOverrideWinsOverFamilyDefaults();
    void filledTonalButtonOverridePreservesSemanticRoles();
    void outlinedButtonOverridePreservesSemanticRoles();
    void elevatedButtonOverridePreservesSemanticRoles();
    void selectionOverrideAppliesToCheckboxAndRadio();
    void inputOverrideAppliesToTextFieldGeometry();
    void surfaceOverridesApplyToCardAndDialog();
};

void ComponentTokenOverridesTest::textButtonOverridePreservesSemanticRoles()
{
    Theme theme = ThemeBuilder().build(ThemeOptions{});

    ComponentTokenOverride text;
    text.colors.insert(ColorRole::Primary, QColor(QStringLiteral("#102030")));
    text.colors.insert(
        ColorRole::OnSurfaceVariant,
        QColor(QStringLiteral("#203040")));
    text.colors.insert(ColorRole::Outline, QColor(QStringLiteral("#304050")));
    theme.componentOverrides().setOverride(ComponentId::ButtonText, text);

    const ButtonSpec spec = ButtonSpecResolver().textButtonSpec(theme);
    QCOMPARE(spec.containerColor, QColor(Qt::transparent));
    QCOMPARE(spec.disabledContainerColor, QColor(Qt::transparent));
    QCOMPARE(spec.labelColor, QColor(QStringLiteral("#102030")));
    QCOMPARE(spec.iconColor, QColor(QStringLiteral("#102030")));
    QCOMPARE(spec.stateLayerColor, QColor(QStringLiteral("#102030")));
    QCOMPARE(
        spec.disabledLabelColor.name(QColor::HexRgb),
        QColor(QStringLiteral("#203040")).name(QColor::HexRgb));
    QVERIFY(qAbs(spec.disabledLabelColor.alphaF() - 0.38) < 0.01);
    QCOMPARE(spec.outlineColor, QColor(Qt::transparent));
    QCOMPARE(spec.disabledOutlineColor, QColor(Qt::transparent));
    QCOMPARE(spec.elevationRole, ElevationRole::Level0);
    QCOMPARE(spec.hoverElevationRole, ElevationRole::Level0);
}

void ComponentTokenOverridesTest::filledButtonOverrideWinsOverFamilyDefaults()
{
    Theme theme = ThemeBuilder().build(ThemeOptions{});

    ComponentTokenOverride family;
    family.custom.insert(QStringLiteral("containerHeight"), 48);
    family.colors.insert(ColorRole::Primary, QColor(QStringLiteral("#102030")));
    theme.componentOverrides().setOverride(ComponentId::Button, family);

    ComponentTokenOverride filled;
    filled.custom.insert(QStringLiteral("containerHeight"), 52);
    filled.custom.insert(QStringLiteral("horizontalPadding"), 30);
    filled.custom.insert(QStringLiteral("iconSize"), 22);
    filled.custom.insert(QStringLiteral("iconSpacing"), 10);
    filled.colors.insert(ColorRole::Primary, QColor(QStringLiteral("#010203")));
    filled.colors.insert(ColorRole::OnPrimary, QColor(QStringLiteral("#FAFAFA")));
    filled.colors.insert(ColorRole::OnSurface, QColor(QStringLiteral("#112233")));
    filled.colors.insert(
        ColorRole::OnSurfaceVariant,
        QColor(QStringLiteral("#445566")));
    filled.custom.insert(QStringLiteral("focusRingWidth"), 3.5);
    theme.componentOverrides().setOverride(ComponentId::ButtonFilled, filled);

    const ButtonSpec spec = ButtonSpecResolver().filledButtonSpec(theme);
    QCOMPARE(spec.containerColor, QColor(QStringLiteral("#010203")));
    QCOMPARE(spec.labelColor, QColor(QStringLiteral("#FAFAFA")));
    QCOMPARE(spec.iconColor, QColor(QStringLiteral("#FAFAFA")));
    QCOMPARE(
        spec.disabledContainerColor.name(QColor::HexRgb),
        QColor(QStringLiteral("#112233")).name(QColor::HexRgb));
    QCOMPARE(
        spec.disabledLabelColor.name(QColor::HexRgb),
        QColor(QStringLiteral("#445566")).name(QColor::HexRgb));
    QVERIFY(qAbs(spec.disabledContainerColor.alphaF() - 0.10) < 0.01);
    QVERIFY(qAbs(spec.disabledLabelColor.alphaF() - 0.38) < 0.01);
    QCOMPARE(spec.focusRingWidth, 3.5);
    QCOMPARE(spec.containerHeight, 52);
    QCOMPARE(spec.horizontalPadding, 30);
    QCOMPARE(spec.iconSize, 22);
    QCOMPARE(spec.iconSpacing, 10);
}

void ComponentTokenOverridesTest::filledTonalButtonOverridePreservesSemanticRoles()
{
    Theme theme = ThemeBuilder().build(ThemeOptions{});

    ComponentTokenOverride tonal;
    tonal.colors.insert(
        ColorRole::SecondaryContainer,
        QColor(QStringLiteral("#102030")));
    tonal.colors.insert(
        ColorRole::OnSecondaryContainer,
        QColor(QStringLiteral("#203040")));
    tonal.colors.insert(ColorRole::OnSurface, QColor(QStringLiteral("#304050")));
    theme.componentOverrides().setOverride(ComponentId::ButtonFilledTonal, tonal);

    const ButtonSpec spec = ButtonSpecResolver().filledTonalButtonSpec(theme);
    QCOMPARE(spec.containerColor, QColor(QStringLiteral("#102030")));
    QCOMPARE(spec.labelColor, QColor(QStringLiteral("#203040")));
    QCOMPARE(spec.iconColor, QColor(QStringLiteral("#203040")));
    QCOMPARE(spec.stateLayerColor, QColor(QStringLiteral("#203040")));
    QCOMPARE(
        spec.disabledContainerColor.name(QColor::HexRgb),
        QColor(QStringLiteral("#304050")).name(QColor::HexRgb));
    QCOMPARE(
        spec.disabledLabelColor.name(QColor::HexRgb),
        QColor(QStringLiteral("#304050")).name(QColor::HexRgb));
    QVERIFY(qAbs(spec.disabledContainerColor.alphaF() - 0.12) < 0.01);
    QVERIFY(qAbs(spec.disabledLabelColor.alphaF() - 0.38) < 0.01);
    QCOMPARE(spec.elevationRole, ElevationRole::Level0);
    QCOMPARE(spec.hoverElevationRole, ElevationRole::Level1);
}

void ComponentTokenOverridesTest::outlinedButtonOverridePreservesSemanticRoles()
{
    Theme theme = ThemeBuilder().build(ThemeOptions{});

    ComponentTokenOverride outlined;
    outlined.colors.insert(
        ColorRole::OnSurfaceVariant,
        QColor(QStringLiteral("#102030")));
    outlined.colors.insert(
        ColorRole::OutlineVariant,
        QColor(QStringLiteral("#203040")));
    outlined.custom.insert(QStringLiteral("outlineWidth"), 2.5);
    theme.componentOverrides().setOverride(ComponentId::ButtonOutlined, outlined);

    const ButtonSpec spec = ButtonSpecResolver().outlinedButtonSpec(theme);
    QCOMPARE(spec.containerColor, QColor(Qt::transparent));
    QCOMPARE(spec.disabledContainerColor, QColor(Qt::transparent));
    QCOMPARE(spec.labelColor, QColor(QStringLiteral("#102030")));
    QCOMPARE(spec.iconColor, QColor(QStringLiteral("#102030")));
    QCOMPARE(spec.stateLayerColor, QColor(QStringLiteral("#102030")));
    QCOMPARE(
        spec.disabledLabelColor.name(QColor::HexRgb),
        QColor(QStringLiteral("#102030")).name(QColor::HexRgb));
    QVERIFY(qAbs(spec.disabledLabelColor.alphaF() - 0.38) < 0.01);
    QCOMPARE(spec.outlineColor, QColor(QStringLiteral("#203040")));
    QCOMPARE(
        spec.disabledOutlineColor.name(QColor::HexRgb),
        QColor(QStringLiteral("#203040")).name(QColor::HexRgb));
    QVERIFY(qAbs(spec.disabledOutlineColor.alphaF() - 0.10) < 0.01);
    QCOMPARE(spec.outlineWidth, 2.5);
}

void ComponentTokenOverridesTest::elevatedButtonOverridePreservesSemanticRoles()
{
    Theme theme = ThemeBuilder().build(ThemeOptions{});

    const ElevationStyle normalElevation{ 5, 2, 0.06 };
    const ElevationStyle hoverElevation{ 9, 3, 0.09 };
    ComponentTokenOverride elevated;
    elevated.colors.insert(
        ColorRole::SurfaceContainerLow,
        QColor(QStringLiteral("#102030")));
    elevated.colors.insert(ColorRole::Primary, QColor(QStringLiteral("#203040")));
    elevated.colors.insert(ColorRole::OnSurface, QColor(QStringLiteral("#304050")));
    elevated.colors.insert(
        ColorRole::OnSurfaceVariant,
        QColor(QStringLiteral("#405060")));
    elevated.elevations.insert(ElevationRole::Level1, normalElevation);
    elevated.elevations.insert(ElevationRole::Level2, hoverElevation);
    theme.componentOverrides().setOverride(ComponentId::ButtonElevated, elevated);

    const ButtonSpec spec = ButtonSpecResolver().elevatedButtonSpec(theme);
    QCOMPARE(spec.containerColor, QColor(QStringLiteral("#102030")));
    QCOMPARE(spec.labelColor, QColor(QStringLiteral("#203040")));
    QCOMPARE(spec.iconColor, QColor(QStringLiteral("#203040")));
    QCOMPARE(spec.stateLayerColor, QColor(QStringLiteral("#203040")));
    QCOMPARE(
        spec.disabledContainerColor.name(QColor::HexRgb),
        QColor(QStringLiteral("#304050")).name(QColor::HexRgb));
    QCOMPARE(
        spec.disabledLabelColor.name(QColor::HexRgb),
        QColor(QStringLiteral("#405060")).name(QColor::HexRgb));
    QVERIFY(qAbs(spec.disabledContainerColor.alphaF() - 0.10) < 0.01);
    QVERIFY(qAbs(spec.disabledLabelColor.alphaF() - 0.38) < 0.01);
    QVERIFY(spec.hasResolvedElevationStyle);
    QVERIFY(spec.hasResolvedHoverElevationStyle);
    QCOMPARE(spec.elevationStyle.shadowBlur, normalElevation.shadowBlur);
    QCOMPARE(spec.elevationStyle.shadowYOffset, normalElevation.shadowYOffset);
    QCOMPARE(
        spec.elevationStyle.tonalOverlayOpacity,
        normalElevation.tonalOverlayOpacity);
    QCOMPARE(spec.hoverElevationStyle.shadowBlur, hoverElevation.shadowBlur);
    QCOMPARE(
        spec.hoverElevationStyle.shadowYOffset,
        hoverElevation.shadowYOffset);
    QCOMPARE(
        spec.hoverElevationStyle.tonalOverlayOpacity,
        hoverElevation.tonalOverlayOpacity);
}

void ComponentTokenOverridesTest::selectionOverrideAppliesToCheckboxAndRadio()
{
    Theme theme = ThemeBuilder().build(ThemeOptions{});

    ComponentTokenOverride selection;
    selection.colors.insert(ColorRole::Primary, QColor(QStringLiteral("#112233")));
    selection.colors.insert(ColorRole::OnSurface, QColor(QStringLiteral("#222222")));
    selection.custom.insert(QStringLiteral("spacing"), 18);
    theme.componentOverrides().setOverride(ComponentId::Selection, selection);

    ComponentTokenOverride checkboxOnly;
    checkboxOnly.custom.insert(QStringLiteral("indicatorSize"), 24);
    theme.componentOverrides().setOverride(ComponentId::Checkbox, checkboxOnly);

    const CheckboxSpec checkbox = SelectionSpecResolver().checkboxSpec(theme);
    QCOMPARE(checkbox.selectedContainerColor, QColor(QStringLiteral("#112233")));
    QCOMPARE(checkbox.labelColor, QColor(QStringLiteral("#222222")));
    QCOMPARE(checkbox.spacing, 18);
    QCOMPARE(checkbox.indicatorSize, 24);

    const RadioButtonSpec radio = SelectionSpecResolver().radioButtonSpec(theme);
    QCOMPARE(radio.selectedColor, QColor(QStringLiteral("#112233")));
    QCOMPARE(radio.labelColor, QColor(QStringLiteral("#222222")));
    QCOMPARE(radio.spacing, 18);
}

void ComponentTokenOverridesTest::inputOverrideAppliesToTextFieldGeometry()
{
    Theme theme = ThemeBuilder().build(ThemeOptions{});

    ComponentTokenOverride textField;
    textField.custom.insert(QStringLiteral("minHeight"), 64);
    textField.custom.insert(QStringLiteral("horizontalPadding"), 20);
    textField.custom.insert(QStringLiteral("focusedOutlineWidth"), 3);
    textField.custom.insert(QStringLiteral("shapeRole"), QStringLiteral("Large"));
    textField.colors.insert(ColorRole::Primary, QColor(QStringLiteral("#335577")));
    theme.componentOverrides().setOverride(ComponentId::TextFieldOutlined, textField);

    const TextFieldSpec spec = TextFieldSpecResolver().outlinedTextFieldSpec(theme);
    QCOMPARE(spec.minHeight, 64);
    QCOMPARE(spec.horizontalPadding, 20);
    QCOMPARE(spec.focusedOutlineWidth, 3);
    QCOMPARE(spec.focusedOutlineColor, QColor(QStringLiteral("#335577")));
    QCOMPARE(spec.shapeRole, ShapeRole::Large);
}

void ComponentTokenOverridesTest::surfaceOverridesApplyToCardAndDialog()
{
    Theme theme = ThemeBuilder().build(ThemeOptions{});

    ComponentTokenOverride surface;
    surface.colors.insert(ColorRole::OnSurface, QColor(QStringLiteral("#111111")));
    surface.colors.insert(ColorRole::OnSurfaceVariant, QColor(QStringLiteral("#222222")));
    theme.componentOverrides().setOverride(ComponentId::Surface, surface);

    ComponentTokenOverride card;
    card.custom.insert(QStringLiteral("contentPaddingLeft"), 20);
    card.custom.insert(QStringLiteral("contentPaddingTop"), 21);
    card.custom.insert(QStringLiteral("contentPaddingRight"), 22);
    card.custom.insert(QStringLiteral("contentPaddingBottom"), 23);
    card.custom.insert(QStringLiteral("elevationRole"), QStringLiteral("Level2"));
    theme.componentOverrides().setOverride(ComponentId::Card, card);

    const CardSpec cardSpec = SurfaceSpecResolver().cardSpec(theme);
    QCOMPARE(cardSpec.contentColor, QColor(QStringLiteral("#111111")));
    QCOMPARE(cardSpec.contentPadding, QMargins(20, 21, 22, 23));
    QCOMPARE(cardSpec.elevationRole, ElevationRole::Level2);

    ComponentTokenOverride dialog;
    dialog.custom.insert(QStringLiteral("maxWidth"), 640);
    dialog.custom.insert(QStringLiteral("padding"), 32);
    dialog.custom.insert(QStringLiteral("enterMotion"), QStringLiteral("Medium3"));
    theme.componentOverrides().setOverride(ComponentId::Dialog, dialog);

    const DialogSpec dialogSpec = DialogSpecResolver().dialogSpec(theme);
    QCOMPARE(dialogSpec.bodyColor, QColor(QStringLiteral("#222222")));
    QCOMPARE(dialogSpec.maxWidth, 640);
    QCOMPARE(dialogSpec.padding, 32);
    QCOMPARE(dialogSpec.enterMotion, MotionToken::Medium3);
}

QTEST_MAIN(ComponentTokenOverridesTest)

#include "tst_componenttokenoverrides.moc"
