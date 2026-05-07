#include <QtTest/QtTest>

#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"

class tst_ExtendedFab : public QObject {
    Q_OBJECT

private slots:
    void constructsWithText();
    void constructsWithIconAndText();
    void keyboardActivation();
    void respectsTouchTargetSize();
    void accessibleNameFallsBackToText();
    void requiresTextOrAccessibleName();
};

void tst_ExtendedFab::constructsWithText()
{
    QtMaterial::QtMaterialExtendedFab widget(QStringLiteral("Compose"));
    QCOMPARE(widget.text(), QStringLiteral("Compose"));
    QCOMPARE(widget.effectiveAccessibleName(), QStringLiteral("Compose"));
    QVERIFY(widget.hasUsableAccessibleName());
}

void tst_ExtendedFab::constructsWithIconAndText()
{
    QtMaterial::QtMaterialExtendedFab widget(QIcon(), QStringLiteral("Compose"));
    QCOMPARE(widget.text(), QStringLiteral("Compose"));
    QCOMPARE(widget.effectiveAccessibleName(), QStringLiteral("Compose"));
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
    QTest::keyClick(&widget, Qt::Key_Space);

    QCOMPARE(clickedSpy.count(), 2);
}

void tst_ExtendedFab::respectsTouchTargetSize()
{
    QtMaterial::QtMaterialExtendedFab widget(QIcon(), QStringLiteral("Compose"));
    QVERIFY(widget.sizeHint().height() >= 56);
    QVERIFY(widget.sizeHint().width() >= 80);
    QVERIFY(widget.minimumSizeHint().height() >= 56);
}

void tst_ExtendedFab::accessibleNameFallsBackToText()
{
    QtMaterial::QtMaterialExtendedFab widget(QStringLiteral("Create task"));

    QCOMPARE(widget.effectiveAccessibleName(), QStringLiteral("Create task"));
    QCOMPARE(widget.accessibleName(), QStringLiteral("Create task"));
    QVERIFY(widget.accessibleDescription().contains(QStringLiteral("Extended floating action button")));
}

void tst_ExtendedFab::requiresTextOrAccessibleName()
{
    QtMaterial::QtMaterialExtendedFab widget;

    QVERIFY(widget.effectiveAccessibleName().isEmpty());
    QVERIFY(!widget.hasUsableAccessibleName());
    QVERIFY(widget.accessibilitySummary().contains(QStringLiteral("requires text")));
    QVERIFY(widget.accessibleDescription().contains(QStringLiteral("requires text")));
}

QTEST_MAIN(tst_ExtendedFab)
#include "tst_extendedfab.moc"
