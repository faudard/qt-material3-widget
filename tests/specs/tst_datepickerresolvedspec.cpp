#include <QtTest>

#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"
#include "qtmaterial/specs/qtmaterialdataspecresolver.h"
#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

using namespace QtMaterial;

class tst_DatePickerResolvedSpec : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void canonicalSpecHeaderIsUsable();
    void resolvesCompleteRuntimeValues();
    void componentOverridesWin();
};

void tst_DatePickerResolvedSpec::canonicalSpecHeaderIsUsable()
{
    DatePickerSpec spec;
    spec.weekdayTextColor = QColor(Qt::red);
    spec.contentSpacing = 11;
    QCOMPARE(spec.weekdayTextColor, QColor(Qt::red));
    QCOMPARE(spec.contentSpacing, 11);
}

void tst_DatePickerResolvedSpec::resolvesCompleteRuntimeValues()
{
    Theme theme = ThemeBuilder().buildDarkFromSeed(
        QColor(QStringLiteral("#6750A4")));
    theme.shapes().setRadius(ShapeRole::ExtraLarge, 24);

    const DatePickerSpec spec =
        DataSpecResolver().datePickerSpec(theme);

    QCOMPARE(spec.cornerRadius, 24);
    QVERIFY(spec.weekdayTextColor.isValid());
    QVERIFY(spec.navigationButtonTextColor.isValid());
    QCOMPARE(spec.headlineFont, theme.typography().style(TypeRole::HeadlineSmall).font);
    QCOMPARE(spec.dayFont, theme.typography().style(TypeRole::BodyMedium).font);
    QCOMPARE(spec.weekdayFont, theme.typography().style(TypeRole::LabelLarge).font);
    QVERIFY(spec.cellSize >= 32);
    QVERIFY(spec.headerHeight >= 40);
}

void tst_DatePickerResolvedSpec::componentOverridesWin()
{
    Theme theme = ThemeBuilder().buildLightFromSeed(
        QColor(QStringLiteral("#006874")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(QStringLiteral("cornerRadius"), 13);
    tokens.custom.insert(QStringLiteral("cellSize"), 52);
    tokens.custom.insert(QStringLiteral("contentSpacing"), 15);
    tokens.custom.insert(
        QStringLiteral("weekdayTextColor"),
        QStringLiteral("#123456"));
    tokens.custom.insert(
        QStringLiteral("navigationButtonTextColor"),
        QStringLiteral("#345678"));
    theme.componentOverrides().setOverride(
        ComponentId::DatePicker,
        tokens);

    const DatePickerSpec spec =
        DataSpecResolver().datePickerSpec(theme);

    QCOMPARE(spec.cornerRadius, 13);
    QCOMPARE(spec.cellSize, 52);
    QCOMPARE(spec.contentSpacing, 15);
    QCOMPARE(
        spec.weekdayTextColor,
        QColor(QStringLiteral("#123456")));
    QCOMPARE(
        spec.navigationButtonTextColor,
        QColor(QStringLiteral("#345678")));
}

QTEST_MAIN(tst_DatePickerResolvedSpec)
#include "tst_datepickerresolvedspec.moc"
