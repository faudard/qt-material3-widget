#include <QtTest/QtTest>
#include <QSignalSpy>
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

class tst_Filledbutton : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
    void keyboardActivation();
};

void tst_Filledbutton::basicConstruction()
{
    QtMaterial::QtMaterialFilledButton widget;
    widget.setText(QStringLiteral("Filled"));
    QVERIFY(widget.sizeHint().height() >= 40);
    QCOMPARE(widget.focusPolicy(), Qt::StrongFocus);
}

void tst_Filledbutton::keyboardActivation()
{
    QtMaterial::QtMaterialFilledButton widget;
    widget.setText(QStringLiteral("Filled"));
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));
    widget.setFocus();

    QSignalSpy clickedSpy(&widget, &QAbstractButton::clicked);
    QVERIFY(clickedSpy.isValid());

    QTest::keyClick(&widget, Qt::Key_Space);
    QVERIFY(clickedSpy.count() >= 1);

    QTest::keyClick(&widget, Qt::Key_Return);
    QVERIFY(clickedSpy.count() >= 2);
}

QTEST_MAIN(tst_Filledbutton)
#include "tst_filledbutton.moc"
