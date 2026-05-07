#include <QtTest/QtTest>

#include <QLineEdit>
#include <QSignalSpy>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

using namespace QtMaterial;

class tst_TextFieldDirtyState : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void outlinedFieldStartsUnmodified();
    void setModifiedUpdatesStateAndSignal();
    void resetModifiedClearsStateAndSignal();
    void setTextResetsModifiedState();
    void userEditingMarksFieldModifiedOnce();
    void filledFieldInheritsDirtyStateApi();
};

void tst_TextFieldDirtyState::outlinedFieldStartsUnmodified()
{
    QtMaterialOutlinedTextField field;

    QVERIFY(field.lineEdit());
    QCOMPARE(field.isModified(), false);
    QCOMPARE(field.lineEdit()->isModified(), false);
}

void tst_TextFieldDirtyState::setModifiedUpdatesStateAndSignal()
{
    QtMaterialOutlinedTextField field;
    QSignalSpy spy(&field, &QtMaterialOutlinedTextField::modifiedChanged);

    field.setModified(true);

    QCOMPARE(field.isModified(), true);
    QCOMPARE(field.lineEdit()->isModified(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toBool(), true);

    field.setModified(true);
    QCOMPARE(spy.count(), 0);
}

void tst_TextFieldDirtyState::resetModifiedClearsStateAndSignal()
{
    QtMaterialOutlinedTextField field;
    field.setModified(true);

    QSignalSpy spy(&field, &QtMaterialOutlinedTextField::modifiedChanged);
    field.resetModified();

    QCOMPARE(field.isModified(), false);
    QCOMPARE(field.lineEdit()->isModified(), false);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toBool(), false);
}

void tst_TextFieldDirtyState::setTextResetsModifiedState()
{
    QtMaterialOutlinedTextField field;
    field.setModified(true);

    QSignalSpy spy(&field, &QtMaterialOutlinedTextField::modifiedChanged);
    field.setText(QStringLiteral("server value"));

    QCOMPARE(field.text(), QStringLiteral("server value"));
    QCOMPARE(field.isModified(), false);
    QCOMPARE(field.lineEdit()->isModified(), false);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toBool(), false);
}

void tst_TextFieldDirtyState::userEditingMarksFieldModifiedOnce()
{
    QtMaterialOutlinedTextField field;
    field.resize(240, 80);
    field.show();
    QVERIFY(QTest::qWaitForWindowExposed(&field));

    QVERIFY(field.lineEdit());
    field.lineEdit()->setFocus();
    QVERIFY(QTest::qWaitForWindowActive(&field) || field.lineEdit()->hasFocus());

    QSignalSpy spy(&field, &QtMaterialOutlinedTextField::modifiedChanged);
    QTest::keyClicks(field.lineEdit(), QStringLiteral("abc"));

    QCOMPARE(field.text(), QStringLiteral("abc"));
    QCOMPARE(field.isModified(), true);
    QCOMPARE(field.lineEdit()->isModified(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toBool(), true);
}

void tst_TextFieldDirtyState::filledFieldInheritsDirtyStateApi()
{
    QtMaterialFilledTextField field;

    QCOMPARE(field.isModified(), false);

    QSignalSpy spy(&field, &QtMaterialOutlinedTextField::modifiedChanged);
    field.setModified(true);

    QCOMPARE(field.isModified(), true);
    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(tst_TextFieldDirtyState)
#include "tst_textfield_dirty_state.moc"
