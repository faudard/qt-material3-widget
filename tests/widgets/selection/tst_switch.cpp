#include <QtTest/QtTest>

#include "qtmaterial/widgets/selection/qtmaterialswitch.h"

class tst_Switch : public QObject
{
    Q_OBJECT

private slots:
    void basicConstruction();
    void keyboardToggle();
    void sizeHintRespectsTouchTarget();
};

void tst_Switch::basicConstruction()
{
    QtMaterial::QtMaterialSwitch widget;
    QVERIFY(widget.isCheckable());
}

void tst_Switch::keyboardToggle()
{
    QtMaterial::QtMaterialSwitch widget;
    widget.resize(120, 48);
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));
    widget.setFocus();
    QVERIFY(widget.hasFocus());

    QCOMPARE(widget.isChecked(), false);
    QTest::keyClick(&widget, Qt::Key_Space);
    QCOMPARE(widget.isChecked(), true);
    QTest::keyClick(&widget, Qt::Key_Space);
    QCOMPARE(widget.isChecked(), false);
}

void tst_Switch::sizeHintRespectsTouchTarget()
{
    QtMaterial::QtMaterialSwitch widget;
    const QSize hint = widget.sizeHint();
    QVERIFY(hint.height() >= 32);
    QVERIFY(hint.width() >= 52);
}

QTEST_MAIN(tst_Switch)
#include "tst_switch.moc"
