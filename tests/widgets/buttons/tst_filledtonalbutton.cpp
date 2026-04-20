#include <QtTest/QtTest>

#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"

class tst_FilledTonalButton : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
    void keyboardActivation();
    void respectsTouchTargetHeight();
};

void tst_FilledTonalButton::constructs()
{
    QtMaterial::QtMaterialFilledTonalButton widget(QStringLiteral("Tonal"));
    QCOMPARE(widget.text(), QStringLiteral("Tonal"));
}

void tst_FilledTonalButton::keyboardActivation()
{
    QtMaterial::QtMaterialFilledTonalButton widget(QStringLiteral("Tonal"));
    widget.resize(widget.sizeHint());
    widget.show();
    QVERIFY(QTest::qWaitForWindowExposed(&widget));
    widget.setFocus();
    QVERIFY(widget.hasFocus());

    QSignalSpy clickedSpy(&widget, &QAbstractButton::clicked);
    QVERIFY(clickedSpy.isValid());

    QTest::keyClick(&widget, Qt::Key_Return);
    QCOMPARE(clickedSpy.count(), 1);
}

void tst_FilledTonalButton::respectsTouchTargetHeight()
{
    QtMaterial::QtMaterialFilledTonalButton widget(QStringLiteral("Tonal"));
    QVERIFY(widget.sizeHint().height() >= 48);
    // QVERIFY(widget.minimumSizeHint().height() >= 48);
}

QTEST_MAIN(tst_FilledTonalButton)
#include "tst_filledtonalbutton.moc"
