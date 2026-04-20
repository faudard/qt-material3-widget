#include <QtTest/QtTest>

#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"

class TestQtMaterialOutlinedButton : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
    void keyboardClick();
    void sizeHintHasTouchTargetHeight();
};

void TestQtMaterialOutlinedButton::constructs()
{
    QtMaterial::QtMaterialOutlinedButton widget;
    QVERIFY(true);
}

void TestQtMaterialOutlinedButton::keyboardClick()
{
    QtMaterial::QtMaterialOutlinedButton widget;
    widget.setText(QStringLiteral("Outlined"));
    widget.resize(widget.sizeHint());
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));
    widget.setFocus();

    QSignalSpy clickedSpy(&widget, &QAbstractButton::clicked);
    QVERIFY(clickedSpy.isValid());

    QTest::keyClick(&widget, Qt::Key_Return);
    QCOMPARE(clickedSpy.count(), 1);
}

void TestQtMaterialOutlinedButton::sizeHintHasTouchTargetHeight()
{
    QtMaterial::QtMaterialOutlinedButton widget;
    widget.setText(QStringLiteral("Outlined"));
    QVERIFY(widget.sizeHint().height() >= 48);
}

QTEST_MAIN(TestQtMaterialOutlinedButton)
#include "tst_outlinedbutton.moc"
