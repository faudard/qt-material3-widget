#include <QtTest/QtTest>

#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"

class tst_QtMaterialBottomSheet : public QObject
{
    Q_OBJECT

private slots:
    void construction();
    void modalApi();
    void escapeCloses();
};

void tst_QtMaterialBottomSheet::construction()
{
    QtMaterial::QtMaterialBottomSheet sheet;
    QVERIFY(!sheet.isOpen());
    QVERIFY(sheet.expandedHeight() >= 120);
}

void tst_QtMaterialBottomSheet::modalApi()
{
    QtMaterial::QtMaterialBottomSheet sheet;
    sheet.setModal(false);
    QVERIFY(!sheet.isModal());
    sheet.setModal(true);
    QVERIFY(sheet.isModal());
}

void tst_QtMaterialBottomSheet::escapeCloses()
{
    QWidget host;
    host.resize(800, 600);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterial::QtMaterialBottomSheet sheet(&host);
    sheet.open();
    QVERIFY(sheet.isVisible());

    QTest::keyClick(&sheet, Qt::Key_Escape);
    QTRY_VERIFY(!sheet.isVisible() || !sheet.isOpen());
}

QTEST_MAIN(tst_QtMaterialBottomSheet)
#include "tst_bottomsheet.moc"
