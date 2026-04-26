#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class ComponentTokenOverridesTest : public QObject {
    Q_OBJECT

private slots:
    void filledButtonOverrideWinsOverFamilyDefaults();
    void selectionOverrideAppliesToCheckboxAndRadio();
    void inputOverrideAppliesToTextFieldGeometry();
    void surfaceOverridesApplyToCardAndDialog();
};

void ComponentTokenOverridesTest::filledButtonOverrideWinsOverFamilyDefaults()
{
    Theme theme = ThemeBuilder().build(ThemeOptions{});

    ComponentTokenOverride family;
    family.custom.insert(QStringLiteral("containerHeight"), 48);
    family.colors.insert(ColorRole::Primary, QColor(QStringLiteral("#102030")));
    theme.componentOverrides().setOverride(QStringLiteral("button"), family);

    ComponentTokenOverride filled;
    filled.custom.insert(QStringLiteral("containerHeight"), 52);
    filled.custom.insert(QStringLiteral("horizontalPadding"), 30);
    filled.custom.insert(QStringLiteral("iconSize"), 22);
    filled.custom.insert(QStringLiteral("iconSpacing"), 10);
    filled.colors.insert(ColorRole::Primary, QColor(QStringLiteral("#010203")));
    filled.colors.insert(ColorRole::OnPrimary, QColor(QStringLiteral("#FAFAFA")));
    theme.componentOverrides().setOverride(QStringLiteral("button.filled"), filled);

    const ButtonSpec spec = SpecFactory().filledButtonSpec(theme);
    QCOMPARE(spec.containerColor, QColor(QStringLiteral("#010203")));
    QCOMPARE(spec.labelColor, QColor(QStringLiteral("#FAFAFA")));
    QCOMPARE(spec.iconColor, QColor(QStringLiteral("#FAFAFA")));
    QCOMPARE(spec.containerHeight, 52);
    QCOMPARE(spec.horizontalPadding, 30);
    QCOMPARE(spec.iconSize, 22);
    QCOMPARE(spec.iconSpacing, 10);
}

void ComponentTokenOverridesTest::selectionOverrideAppliesToCheckboxAndRadio()
{
    Theme theme = ThemeBuilder().build(ThemeOptions{});

    ComponentTokenOverride selection;
    selection.colors.insert(ColorRole::Primary, QColor(QStringLiteral("#112233")));
    selection.colors.insert(ColorRole::OnSurface, QColor(QStringLiteral("#222222")));
    selection.custom.insert(QStringLiteral("spacing"), 18);
    theme.componentOverrides().setOverride(QStringLiteral("selection"), selection);

    ComponentTokenOverride checkboxOnly;
    checkboxOnly.custom.insert(QStringLiteral("indicatorSize"), 24);
    theme.componentOverrides().setOverride(QStringLiteral("checkbox"), checkboxOnly);

    const CheckboxSpec checkbox = SpecFactory().checkboxSpec(theme);
    QCOMPARE(checkbox.selectedContainerColor, QColor(QStringLiteral("#112233")));
    QCOMPARE(checkbox.labelColor, QColor(QStringLiteral("#222222")));
    QCOMPARE(checkbox.spacing, 18);
    QCOMPARE(checkbox.indicatorSize, 24);

    const RadioButtonSpec radio = SpecFactory().radioButtonSpec(theme);
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
    theme.componentOverrides().setOverride(QStringLiteral("textField.outlined"), textField);

    const TextFieldSpec spec = SpecFactory().outlinedTextFieldSpec(theme);
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
    theme.componentOverrides().setOverride(QStringLiteral("surface"), surface);

    ComponentTokenOverride card;
    card.custom.insert(QStringLiteral("contentPaddingLeft"), 20);
    card.custom.insert(QStringLiteral("contentPaddingTop"), 21);
    card.custom.insert(QStringLiteral("contentPaddingRight"), 22);
    card.custom.insert(QStringLiteral("contentPaddingBottom"), 23);
    card.custom.insert(QStringLiteral("elevationRole"), QStringLiteral("Level2"));
    theme.componentOverrides().setOverride(QStringLiteral("card"), card);

    const CardSpec cardSpec = SpecFactory().cardSpec(theme);
    QCOMPARE(cardSpec.contentColor, QColor(QStringLiteral("#111111")));
    QCOMPARE(cardSpec.contentPadding, QMargins(20, 21, 22, 23));
    QCOMPARE(cardSpec.elevationRole, ElevationRole::Level2);

    ComponentTokenOverride dialog;
    dialog.custom.insert(QStringLiteral("maxWidth"), 640);
    dialog.custom.insert(QStringLiteral("padding"), 32);
    dialog.custom.insert(QStringLiteral("enterMotion"), QStringLiteral("Medium3"));
    theme.componentOverrides().setOverride(QStringLiteral("dialog"), dialog);

    const DialogSpec dialogSpec = SpecFactory().dialogSpec(theme);
    QCOMPARE(dialogSpec.bodyColor, QColor(QStringLiteral("#111111")));
    QCOMPARE(dialogSpec.maxWidth, 640);
    QCOMPARE(dialogSpec.padding, 32);
    QCOMPARE(dialogSpec.enterMotion, MotionToken::Medium3);
}

QTEST_MAIN(ComponentTokenOverridesTest)
#include "tst_componenttokenoverrides.moc"
