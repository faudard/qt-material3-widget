#include <QtTest/QtTest>

#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"

class tst_QtMaterialBottomSheetRender : public QObject
{
    Q_OBJECT

private slots:
    void defaultLightBaseline();
};

void tst_QtMaterialBottomSheetRender::defaultLightBaseline()
{
    QWidget host;
    host.resize(900, 700);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));

    QtMaterialBottomSheet sheet(&host);
    sheet.open();
    sheet.resize(host.size());
    sheet.show();
    QVERIFY(QTest::qWaitForWindowExposed(&sheet));

    const QImage image = sheet.grab().toImage();
    QVERIFY(!image.isNull());
    QVERIFY(image.width() > 0);
    QVERIFY(image.height() > 0);
}

QTEST_MAIN(tst_QtMaterialBottomSheetRender)
#include "tst_bottomsheet_render.moc"
