#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"

class tst_QtMaterialBottomSheet : public QObject
{
    Q_OBJECT

private slots:
    void construction();
    void modalApi();
    void expandedCollapsedApi();
    void accessibilitySummary();
    void escapePolicy();
    void initialFocusAndRestoreFocus();
    void dragDownCollapsesOrDismisses();
};

void tst_QtMaterialBottomSheet::construction()
{
    QtMaterial::QtMaterialBottomSheet sheet;
    QVERIFY(!sheet.isOpen());
    QCOMPARE(sheet.state(), QtMaterial::QtMaterialBottomSheet::SheetState::Closed);
    QVERIFY(sheet.expandedHeight() >= 120);
    QVERIFY(sheet.collapsedHeight() >= 56);
    QVERIFY(sheet.contentWidget());
    QVERIFY(sheet.dismissOnEscape());
    QVERIFY(sheet.dismissOnScrim());
    QVERIFY(sheet.isDragToDismissEnabled());
    QVERIFY(sheet.restoreFocusOnClose());
}

void tst_QtMaterialBottomSheet::modalApi()
{
    QtMaterial::QtMaterialBottomSheet sheet;
    QSignalSpy modalSpy(&sheet, &QtMaterial::QtMaterialBottomSheet::modalChanged);

    sheet.setModal(false);
    QVERIFY(!sheet.isModal());
    QCOMPARE(modalSpy.count(), 1);

    sheet.setModal(true);
    QVERIFY(sheet.isModal());
    QCOMPARE(modalSpy.count(), 2);
}

void tst_QtMaterialBottomSheet::expandedCollapsedApi()
{
    QtMaterial::QtMaterialBottomSheet sheet;
    QSignalSpy expandedSpy(&sheet, &QtMaterial::QtMaterialBottomSheet::expandedChanged);

    sheet.setExpandedHeight(420);
    QCOMPARE(sheet.expandedHeight(), 420);

    sheet.setCollapsedHeight(96);
    QCOMPARE(sheet.collapsedHeight(), 96);

    sheet.collapse();
    QVERIFY(!sheet.isExpanded());
    QCOMPARE(expandedSpy.count(), 1);

    sheet.expand();
    QVERIFY(sheet.isExpanded());
    QCOMPARE(expandedSpy.count(), 2);
}

void tst_QtMaterialBottomSheet::accessibilitySummary()
{
    QtMaterial::QtMaterialBottomSheet sheet;
    QSignalSpy spy(&sheet, &QtMaterial::QtMaterialBottomSheet::accessibilitySummaryChanged);

    sheet.setTitleText(QStringLiteral("Filters"));
    sheet.setSupportingText(QStringLiteral("3 options available"));

    const QString summary = sheet.accessibilitySummary();
    QVERIFY(summary.contains(QStringLiteral("Filters")));
    QVERIFY(summary.contains(QStringLiteral("3 options available")));
    QVERIFY(summary.contains(QStringLiteral("Expanded")));
    QCOMPARE(sheet.accessibleName(), QStringLiteral("Filters"));
    QVERIFY(sheet.accessibleDescription().contains(QStringLiteral("Filters")));
    QVERIFY(spy.count() >= 1);
}

void tst_QtMaterialBottomSheet::escapePolicy()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterial::QtMaterialBottomSheet sheet(&host);
    sheet.setDismissOnEscape(false);
    sheet.open();
    QVERIFY(sheet.isVisible());
    QVERIFY(sheet.isOpen());

    QTest::keyClick(&sheet, Qt::Key_Escape);
    QVERIFY(sheet.isOpen());

    sheet.setDismissOnEscape(true);
    QTest::keyClick(&sheet, Qt::Key_Escape);
    QTRY_VERIFY(!sheet.isOpen());
}

void tst_QtMaterialBottomSheet::initialFocusAndRestoreFocus()
{
    QWidget host;
    auto *layout = new QVBoxLayout(&host);
    auto *outsideButton = new QPushButton(QStringLiteral("Outside"), &host);
    layout->addWidget(outsideButton);
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    outsideButton->setFocus();
    QCOMPARE(QApplication::focusWidget(), outsideButton);

    QtMaterial::QtMaterialBottomSheet sheet(&host);
    auto *insideButton = new QPushButton(QStringLiteral("Inside"), sheet.contentWidget());
    insideButton->setFocusPolicy(Qt::StrongFocus);
    sheet.setInitialFocusWidget(insideButton);

    sheet.open();
    QTRY_COMPARE(QApplication::focusWidget(), insideButton);

    sheet.close();
    QTRY_VERIFY(!sheet.isOpen());
    QTRY_COMPARE(QApplication::focusWidget(), outsideButton);
}

void tst_QtMaterialBottomSheet::dragDownCollapsesOrDismisses()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterial::QtMaterialBottomSheet sheet(&host);
    sheet.setExpandedHeight(360);
    sheet.setCollapsedHeight(96);
    sheet.open();
    QVERIFY(sheet.isOpen());

    const QPoint start(sheet.width() / 2, host.height() - 300);
    const QPoint end(sheet.width() / 2, host.height() - 80);
    QTest::mousePress(&sheet, Qt::LeftButton, Qt::NoModifier, start);
    QTest::mouseMove(&sheet, end);
    QTest::mouseRelease(&sheet, Qt::LeftButton, Qt::NoModifier, end);

    QVERIFY(!sheet.isExpanded() || !sheet.isOpen());
}

QTEST_MAIN(tst_QtMaterialBottomSheet)
#include "tst_bottomsheet.moc"
