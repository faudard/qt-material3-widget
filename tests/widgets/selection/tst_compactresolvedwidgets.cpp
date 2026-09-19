#include <QtTest>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"
#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"

using namespace QtMaterial;

class tst_CompactResolvedWidgets : public QObject
{
    Q_OBJECT

private slots:
    void chipUsesOnlyResolvedRuntimeValues();
    void segmentedButtonUsesOnlyResolvedRuntimeValues();
    void themeContextInvalidatesBothWidgets();
};

void tst_CompactResolvedWidgets::chipUsesOnlyResolvedRuntimeValues()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    theme.shapes().setRadius(ShapeRole::Small, 13);

    TypographyStyle typography;
    typography.font = QFont(QStringLiteral("Sans Serif"), 15);
    theme.typography().setStyle(TypeRole::LabelLarge, typography);

    ThemeContext context(theme);
    QtMaterialChip chip(QStringLiteral("Assist"));
    chip.setThemeContext(&context);

    const ChipSpec& spec = chip.resolvedSpec();
    QCOMPARE(spec.cornerRadius, 13.0);
    QCOMPARE(spec.labelFont, typography.font);
}

void tst_CompactResolvedWidgets::segmentedButtonUsesOnlyResolvedRuntimeValues()
{
    Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));

    TypographyStyle typography;
    typography.font = QFont(QStringLiteral("Serif"), 14);
    theme.typography().setStyle(TypeRole::LabelLarge, typography);

    ThemeContext context(theme);
    QtMaterialSegmentedButton button;
    button.addSegment(QStringLiteral("Day"));
    button.addSegment(QStringLiteral("Week"));
    button.setThemeContext(&context);
    button.resize(button.sizeHint());

    const SegmentedButtonSpec& spec = button.resolvedSpec();
    QCOMPARE(spec.labelFont, typography.font);
    QCOMPARE(spec.cornerRadius, -1.0);
}

void tst_CompactResolvedWidgets::themeContextInvalidatesBothWidgets()
{
    Theme first = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    first.shapes().setRadius(ShapeRole::Small, 7);

    ThemeContext context(first);
    QtMaterialChip chip;
    QtMaterialSegmentedButton segmented;
    chip.setThemeContext(&context);
    segmented.setThemeContext(&context);

    QCOMPARE(chip.resolvedSpec().cornerRadius, 7.0);

    Theme second = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#006874")));
    second.shapes().setRadius(ShapeRole::Small, 19);
    second.stateLayer().pressOpacity = 0.21;
    QVERIFY(context.setTheme(second));

    QCOMPARE(chip.resolvedSpec().cornerRadius, 19.0);
    QCOMPARE(chip.resolvedSpec().pressStateLayerOpacity, 0.21);
    QCOMPARE(
        segmented.resolvedSpec().pressStateLayerOpacity,
        0.21);
}

QTEST_MAIN(tst_CompactResolvedWidgets)
#include "tst_compactresolvedwidgets.moc"
