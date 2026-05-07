#include "qtmaterial/widgets/data/qtmaterialcarousel.h"

#include <QtTest/QtTest>

using namespace QtMaterial;

class tst_Carousel : public QObject
{
    Q_OBJECT

private slots:
    void constructsEmpty();
    void addItemsSelectsFirstItem();
    void setCurrentIndexSkipsDisabledItems();
    void keyboardNavigationWrapsAndSkipsDisabledItems();
    void activationEmitsCurrentIndex();
    void accessibilitySummaryTracksCurrentItem();
    void visibleItemCountIsClamped();
};

void tst_Carousel::constructsEmpty()
{
    QtMaterialCarousel carousel;

    QCOMPARE(carousel.count(), 0);
    QCOMPARE(carousel.currentIndex(), -1);
    QVERIFY(carousel.isEmpty());
    QVERIFY(carousel.accessibilitySummary().contains(QStringLiteral("empty")));
    QVERIFY(!carousel.sizeHint().isEmpty());
}

void tst_Carousel::addItemsSelectsFirstItem()
{
    QtMaterialCarousel carousel;
    QSignalSpy countSpy(&carousel, &QtMaterialCarousel::itemCountChanged);
    QSignalSpy currentSpy(&carousel, &QtMaterialCarousel::currentIndexChanged);

    QCOMPARE(carousel.addItem(QStringLiteral("One")), 0);
    QCOMPARE(carousel.addItem(QStringLiteral("Two"), QStringLiteral("Second card")), 1);

    QCOMPARE(carousel.count(), 2);
    QCOMPARE(carousel.currentIndex(), 0);
    QCOMPARE(countSpy.count(), 2);
    QVERIFY(currentSpy.count() >= 1);
    QCOMPARE(carousel.itemTitle(1), QStringLiteral("Two"));
    QCOMPARE(carousel.itemSupportingText(1), QStringLiteral("Second card"));
}

void tst_Carousel::setCurrentIndexSkipsDisabledItems()
{
    QtMaterialCarousel carousel;
    carousel.addItem(QStringLiteral("One"));
    carousel.addItem(QStringLiteral("Two"));
    carousel.addItem(QStringLiteral("Three"));

    carousel.setItemEnabled(1, false);
    carousel.setCurrentIndex(1);
    QCOMPARE(carousel.currentIndex(), 0);

    carousel.setCurrentIndex(2);
    QCOMPARE(carousel.currentIndex(), 2);
}

void tst_Carousel::keyboardNavigationWrapsAndSkipsDisabledItems()
{
    QtMaterialCarousel carousel;
    carousel.addItem(QStringLiteral("One"));
    carousel.addItem(QStringLiteral("Two"));
    carousel.addItem(QStringLiteral("Three"));
    carousel.setItemEnabled(1, false);
    carousel.resize(480, 180);
    carousel.show();
    QVERIFY(QTest::qWaitForWindowExposed(&carousel));
    carousel.setFocus();

    QTest::keyClick(&carousel, Qt::Key_Right);
    QCOMPARE(carousel.currentIndex(), 2);

    QTest::keyClick(&carousel, Qt::Key_Right);
    QCOMPARE(carousel.currentIndex(), 0);

    carousel.setWrapAround(false);
    QTest::keyClick(&carousel, Qt::Key_Left);
    QCOMPARE(carousel.currentIndex(), 0);

    QTest::keyClick(&carousel, Qt::Key_End);
    QCOMPARE(carousel.currentIndex(), 2);
}

void tst_Carousel::activationEmitsCurrentIndex()
{
    QtMaterialCarousel carousel;
    carousel.addItem(QStringLiteral("One"));
    carousel.addItem(QStringLiteral("Two"));
    carousel.setCurrentIndex(1);

    QSignalSpy activatedSpy(&carousel, &QtMaterialCarousel::itemActivated);
    carousel.activateCurrentItem();

    QCOMPARE(activatedSpy.count(), 1);
    QCOMPARE(activatedSpy.takeFirst().at(0).toInt(), 1);
}

void tst_Carousel::accessibilitySummaryTracksCurrentItem()
{
    QtMaterialCarousel carousel;
    QSignalSpy summarySpy(&carousel, &QtMaterialCarousel::accessibilitySummaryChanged);

    carousel.addItem(QStringLiteral("Alpha"), QStringLiteral("First item"));
    carousel.addItem(QStringLiteral("Beta"));
    carousel.setCurrentIndex(1);

    QVERIFY(carousel.currentItemAccessibleText().contains(QStringLiteral("Beta")));
    QVERIFY(carousel.accessibilitySummary().contains(QStringLiteral("2 items")));
    QVERIFY(carousel.accessibilitySummary().contains(QStringLiteral("Beta")));
    QVERIFY(summarySpy.count() >= 1);
}

void tst_Carousel::visibleItemCountIsClamped()
{
    QtMaterialCarousel carousel;

    carousel.setVisibleItemCount(0);
    QCOMPARE(carousel.visibleItemCount(), 1);

    carousel.setVisibleItemCount(42);
    QCOMPARE(carousel.visibleItemCount(), 9);
}

QTEST_MAIN(tst_Carousel)
#include "tst_carousel.moc"
