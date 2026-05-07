#include <QtTest/QtTest>

#include <QFocusEvent>
#include <QLineEdit>
#include <QSignalSpy>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

using namespace QtMaterial;

class tst_TextFieldTouchedState : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void outlinedFieldStartsUntouched();
    void setTouchedUpdatesStateAndSignal();
    void resetTouchedClearsStateAndSignal();
    void programmaticSetTextDoesNotMarkTouched();
    void textEditedMarksFieldTouchedOnce();
    void focusOutMarksFieldTouched();
    void filledFieldInheritsTouchedStateApi();
};

void tst_TextFieldTouchedState::outlinedFieldStartsUntouched()
{
    QtMaterialOutlinedTextField field;

    QVERIFY(field.lineEdit());
    QCOMPARE(field.isTouched(), false);
}

void tst_TextFieldTouchedState::setTouchedUpdatesStateAndSignal()
{
    QtMaterialOutlinedTextField field;
    QSignalSpy spy(&field, &QtMaterialOutlinedTextField::touchedChanged);

    field.setTouched(true);

    QCOMPARE(field.isTouched(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toBool(), true);

    field.setTouched(true);
    QCOMPARE(spy.count(), 0);
}

void tst_TextFieldTouchedState::resetTouchedClearsStateAndSignal()
{
    QtMaterialOutlinedTextField field;
    field.setTouched(true);

    QSignalSpy spy(&field, &QtMaterialOutlinedTextField::touchedChanged);
    field.resetTouched();

    QCOMPARE(field.isTouched(), false);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toBool(), false);
}

void tst_TextFieldTouchedState::programmaticSetTextDoesNotMarkTouched()
{
    QtMaterialOutlinedTextField field;
    QSignalSpy spy(&field, &QtMaterialOutlinedTextField::touchedChanged);

    field.setText(QStringLiteral("server value"));

    QCOMPARE(field.text(), QStringLiteral("server value"));
    QCOMPARE(field.isTouched(), false);
    QCOMPARE(spy.count(), 0);
}

void tst_TextFieldTouchedState::textEditedMarksFieldTouchedOnce()
{
    QtMaterialOutlinedTextField field;
    QVERIFY(field.lineEdit());

    QSignalSpy spy(&field, &QtMaterialOutlinedTextField::touchedChanged);

    Q_EMIT field.lineEdit()->textEdited(QStringLiteral("a"));
    QCOMPARE(field.isTouched(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toBool(), true);

    Q_EMIT field.lineEdit()->textEdited(QStringLiteral("ab"));
    QCOMPARE(field.isTouched(), true);
    QCOMPARE(spy.count(), 0);
}

void tst_TextFieldTouchedState::focusOutMarksFieldTouched()
{
    QtMaterialOutlinedTextField field;
    QVERIFY(field.lineEdit());

    QSignalSpy spy(&field, &QtMaterialOutlinedTextField::touchedChanged);

    QFocusEvent focusOut(QEvent::FocusOut);
    QApplication::sendEvent(field.lineEdit(), &focusOut);

    QCOMPARE(field.isTouched(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toBool(), true);
}

void tst_TextFieldTouchedState::filledFieldInheritsTouchedStateApi()
{
    QtMaterialFilledTextField field;

    QCOMPARE(field.isTouched(), false);

    QSignalSpy spy(&field, &QtMaterialFilledTextField::touchedChanged);
    field.setTouched(true);

    QCOMPARE(field.isTouched(), true);
    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(tst_TextFieldTouchedState)
#include "tst_textfield_touched_state.moc"
