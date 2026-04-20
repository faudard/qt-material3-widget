#include <QtTest/QtTest>

#include "qtmaterial/widgets/selection/qtmaterialswitch.h"

class tst_QtMaterialSwitch : public QObject
{
    Q_OBJECT

private slots:
    void construction();
    void keyboardToggle();
    void arrowKeysSetState();
    void sizeHintRespectsTouchTarget();
};

void tst_QtMaterialSwitch::construction()
{
    QtMaterial::QtMaterialSwitch w;
    QVERIFY(w.isCheckable());
}

void tst_QtMaterialSwitch::keyboardToggle()
{
    QtMaterial::QtMaterialSwitch w;
    w.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w));
    w.setFocus();

    QCOMPARE(w.isChecked(), false);
    QTest::keyClick(&w, Qt::Key_Space);
    QCOMPARE(w.isChecked(), true);
}

void tst_QtMaterialSwitch::arrowKeysSetState()
{
    QtMaterial::QtMaterialSwitch w(QStringLiteral("Wi‑Fi"));
    w.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w));
    w.setFocus();

    w.setChecked(false);
    QTest::keyClick(&w, Qt::Key_Right);
    QCOMPARE(w.isChecked(), true);

    QTest::keyClick(&w, Qt::Key_Left);
    QCOMPARE(w.isChecked(), false);
}

void tst_QtMaterialSwitch::sizeHintRespectsTouchTarget()
{
    QtMaterial::QtMaterialSwitch w(QStringLiteral("Airplane mode"));
    const QSize hint = w.sizeHint();
    QVERIFY(hint.height() >= 32);
    QVERIFY(hint.width() >= 52);
}

QTEST_MAIN(tst_QtMaterialSwitch)
#include "tst_switch.moc"
