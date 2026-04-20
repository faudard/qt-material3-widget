#include <QtTest/QtTest>
#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"

class tst_Checkbox : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
    void keyboardToggle();
};

void tst_Checkbox::basicConstruction()
{
    QtMaterial::QtMaterialCheckbox widget;
    widget.setText(QStringLiteral("Checkbox"));
    QVERIFY(widget.isCheckable());
    QCOMPARE(widget.focusPolicy(), Qt::StrongFocus);
}

void tst_Checkbox::keyboardToggle()
{
    QtMaterial::QtMaterialCheckbox widget;
    widget.setText(QStringLiteral("Checkbox"));
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));
    widget.setFocus();

    QVERIFY(!widget.isChecked());
    QTest::keyClick(&widget, Qt::Key_Space);
    QVERIFY(widget.isChecked());
}

QTEST_MAIN(tst_Checkbox)
#include "tst_checkbox.moc"
