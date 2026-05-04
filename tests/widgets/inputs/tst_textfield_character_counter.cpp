#include <QtTest/QtTest>
#include <QLabel>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

using namespace QtMaterial;

class tst_textfield_character_counter : public QObject
{
    Q_OBJECT

private slots:
    void maxLengthDelegatesToInternalLineEdit();
    void counterIsDisabledByDefault();
    void enablingCounterCreatesStableLabel();
    void counterTextUpdatesWhenTextChanges();
    void disablingCounterHidesLabel();
    void filledTextFieldInheritsCounterApi();
};

void tst_textfield_character_counter::maxLengthDelegatesToInternalLineEdit()
{
    QtMaterialOutlinedTextField field;

    field.setMaxLength(5);
    QCOMPARE(field.maxLength(), 5);

    field.setText(QStringLiteral("abcdef"));
    QCOMPARE(field.text(), QStringLiteral("abcde"));
    QCOMPARE(field.characterCounterText(), QStringLiteral("5/5"));
}

void tst_textfield_character_counter::counterIsDisabledByDefault()
{
    QtMaterialOutlinedTextField field;

    QVERIFY(!field.isCharacterCounterEnabled());
    QVERIFY(field.findChild<QLabel*>(QStringLiteral("qtmaterial_textfield_characterCounter")) == nullptr);
}

void tst_textfield_character_counter::enablingCounterCreatesStableLabel()
{
    QtMaterialOutlinedTextField field;
    field.resize(240, 80);
    field.setMaxLength(8);
    field.setText(QStringLiteral("abc"));
    field.setCharacterCounterEnabled(true);
    field.show();
    QVERIFY(QTest::qWaitForWindowExposed(&field));
    field.repaint();
    QCoreApplication::processEvents();

    auto* counter = field.findChild<QLabel*>(QStringLiteral("qtmaterial_textfield_characterCounter"));
    QVERIFY(counter != nullptr);
    QCOMPARE(counter->text(), QStringLiteral("3/8"));
    QVERIFY(counter->isVisible());
    QVERIFY(counter->geometry().isValid());
}

void tst_textfield_character_counter::counterTextUpdatesWhenTextChanges()
{
    QtMaterialOutlinedTextField field;
    field.resize(240, 80);
    field.setMaxLength(12);
    field.setCharacterCounterEnabled(true);
    field.show();
    QVERIFY(QTest::qWaitForWindowExposed(&field));

    field.setText(QStringLiteral("material"));
    field.repaint();
    QCoreApplication::processEvents();

    QCOMPARE(field.characterCounterText(), QStringLiteral("8/12"));
    auto* counter = field.findChild<QLabel*>(QStringLiteral("qtmaterial_textfield_characterCounter"));
    QVERIFY(counter != nullptr);
    QCOMPARE(counter->text(), QStringLiteral("8/12"));
}

void tst_textfield_character_counter::disablingCounterHidesLabel()
{
    QtMaterialOutlinedTextField field;
    field.resize(240, 80);
    field.setMaxLength(4);
    field.setText(QStringLiteral("ab"));
    field.setCharacterCounterEnabled(true);
    field.show();
    QVERIFY(QTest::qWaitForWindowExposed(&field));

    auto* counter = field.findChild<QLabel*>(QStringLiteral("qtmaterial_textfield_characterCounter"));
    QVERIFY(counter != nullptr);

    field.setCharacterCounterEnabled(false);
    QCoreApplication::processEvents();
    QVERIFY(!counter->isVisible());
}

void tst_textfield_character_counter::filledTextFieldInheritsCounterApi()
{
    QtMaterialFilledTextField field;

    field.setMaxLength(6);
    field.setText(QStringLiteral("abcdefghi"));
    field.setCharacterCounterEnabled(true);

    QCOMPARE(field.maxLength(), 6);
    QCOMPARE(field.text(), QStringLiteral("abcdef"));
    QCOMPARE(field.characterCounterText(), QStringLiteral("6/6"));
    QVERIFY(field.isCharacterCounterEnabled());
}

QTEST_MAIN(tst_textfield_character_counter)
#include "tst_textfield_character_counter.moc"
