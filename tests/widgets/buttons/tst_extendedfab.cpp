#include <QtTest/QtTest>

#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"

class tst_ExtendedFab : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
    void keyboardActivation();
    void respectsTouchTargetSize();
};

void tst_ExtendedFab::constructs()
{
    QtMaterial::QtMaterialExtendedFab widget(QIcon(), QStringLiteral("Compose"));
    QCOMPARE(widget.text(), QStringLiteral("Compose"));
}

void tst_ExtendedFab::keyboardActivation()
{
    QtMaterial::QtMaterialExtendedFab widget(QIcon(), QStringLiteral("Compose"));
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

void tst_ExtendedFab::respectsTouchTargetSize()
{
    QtMaterial::QtMaterialExtendedFab widget(QIcon(), QStringLiteral("Compose"));
    QVERIFY(widget.sizeHint().height() >= 56);
    QVERIFY(widget.sizeHint().width() >= 80);
    QVERIFY(widget.minimumSizeHint().height() >= 56);
}

QTEST_MAIN(tst_ExtendedFab)
#include "tst_extendedfab.moc"
