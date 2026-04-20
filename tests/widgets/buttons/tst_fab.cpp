#include <QtTest/QtTest>

#include "qtmaterial/widgets/buttons/qtmaterialfab.h"

class tst_Fab : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
    void keyboardActivation();
    void respectsTouchTargetSize();
};

void tst_Fab::constructs()
{
    QtMaterial::QtMaterialFab widget;
    QVERIFY(widget.text().isEmpty());
}

void tst_Fab::keyboardActivation()
{
    QtMaterial::QtMaterialFab widget;
    widget.setIcon(QIcon());
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

void tst_Fab::respectsTouchTargetSize()
{
    QtMaterial::QtMaterialFab widget;
    QVERIFY(widget.sizeHint().height() >= 56);
    QVERIFY(widget.sizeHint().width() >= 56);
    QVERIFY(widget.minimumSizeHint().height() >= 56);
    QVERIFY(widget.minimumSizeHint().width() >= 56);
}

QTEST_MAIN(tst_Fab)
#include "tst_fab.moc"
