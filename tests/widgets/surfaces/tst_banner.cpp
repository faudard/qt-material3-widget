#include <QtTest/QtTest>

#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"

class tst_QtMaterialBanner : public QObject
{
    Q_OBJECT

private slots:
    void constructs();
    void apiRoundTrip();
    void dismissSignal();
};

void tst_QtMaterialBanner::constructs()
{
    QtMaterialBanner banner;
    QVERIFY(banner.isDismissible());
    QVERIFY(banner.minimumSizeHint().height() > 0);
}

void tst_QtMaterialBanner::apiRoundTrip()
{
    QtMaterialBanner banner;
    banner.setTitleText(QStringLiteral("Title"));
    banner.setBodyText(QStringLiteral("Body"));
    banner.setDismissible(false);

    QCOMPARE(banner.titleText(), QStringLiteral("Title"));
    QCOMPARE(banner.bodyText(), QStringLiteral("Body"));
    QVERIFY(!banner.isDismissible());
}

void tst_QtMaterialBanner::dismissSignal()
{
    QtMaterialBanner banner;
    QSignalSpy spy(&banner, &QtMaterialBanner::dismissed);
    QVERIFY(spy.isValid());

    banner.dismiss();
    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(tst_QtMaterialBanner)
#include "tst_banner.moc"
