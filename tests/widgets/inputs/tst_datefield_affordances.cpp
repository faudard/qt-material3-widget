#include <QLineEdit>
#include <QSignalSpy>
#include <QToolButton>
#include <QtTest/QtTest>

#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"

using namespace QtMaterial;

class tst_DateFieldAffordances : public QObject
{
    Q_OBJECT

private slots:
    void editingFinishedParsesDate();
    void calendarActionIsAvailableWhenEmpty();
    void clearActionClearsDate();
    void clearDoesNotEmitSpuriousParseErrorSignal();
    void resolvedSpecFollowsThemeContext();
};

void tst_DateFieldAffordances::editingFinishedParsesDate()
{
    QtMaterialDateField field;
    field.setDisplayFormat(
        QStringLiteral("yyyy-MM-dd"));

    QSignalSpy dateSpy(
        &field,
        &QtMaterialDateField::dateChanged);

    QVERIFY(field.lineEdit());
    field.lineEdit()->setText(
        QStringLiteral("2026-07-14"));

    QVERIFY(QMetaObject::invokeMethod(
        field.lineEdit(),
        "editingFinished",
        Qt::DirectConnection));

    QCOMPARE(
        field.date(),
        QDate(2026, 7, 14));
    QCOMPARE(dateSpy.count(), 1);
}

void tst_DateFieldAffordances::
calendarActionIsAvailableWhenEmpty()
{
    QtMaterialDateField field;
    QSignalSpy calendarSpy(
        &field,
        &QtMaterialDateField::calendarRequested);

    auto* actionButton =
        field.findChild<QToolButton*>(
            QStringLiteral("endActionButton"));
    QVERIFY(actionButton);
    QVERIFY(actionButton->isVisibleTo(&field)
        || !field.isVisible());

    actionButton->click();

    QCOMPARE(calendarSpy.count(), 1);
}

void tst_DateFieldAffordances::clearActionClearsDate()
{
    QtMaterialDateField field;
    field.setClearable(true);
    field.setDate(QDate(2026, 7, 14));

    auto* actionButton =
        field.findChild<QToolButton*>(
            QStringLiteral("endActionButton"));
    QVERIFY(actionButton);

    actionButton->click();

    QVERIFY(!field.date().isValid());
    QVERIFY(field.text().isEmpty());
    QVERIFY(field.isDateAcceptable());
}

void tst_DateFieldAffordances::
clearDoesNotEmitSpuriousParseErrorSignal()
{
    QtMaterialDateField field;
    field.setClearable(true);
    field.setDate(QDate(2026, 7, 14));

    QSignalSpy parseSpy(
        &field,
        &QtMaterialDateField::parseErrorChanged);

    auto* actionButton =
        field.findChild<QToolButton*>(
            QStringLiteral("endActionButton"));
    QVERIFY(actionButton);
    actionButton->click();

    QCOMPARE(parseSpy.count(), 0);
}

void tst_DateFieldAffordances::resolvedSpecFollowsThemeContext()
{
    ThemeBuilder builder;
    Theme theme =
        builder.buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));

    ComponentTokenOverride tokens;
    tokens.custom.insert(
        QStringLiteral("actionIconSize"),
        26);
    theme.componentOverrides().setOverride(
        QStringLiteral("dateField"),
        tokens);

    ThemeContext context(theme);
    QtMaterialDateField field;
    field.setThemeContext(&context);

    QCOMPARE(
        field.resolvedDateFieldSpec()
            .actionIconSize,
        26);
}

QTEST_MAIN(tst_DateFieldAffordances)

#include "tst_datefield_affordances.moc"
