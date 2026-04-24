#include <QtTest>
#include <QPushButton>

class tst_accessibility_smoke : public QObject
{
    Q_OBJECT

private slots:
    void keyboardActivation_smoke()
    {
        QPushButton button;
        button.setText("Action");
        button.show();
        QVERIFY(QTest::qWaitForWindowExposed(&button));

        button.setFocus();
        QVERIFY(button.hasFocus());

        QSignalSpy clickedSpy(&button, &QPushButton::clicked);
        QTest::keyClick(&button, Qt::Key_Space);
        QVERIFY(clickedSpy.count() >= 1);
    }
};

QTEST_MAIN(tst_accessibility_smoke)
#include "tst_accessibility_smoke.moc"
