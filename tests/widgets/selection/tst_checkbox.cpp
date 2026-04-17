#include <QtTest/QtTest>

#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"

class tst_Checkbox : public QObject
{
    Q_OBJECT

private slots:
    void basicConstruction();
    void togglesOnClick();
    void hasTouchTargetSizedHint();
};

void tst_Checkbox::basicConstruction()
{
    QtMaterial::QtMaterialCheckbox widget;
    widget.setText(QStringLiteral("Checkbox"));

    QVERIFY(widget.isCheckable());
    QVERIFY(!widget.text().isEmpty());
}

void tst_Checkbox::togglesOnClick()
{
    QtMaterial::QtMaterialCheckbox widget;
    widget.setText(QStringLiteral("Checkbox"));
    widget.resize(widget.sizeHint());
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));

    QVERIFY(!widget.isChecked());
    QTest::mouseClick(&widget, Qt::LeftButton, Qt::NoModifier, widget.rect().center());
    QVERIFY(widget.isChecked());
}

void tst_Checkbox::hasTouchTargetSizedHint()
{
    QtMaterial::QtMaterialCheckbox widget(QStringLiteral("Checkbox"));
    const QSize hint = widget.sizeHint();

    QVERIFY(hint.height() >= 48);
    QVERIFY(hint.width() >= 48);
}

QTEST_MAIN(tst_Checkbox)
#include "tst_checkbox.moc"
