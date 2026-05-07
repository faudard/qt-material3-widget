#include <QtTest/QtTest>

#include "qtmaterial/widgets/buttons/qtmaterialfab.h"

class tst_Fab : public QObject {
    Q_OBJECT

private slots:
    void constructs();
    void keyboardActivation();
    void respectsTouchTargetSize();
    void accessibleNameCanComeFromExplicitIconLabel();
    void accessibleNameCanComeFromTooltip();
    void accessibleNameGuardCanBeRelaxed();
};

void tst_Fab::constructs()
{
    QtMaterial::QtMaterialFab widget;
    QVERIFY(widget.text().isEmpty());
    QVERIFY(widget.requiresAccessibleName());
    QVERIFY(!widget.hasUsableAccessibleName());
}

void tst_Fab::keyboardActivation()
{
    QtMaterial::QtMaterialFab widget;
    widget.setIconAccessibleName(QStringLiteral("Create"));
    widget.resize(widget.sizeHint());
    widget.show();

    QVERIFY(QTest::qWaitForWindowExposed(&widget));

    widget.setFocus();
    QVERIFY(widget.hasFocus());

    QSignalSpy clickedSpy(&widget, &QAbstractButton::clicked);
    QVERIFY(clickedSpy.isValid());

    QTest::keyClick(&widget, Qt::Key_Return);
    QTest::keyClick(&widget, Qt::Key_Space);

    QCOMPARE(clickedSpy.count(), 2);
}

void tst_Fab::respectsTouchTargetSize()
{
    QtMaterial::QtMaterialFab widget;
    QVERIFY(widget.sizeHint().height() >= 56);
    QVERIFY(widget.sizeHint().width() >= 56);
    QVERIFY(widget.minimumSizeHint().height() >= 56);
    QVERIFY(widget.minimumSizeHint().width() >= 56);
}

void tst_Fab::accessibleNameCanComeFromExplicitIconLabel()
{
    QtMaterial::QtMaterialFab widget;
    QSignalSpy summarySpy(&widget, &QtMaterial::QtMaterialFab::accessibilitySummaryChanged);
    QVERIFY(summarySpy.isValid());

    widget.setIconAccessibleName(QStringLiteral("Create item"));

    QCOMPARE(widget.iconAccessibleName(), QStringLiteral("Create item"));
    QCOMPARE(widget.effectiveAccessibleName(), QStringLiteral("Create item"));
    QVERIFY(widget.hasUsableAccessibleName());
    QCOMPARE(widget.accessibleName(), QStringLiteral("Create item"));
    QVERIFY(summarySpy.count() >= 1);
}

void tst_Fab::accessibleNameCanComeFromTooltip()
{
    QtMaterial::QtMaterialFab widget;
    widget.setToolTip(QStringLiteral("Compose"));
    QCOMPARE(widget.effectiveAccessibleName(), QStringLiteral("Compose"));
    QVERIFY(widget.hasUsableAccessibleName());
}

void tst_Fab::accessibleNameGuardCanBeRelaxed()
{
    QtMaterial::QtMaterialFab widget;
    widget.setRequiresAccessibleName(false);

    QVERIFY(!widget.requiresAccessibleName());
    QCOMPARE(widget.effectiveAccessibleName(), QStringLiteral("Floating action button"));
    QVERIFY(widget.hasUsableAccessibleName());
}

QTEST_MAIN(tst_Fab)
#include "tst_fab.moc"
