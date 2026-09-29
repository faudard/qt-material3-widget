#include <QtTest/QtTest>

#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"

class tst_RadioButton : public QObject
{
    Q_OBJECT

private slots:
    void basicConstruction();
    void keyboardToggle();
    void sizeHintRespectsTouchTarget();
};

void tst_RadioButton::basicConstruction()
{
    QtMaterial::QtMaterialRadioButton widget;
    QVERIFY(widget.isCheckable());
}

void tst_RadioButton::keyboardToggle()
{
    QtMaterial::QtMaterialRadioButton widget;
    widget.resize(140, 48);
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));
    QCOMPARE(widget.focusPolicy(), Qt::StrongFocus);

    // QTest::keyClick sends the key event directly to the widget. Requiring
    // native window activation/focus here makes this unit test flaky on
    // headless macOS runners without testing any RadioButton behavior.
    QCOMPARE(widget.isChecked(), false);
    QTest::keyClick(&widget, Qt::Key_Space);
    QCOMPARE(widget.isChecked(), true);
}

void tst_RadioButton::sizeHintRespectsTouchTarget()
{
    QtMaterial::QtMaterialRadioButton widget;
    const QSize hint = widget.sizeHint();
    QVERIFY(hint.height() >= 48);
    QVERIFY(hint.width() >= 48);
}

QTEST_MAIN(tst_RadioButton)
#include "tst_radiobutton.moc"
