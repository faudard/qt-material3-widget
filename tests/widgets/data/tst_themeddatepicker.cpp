#include <QtTest>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/qtmaterialdatepicker.h"

using namespace QtMaterial;

class tst_ThemedDatePicker : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void resolvesSpecFromParentContext();
    void followsThemeChanges();
    void explicitSpecRemainsPinned();
    void resetSpecReturnsToThemeMode();
};

void tst_ThemedDatePicker::resolvesSpecFromParentContext()
{
    const Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    ThemeContext context(theme);

    QtMaterialWidget parent;
    parent.setThemeContext(&context);
    QtMaterialDatePicker picker(&parent);

    const DatePickerSpec expected =
        DataSpecResolver().datePickerSpec(theme);

    QCOMPARE(picker.effectiveThemeContext(), &context);
    QVERIFY(!picker.hasExplicitSpec());
    QCOMPARE(
        picker.spec().backgroundColor,
        expected.backgroundColor);
    QCOMPARE(
        picker.spec().headlineFont,
        expected.headlineFont);
}

void tst_ThemedDatePicker::followsThemeChanges()
{
    Theme first = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#6750A4")));
    Theme second = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));
    ThemeContext context(first);

    QtMaterialDatePicker picker;
    picker.setThemeContext(&context);
    const QColor before = picker.spec().backgroundColor;

    QVERIFY(context.setTheme(second));

    const DatePickerSpec expected =
        DataSpecResolver().datePickerSpec(second);
    QCOMPARE(
        picker.spec().backgroundColor,
        expected.backgroundColor);
    QVERIFY(picker.spec().backgroundColor != before);
}

void tst_ThemedDatePicker::explicitSpecRemainsPinned()
{
    ThemeContext context(
        ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4"))));

    QtMaterialDatePicker picker;
    picker.setThemeContext(&context);

    DatePickerSpec explicitSpec = picker.spec();
    explicitSpec.backgroundColor =
        QColor(QStringLiteral("#123456"));
    explicitSpec.cornerRadius = 9;
    picker.setSpec(explicitSpec);

    QVERIFY(picker.hasExplicitSpec());

    QVERIFY(context.setTheme(
        ThemeBuilder().buildDarkFromSeed(
            QColor(QStringLiteral("#006874")))));

    QCOMPARE(
        picker.spec().backgroundColor,
        QColor(QStringLiteral("#123456")));
    QCOMPARE(picker.spec().cornerRadius, 9);
}

void tst_ThemedDatePicker::resetSpecReturnsToThemeMode()
{
    const Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#006874")));
    ThemeContext context(theme);

    QtMaterialDatePicker picker;
    picker.setThemeContext(&context);

    DatePickerSpec explicitSpec = picker.spec();
    explicitSpec.backgroundColor =
        QColor(QStringLiteral("#123456"));
    picker.setSpec(explicitSpec);
    picker.resetSpec();

    const DatePickerSpec expected =
        DataSpecResolver().datePickerSpec(theme);

    QVERIFY(!picker.hasExplicitSpec());
    QCOMPARE(
        picker.spec().backgroundColor,
        expected.backgroundColor);
}

QTEST_MAIN(tst_ThemedDatePicker)
#include "tst_themeddatepicker.moc"
