#include <QtTest/QtTest>

#include <QBoxLayout>
#include <QSignalSpy>

#include "qtmaterial/widgets/buttons/qtmaterialbuttongroup.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialsplitbutton.h"
#include "qtmaterial/widgets/data/qtmateriallistitem.h"
#include "qtmaterial/widgets/data/qtmaterialsegmentedlist.h"
#include "qtmaterial/widgets/navigation/qtmaterialfloatingtoolbar.h"
#include "qtmaterial/widgets/navigation/qtmaterialmenu.h"
#include "qtmaterial/widgets/progress/qtmaterialloadingindicator.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"

using namespace QtMaterial;

class ExpressiveCatalogueTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void splitButtonSeparatesActions()
    {
        QtMaterialSplitButton split(QStringLiteral("Create"));
        QSignalSpy primarySpy(&split, &QtMaterialSplitButton::primaryTriggered);
        QSignalSpy secondarySpy(&split, &QtMaterialSplitButton::secondaryTriggered);

        split.primaryButton()->click();
        split.trailingButton()->click();

        QCOMPARE(primarySpy.count(), 1);
        QCOMPARE(secondarySpy.count(), 1);
        QVERIFY(split.expressive());
    }

    void buttonGroupOwnsSelection()
    {
        QtMaterialButtonGroup group;
        auto* first = group.addButton(QStringLiteral("Day"));
        auto* second = group.addButton(QStringLiteral("Week"));
        group.addButton(QStringLiteral("Month"));

        QSignalSpy currentSpy(&group, &QtMaterialButtonGroup::currentIndexChanged);
        second->click();

        QCOMPARE(group.count(), 3);
        QCOMPARE(group.currentIndex(), 1);
        QVERIFY(second->isChecked());
        QVERIFY(!first->isChecked());
        QCOMPARE(currentSpy.count(), 1);
    }

    void buttonGroupRemovalIsTeardownSafe()
    {
        QtMaterialButtonGroup group;
        auto* button = group.addButton(QStringLiteral("Temporary"));

        group.removeButton(button);
        delete button;

        QCOMPARE(group.count(), 0);
    }

    void rtlMirrorsExpressiveHorizontalLayouts()
    {
        QtMaterialButtonGroup group;
        auto* one = group.addButton(QStringLiteral("One"));
        auto* two = group.addButton(QStringLiteral("Two"));
        group.resize(group.sizeHint());
        group.setLayoutDirection(Qt::RightToLeft);

        auto* groupLayout = static_cast<QBoxLayout*>(group.layout());
        QVERIFY(groupLayout);
        QCOMPARE(groupLayout->direction(), QBoxLayout::LeftToRight);
        groupLayout->activate();
        QVERIFY(one->geometry().center().x() > two->geometry().center().x());

        QtMaterialFloatingToolbar toolbar;
        auto* first = toolbar.addAction(QIcon(), QStringLiteral("First"));
        auto* second = toolbar.addAction(QIcon(), QStringLiteral("Second"));
        toolbar.resize(toolbar.sizeHint());
        toolbar.setLayoutDirection(Qt::RightToLeft);

        auto* toolbarLayout = static_cast<QBoxLayout*>(toolbar.layout());
        QVERIFY(toolbarLayout);
        QCOMPARE(toolbarLayout->direction(), QBoxLayout::LeftToRight);
        toolbarLayout->activate();
        QVERIFY(first->geometry().center().x() > second->geometry().center().x());

        toolbar.setOrientation(Qt::Vertical);
        QCOMPARE(toolbarLayout->direction(), QBoxLayout::TopToBottom);
    }

    void floatingToolbarCollapsesToLeadingAction()
    {
        QtMaterialFloatingToolbar toolbar;
        toolbar.addAction(QIcon(), QStringLiteral("Edit"));
        toolbar.addAction(QIcon(), QStringLiteral("Share"));

        QCOMPARE(toolbar.count(), 2);
        toolbar.setExpanded(false);
        QVERIFY(!toolbar.itemAt(0)->isHidden());
        QVERIFY(toolbar.itemAt(1)->isHidden());

        toolbar.setOrientation(Qt::Vertical);
        QCOMPARE(toolbar.orientation(), Qt::Vertical);
    }

    void floatingToolbarRemovalIsTeardownSafe()
    {
        QtMaterialFloatingToolbar toolbar;
        auto* action = toolbar.addAction(QIcon(), QStringLiteral("Temporary"));

        toolbar.removeWidget(action);
        delete action;

        QCOMPARE(toolbar.count(), 0);
    }

    void loadingIndicatorIsStateful()
    {
        QtMaterialLoadingIndicator indicator;
        QVERIFY(indicator.isActive());

        QSignalSpy spy(&indicator, &QtMaterialLoadingIndicator::activeChanged);
        indicator.setActive(false);
        QCOMPARE(spy.count(), 1);
        QVERIFY(!indicator.isActive());

        indicator.setIndicatorSize(64);
        QCOMPARE(indicator.sizeHint(), QSize(64, 64));
    }

    void segmentedListAssignsExpressivePositions()
    {
        QtMaterialSegmentedList list;
        auto* first = list.addItem(QStringLiteral("First"));
        auto* middle = list.addItem(QStringLiteral("Middle"));
        auto* last = list.addItem(QStringLiteral("Last"));

        QVERIFY(first->expressive());
        QCOMPARE(first->expressiveSegmentPosition(),
                 QtMaterialListItem::ExpressiveSegmentPosition::First);
        QCOMPARE(middle->expressiveSegmentPosition(),
                 QtMaterialListItem::ExpressiveSegmentPosition::Middle);
        QCOMPARE(last->expressiveSegmentPosition(),
                 QtMaterialListItem::ExpressiveSegmentPosition::Last);
    }

    void segmentedListRefreshesAfterExternalDeletion()
    {
        QtMaterialSegmentedList list;
        auto* first = list.addItem(QStringLiteral("First"));
        auto* second = list.addItem(QStringLiteral("Second"));
        auto* third = list.addItem(QStringLiteral("Third"));

        delete first;

        QCOMPARE(list.count(), 2);
        QCOMPARE(second->expressiveSegmentPosition(),
                 QtMaterialListItem::ExpressiveSegmentPosition::First);
        QCOMPARE(third->expressiveSegmentPosition(),
                 QtMaterialListItem::ExpressiveSegmentPosition::Last);
    }

    void menuAndChipExposeExpressiveMode()
    {
        QtMaterialMenu menu;
        menu.addItem(QStringLiteral("Open"));
        menu.setExpressive(true);
        QVERIFY(menu.expressive());
        QVERIFY(menu.spec().minItemSize.height() >= 56);
        QVERIFY(menu.spec().cornerRadius >= 16.0);

        QtMaterialChip chip(QStringLiteral("Filter"));
        chip.setVariant(ChipVariant::Filter);
        chip.setExpressive(true);
        QVERIFY(chip.expressive());
        chip.setChecked(true);
        QVERIFY(chip.isChecked());
    }
};

QTEST_MAIN(ExpressiveCatalogueTest)
#include "tst_expressive_catalogue.moc"
