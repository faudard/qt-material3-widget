#include <QtTest/QtTest>

#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"

class tst_QtMaterialBannerRender : public QObject
{
    Q_OBJECT

private slots:
    void defaultBannerRenders();
};

void tst_QtMaterialBannerRender::defaultBannerRenders()
{
    QtMaterialBanner banner(QStringLiteral("Banner title"),
                            QStringLiteral("Banner body content"));
    banner.resize(420, 96);
    banner.show();
    QVERIFY(QTest::qWaitForWindowExposed(&banner));
    QCOMPARE(banner.width(), 420);
}

QTEST_MAIN(tst_QtMaterialBannerRender)
#include "tst_banner_render.moc"
