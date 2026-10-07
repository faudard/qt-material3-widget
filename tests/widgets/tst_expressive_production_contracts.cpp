#include <QtTest/QtTest>

#include <QBoxLayout>
#include <QImage>
#include <QPainter>
#include <QSignalSpy>

#include "qtmaterial/widgets/buttons/qtmaterialbuttongroup.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialsplitbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
#include "qtmaterial/widgets/data/qtmateriallistitem.h"
#include "qtmaterial/widgets/data/qtmaterialsegmentedlist.h"
#include "qtmaterial/widgets/navigation/qtmaterialfloatingtoolbar.h"
#include "qtmaterial/widgets/progress/qtmaterialloadingindicator.h"

using namespace QtMaterial;

namespace {

QImage renderAtDpr2(QWidget* widget)
{
    const QSize logicalSize =
        widget->sizeHint().expandedTo(QSize(160, 96)).boundedTo(QSize(640, 480));
    widget->resize(logicalSize);
    widget->ensurePolished();
    widget->show();
    QCoreApplication::processEvents();

    QImage image(
        QSize(logicalSize.width() * 2, logicalSize.height() * 2),
        QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(2.0);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    widget->render(&painter);
    painter.end();
    widget->hide();
    return image;
}

bool hasPaintedPixel(const QImage& image)
{
    for (int y = 0; y < image.height(); y += 4) {
        for (int x = 0; x < image.width(); x += 4) {
            if (qAlpha(image.pixel(x, y)) != 0) {
                return true;
            }
        }
    }
    return false;
}

} // namespace

class ExpressiveProductionContractsTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void disabledAndAccessibilityContracts()
    {
        QtMaterialSplitButton split(QStringLiteral("Create"));
        QSignalSpy primarySpy(&split, &QtMaterialSplitButton::primaryTriggered);
        QSignalSpy secondarySpy(&split, &QtMaterialSplitButton::secondaryTriggered);
        split.setEnabled(false);
        QVERIFY(!split.primaryButton()->isEnabled());
        QVERIFY(!split.trailingButton()->isEnabled());
        split.primaryButton()->click();
        split.trailingButton()->click();
        QCOMPARE(primarySpy.count(), 0);
        QCOMPARE(secondarySpy.count(), 0);
        QVERIFY(!split.accessibleName().isEmpty());
        QVERIFY(!split.trailingButton()->accessibleName().isEmpty());

        QtMaterialFloatingToolbar toolbar;
        auto* edit = toolbar.addAction(QIcon(), QStringLiteral("Edit"));
        toolbar.addAction(QIcon(), QStringLiteral("Share"));
        QVERIFY(!toolbar.accessibleName().isEmpty());
        QCOMPARE(edit->accessibleName(), QStringLiteral("Edit"));
        toolbar.setExpanded(false);
        QVERIFY(!toolbar.itemAt(0)->isHidden());
        QVERIFY(toolbar.itemAt(1)->isHidden());

        QtMaterialLoadingIndicator loading;
        QVERIFY(!loading.accessibleName().isEmpty());
        const QString activeDescription = loading.accessibleDescription();
        loading.setActive(false);
        QVERIFY(!loading.accessibleDescription().isEmpty());
        QVERIFY(loading.accessibleDescription() != activeDescription);

        QtMaterialSegmentedList list;
        list.addItem(QStringLiteral("First"));
        list.addItem(QStringLiteral("Second"));
        QVERIFY(!list.accessibleName().isEmpty());
        QVERIFY(!list.accessibilitySummary().isEmpty());
    }

    void keyboardAndRtlContracts()
    {
        QtMaterialButtonGroup group;
        auto* first = group.addButton(QStringLiteral("One"));
        auto* second = group.addButton(QStringLiteral("Two"));
        group.addButton(QStringLiteral("Three"));
        group.resize(group.sizeHint());
        group.show();
        QCoreApplication::processEvents();

        first->setFocus(Qt::TabFocusReason);
        QTRY_VERIFY(first->hasFocus());
        QTest::keyClick(first, Qt::Key_Right);
        QTRY_VERIFY(second->hasFocus());

        group.setLayoutDirection(Qt::RightToLeft);
        auto* groupLayout = static_cast<QBoxLayout*>(group.layout());
        QCOMPARE(groupLayout->direction(), QBoxLayout::RightToLeft);
        second->setFocus(Qt::TabFocusReason);
        QTest::keyClick(second, Qt::Key_Right);
        QTRY_VERIFY(first->hasFocus());

        QtMaterialFloatingToolbar toolbar;
        auto* edit = toolbar.addAction(QIcon(), QStringLiteral("Edit"));
        auto* share = toolbar.addAction(QIcon(), QStringLiteral("Share"));
        toolbar.resize(toolbar.sizeHint());
        toolbar.show();
        QCoreApplication::processEvents();

        edit->setFocus(Qt::TabFocusReason);
        QTRY_VERIFY(edit->hasFocus());
        QTest::keyClick(edit, Qt::Key_Right);
        QTRY_VERIFY(share->hasFocus());

        toolbar.setLayoutDirection(Qt::RightToLeft);
        auto* toolbarLayout = static_cast<QBoxLayout*>(toolbar.layout());
        QCOMPARE(toolbarLayout->direction(), QBoxLayout::RightToLeft);
        share->setFocus(Qt::TabFocusReason);
        QTest::keyClick(share, Qt::Key_Right);
        QTRY_VERIFY(edit->hasFocus());

        QtMaterialSplitButton split(QStringLiteral("Create"));
        split.setLayoutDirection(Qt::RightToLeft);
        QCOMPARE(split.layoutDirection(), Qt::RightToLeft);
        QCOMPARE(split.primaryButton()->layoutDirection(), Qt::RightToLeft);
        QCOMPARE(split.trailingButton()->layoutDirection(), Qt::RightToLeft);

        QtMaterialSegmentedList list;
        list.addItem(QStringLiteral("First"));
        list.addItem(QStringLiteral("Second"));
        list.setLayoutDirection(Qt::RightToLeft);
        QCOMPARE(list.layoutDirection(), Qt::RightToLeft);
        QCOMPARE(list.itemAt(0)->layoutDirection(), Qt::RightToLeft);

        list.resize(list.sizeHint());
        list.show();
        list.setCurrentIndex(0);
        list.setFocus(Qt::TabFocusReason);
        QCoreApplication::processEvents();
        QTest::keyClick(&list, Qt::Key_Down);
        QCOMPARE(list.currentIndex(), 1);
    }

    void dpr2RenderingSmoke()
    {
        QtMaterialSplitButton split(QStringLiteral("Create"));
        const QImage splitImage = renderAtDpr2(&split);
        QCOMPARE(splitImage.devicePixelRatio(), qreal(2.0));
        QVERIFY(hasPaintedPixel(splitImage));

        QtMaterialButtonGroup group;
        group.addButton(QStringLiteral("Day"));
        group.addButton(QStringLiteral("Week"));
        const QImage groupImage = renderAtDpr2(&group);
        QCOMPARE(groupImage.devicePixelRatio(), qreal(2.0));
        QVERIFY(hasPaintedPixel(groupImage));

        QtMaterialFloatingToolbar toolbar;
        toolbar.addAction(QIcon(), QStringLiteral("Edit"));
        toolbar.addAction(QIcon(), QStringLiteral("Share"));
        const QImage toolbarImage = renderAtDpr2(&toolbar);
        QCOMPARE(toolbarImage.devicePixelRatio(), qreal(2.0));
        QVERIFY(hasPaintedPixel(toolbarImage));

        QtMaterialLoadingIndicator loading;
        loading.setActive(true);
        const QImage loadingImage = renderAtDpr2(&loading);
        QCOMPARE(loadingImage.devicePixelRatio(), qreal(2.0));
        QVERIFY(hasPaintedPixel(loadingImage));

        QtMaterialSegmentedList list;
        list.addItem(QStringLiteral("Personal"));
        list.addItem(QStringLiteral("Work"));
        const QImage listImage = renderAtDpr2(&list);
        QCOMPARE(listImage.devicePixelRatio(), qreal(2.0));
        QVERIFY(hasPaintedPixel(listImage));
    }
};

QTEST_MAIN(ExpressiveProductionContractsTest)
#include "tst_expressive_production_contracts.moc"
