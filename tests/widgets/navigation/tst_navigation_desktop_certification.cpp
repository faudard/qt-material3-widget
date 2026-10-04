#include <QAccessible>
#include <QApplication>
#include <QLineEdit>
#include <QListView>
#include <QMenu>
#include <QSignalSpy>
#include <QSplitterHandle>
#include <QStandardItemModel>
#include <QTabBar>
#include <QTest>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <memory>

#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"
#include "qtmaterial/widgets/navigation/qtmaterialnavigationrail.h"
#include "qtmaterial/widgets/navigation/qtmaterialmenu.h"
#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"
#include "qtmaterial/widgets/layouts/qtmaterialsplitview.h"
#include "../../helpers/widgettestactivation.h"

using namespace QtMaterial;

static void directionRows() {
    QTest::addColumn<int>("direction");
    QTest::newRow("ltr") << int(Qt::LeftToRight);
    QTest::newRow("rtl") << int(Qt::RightToLeft);
}

class tst_NavigationDesktopCertification : public QObject {
    Q_OBJECT
private slots:
    void tabsFocusTraversalAndAccessibleSelection_data() { directionRows(); }
    void tabsFocusTraversalAndAccessibleSelection() {
        QFETCH(int, direction);
        QWidget host; host.setLayoutDirection(Qt::LayoutDirection(direction));
        auto* layout = new QVBoxLayout(&host);
        auto* before = new QLineEdit(&host);
        auto* tabs = new QtMaterialTabs(&host);
        auto* after = new QLineEdit(&host);
        tabs->addTab(new QWidget(tabs), QStringLiteral("First"));
        tabs->addTab(new QWidget(tabs), QStringLiteral("Unavailable"));
        tabs->addTab(new QWidget(tabs), QStringLiteral("Last"));
        tabs->setTabEnabled(1, false); tabs->setWrapNavigation(false); tabs->setCurrentIndex(0);
        layout->addWidget(before); layout->addWidget(tabs); layout->addWidget(after);
        auto* bar = tabs->findChild<QTabBar*>(); QVERIFY(bar);
        QWidget::setTabOrder(before, bar); QWidget::setTabOrder(bar, after);
        host.resize(800, 400); host.show(); QVERIFY(QTest::qWaitForWindowExposed(&host));
        activateTestWindow(&host); before->setFocus(); QTRY_VERIFY(before->hasFocus());
        QTest::keyClick(before, Qt::Key_Tab); QTRY_VERIFY(bar->hasFocus());
        const Qt::Key forward = direction == int(Qt::RightToLeft) ? Qt::Key_Left : Qt::Key_Right;
        QTest::keyClick(bar, forward); QCOMPARE(tabs->currentIndex(), 2);
        QTest::keyClick(bar, forward); QCOMPARE(tabs->currentIndex(), 2);
        auto* accessible = QAccessible::queryAccessibleInterface(bar); QVERIFY(accessible);
        QVERIFY(accessible->childCount() >= 3);
        auto* selected = accessible->child(2); QVERIFY(selected);
        QCOMPARE(selected->role(), QAccessible::PageTab);
        QVERIFY(selected->state().selected);
        QVERIFY(selected->state().focused);
        QCOMPARE(accessible->focusChild(), selected);
        QVERIFY(accessible->child(1)->state().disabled);
        QVERIFY(accessible->child(1)->actionInterface()->actionNames().isEmpty());
        QTest::keyClick(bar, Qt::Key_Tab); QTRY_VERIFY(after->hasFocus());
        QTest::keyClick(after, Qt::Key_Backtab); QTRY_VERIFY(bar->hasFocus());
        tabs->setTabEnabled(0, false); tabs->setTabEnabled(2, false);
        const int unchanged = tabs->currentIndex();
        QTest::keyClick(bar, forward); QCOMPARE(tabs->currentIndex(), unchanged);
    }

    void railExposesRealAccessibleDestinations_data() { directionRows(); }
    void railExposesRealAccessibleDestinations() {
        QFETCH(int, direction);
        QtMaterialNavigationRail rail; rail.setLayoutDirection(Qt::LayoutDirection(direction));
        rail.addDestination(QStringLiteral("Home")); rail.addDestination(QStringLiteral("Disabled"));
        rail.addDestination(QStringLiteral("Settings")); rail.setDestinationEnabled(1, false);
        rail.setCurrentIndex(0); rail.resize(rail.sizeHint()); rail.show();
        QVERIFY(QTest::qWaitForWindowExposed(&rail)); activateTestWindow(&rail); rail.setFocus();
        QTRY_VERIFY(rail.hasFocus());
        auto* accessible = QAccessible::queryAccessibleInterface(&rail); QVERIFY(accessible);
        QCOMPARE(accessible->role(), QAccessible::List); QCOMPARE(accessible->childCount(), 3);
        auto* settings = accessible->child(2); QVERIFY(settings);
        QCOMPARE(settings->text(QAccessible::Name), QStringLiteral("Settings"));
        QCOMPARE(settings->role(), QAccessible::ListItem);
        QCOMPARE(accessible->indexOfChild(settings), 2);
        QVERIFY(!settings->rect().isEmpty());
        QCOMPARE(accessible->childAt(settings->rect().center().x(), settings->rect().center().y()), settings);
        QVERIFY(accessible->child(1)->state().disabled);
        QVERIFY(accessible->child(1)->actionInterface()->actionNames().isEmpty());
        QSignalSpy activated(&rail, &QtMaterialNavigationRail::destinationActivated);
        settings->actionInterface()->doAction(QAccessibleActionInterface::pressAction());
        QCOMPARE(activated.count(), 1); QCOMPARE(activated.first().first().toInt(), 2);
        QVERIFY(settings->state().selected); QVERIFY(settings->state().focused);
        QCOMPARE(accessible->focusChild(), settings);
        rail.setDestinationEnabled(2, false);
        QVERIFY(settings->state().disabled);
        rail.setDestinationEnabled(0, false);
        for (Qt::Key key : {Qt::Key_Home, Qt::Key_End, Qt::Key_Down, Qt::Key_Up, Qt::Key_Return, Qt::Key_Space}) {
            QTest::keyClick(&rail, key);
        }
        QCOMPARE(rail.currentIndex(), -1); QCOMPARE(activated.count(), 1);
        rail.clearDestinations(); QCOMPARE(accessible->childCount(), 0); QVERIFY(!accessible->child(0));
    }

    void menuAccessibleActionsExposeCheckableAndDisabledStates_data() { directionRows(); }
    void menuAccessibleActionsExposeCheckableAndDisabledStates() {
        QFETCH(int, direction);
        QtMaterialMenu menu; menu.setLayoutDirection(Qt::LayoutDirection(direction));
        const int open = menu.addItem(QStringLiteral("Open")); menu.setItemShortcutText(open, QStringLiteral("Ctrl+O"));
        menu.addSeparator(); const int checked = menu.addItem(QStringLiteral("Show grid"));
        menu.setItemCheckable(checked, true);
        const int disabled = menu.addItem(QStringLiteral("Delete")); menu.setItemEnabled(disabled, false);
        menu.resize(menu.sizeHint()); menu.show(); QVERIFY(QTest::qWaitForWindowExposed(&menu));
        activateTestWindow(&menu); menu.setFocus(); QTRY_VERIFY(menu.hasFocus());
        auto* accessible = QAccessible::queryAccessibleInterface(&menu); QVERIFY(accessible);
        QCOMPARE(accessible->role(), QAccessible::PopupMenu); QCOMPARE(accessible->childCount(), 4);
        QCOMPARE(accessible->child(open)->text(QAccessible::Accelerator), QStringLiteral("Ctrl+O"));
        QCOMPARE(accessible->child(1)->role(), QAccessible::Separator);
        QVERIFY(accessible->child(1)->actionInterface()->actionNames().isEmpty());
        auto* grid = accessible->child(checked); QVERIFY(grid);
        QCOMPARE(grid->role(), QAccessible::MenuItem); QVERIFY(grid->state().checkable);
        QSignalSpy activation(&menu, &QtMaterialMenu::activated);
        grid->actionInterface()->doAction(QAccessibleActionInterface::pressAction());
        QCOMPARE(activation.count(), 1); QVERIFY(grid->state().checked); QVERIFY(grid->state().focused);
        accessible->child(disabled)->actionInterface()->doAction(QAccessibleActionInterface::pressAction());
        QCOMPARE(activation.count(), 1);
        QTest::keyClick(&menu, Qt::Key_Home); QCOMPARE(menu.currentIndex(), open);
        QTest::keyClick(&menu, Qt::Key_End); QCOMPARE(menu.currentIndex(), checked);
        QTest::keyClick(&menu, Qt::Key_Space); QVERIFY(!grid->state().checked);
        menu.clear(); QCOMPARE(accessible->childCount(), 0); QVERIFY(!accessible->focusChild());
        QTest::keyClick(&menu, Qt::Key_Return); QCOMPARE(activation.count(), 2);
        QSignalSpy dismissed(&menu, &QtMaterialMenu::dismissed);
        QTest::keyClick(&menu, Qt::Key_Escape); QCOMPARE(dismissed.count(), 1);
    }

    void paintedItemAccessibleCacheTracksMutationAndDestruction() {
        auto tabs = std::make_unique<QtMaterialTabs>();
        tabs->addTab(new QWidget(tabs.get()), QStringLiteral("&First"));
        tabs->addTab(new QWidget(tabs.get()), QStringLiteral("Unavailable"));
        tabs->addTab(new QWidget(tabs.get()), QStringLiteral("Last && final"));
        auto* bar = tabs->findChild<QTabBar*>(); QVERIFY(bar);
        auto* tabRoot = QAccessible::queryAccessibleInterface(bar); QVERIFY(tabRoot);
        QCOMPARE(tabRoot->role(), QAccessible::PageTabList);
        QCOMPARE(tabRoot->child(0)->text(QAccessible::Name), QStringLiteral("First"));
        auto* lastTab = tabRoot->child(2); QVERIFY(lastTab);
        const auto removedTabId = QAccessible::uniqueId(lastTab);
        tabs->removeTab(1);
        QVERIFY(tabRoot->childCount() >= tabs->count());
        QVERIFY(!QAccessible::accessibleInterface(removedTabId));
        QCOMPARE(tabRoot->child(1)->text(QAccessible::Name), QStringLiteral("Last & final"));
        for (auto* button : bar->findChildren<QToolButton*>(QString(), Qt::FindDirectChildrenOnly)) {
            auto* control = QAccessible::queryAccessibleInterface(button); QVERIFY(control);
            QVERIFY(tabRoot->indexOfChild(control) >= tabs->count());
        }
        tabRoot->child(1)->actionInterface()->doAction(QAccessibleActionInterface::pressAction());
        QCOMPARE(tabs->currentIndex(), 1);
        QVERIFY(tabRoot->child(1)->state().selected);
        const auto survivingTabId = QAccessible::uniqueId(tabRoot->child(0));
        tabs.reset();
        QVERIFY(!QAccessible::accessibleInterface(survivingTabId));

        auto rail = std::make_unique<QtMaterialNavigationRail>();
        rail->addDestination(QStringLiteral("Home"));
        rail->addDestination(QStringLiteral("Projects"));
        rail->addDestination(QStringLiteral("Settings"));
        auto* root = QAccessible::queryAccessibleInterface(rail.get()); QVERIFY(root);
        auto* last = root->child(2); QVERIFY(last);
        const auto removedId = QAccessible::uniqueId(last);
        rail->removeDestination(1);
        QCOMPARE(root->childCount(), 2);
        QVERIFY(!QAccessible::accessibleInterface(removedId));
        QCOMPARE(root->child(1)->text(QAccessible::Name), QStringLiteral("Settings"));
        const auto survivingId = QAccessible::uniqueId(root->child(0));
        rail.reset();
        QVERIFY(!QAccessible::accessibleInterface(survivingId));

        auto menu = std::make_unique<QtMaterialMenu>();
        menu->addItem(QStringLiteral("Open"));
        root = QAccessible::queryAccessibleInterface(menu.get()); QVERIFY(root);
        const auto actionId = QAccessible::uniqueId(root->child(0));
        menu.reset();
        QVERIFY(!QAccessible::accessibleInterface(actionId));
    }

    void breadcrumbOverflowAndEditorAreKeyboardReachable_data() { directionRows(); }
    void breadcrumbOverflowAndEditorAreKeyboardReachable() {
        QFETCH(int, direction);
        QtMaterialBreadcrumb breadcrumb; breadcrumb.setLayoutDirection(Qt::LayoutDirection(direction));
        breadcrumb.setItems({QStringLiteral("Root"), QStringLiteral("Projects"), QStringLiteral("Source"), QStringLiteral("File")});
        breadcrumb.setCurrentIndex(3); breadcrumb.setMaximumVisibleItems(2); breadcrumb.setLocationEditable(true);
        breadcrumb.resize(360, 64); breadcrumb.show(); QVERIFY(QTest::qWaitForWindowExposed(&breadcrumb));
        activateTestWindow(&breadcrumb);
        QToolButton* overflow = nullptr;
        for (auto* button : breadcrumb.findChildren<QToolButton*>()) {
            if (button->isVisible() && button->menu()) { overflow = button; break; }
        }
        QVERIFY(overflow); overflow->setFocus(); QTRY_VERIFY(overflow->hasFocus());
        auto* accessible = QAccessible::queryAccessibleInterface(overflow); QVERIFY(accessible);
        QVERIFY(!accessible->text(QAccessible::Name).isEmpty());
        auto* menu = overflow->menu();
        int keyboardSelection = -1;
        QSignalSpy activated(&breadcrumb, &QtMaterialBreadcrumb::activated);
        QTimer::singleShot(0, menu, [menu, &keyboardSelection]() {
            // Home only scrolls a native QMenu; Down selects an action.
            QTest::keyClick(menu, Qt::Key_Down);
            if (auto* action = menu->activeAction()) { keyboardSelection = action->data().toInt(); }
            QTest::keyClick(menu, Qt::Key_Return);
            menu->close(); // Also releases a native popup loop if no action was selected.
        });
        QTest::keyClick(overflow, Qt::Key_Space);
        QTRY_COMPARE(activated.count(), 1);
        QVERIFY(keyboardSelection == 1 || keyboardSelection == 2);
        QCOMPARE(activated.first().first().toInt(), keyboardSelection);
        breadcrumb.setCurrentIndex(3);
        activateTestWindow(&breadcrumb);
        const auto visibleRoot = [&breadcrumb]() -> QToolButton* {
            for (auto* button : breadcrumb.findChildren<QToolButton*>()) {
                if (button->isVisible() && button->text() == QStringLiteral("Root")) { return button; }
            }
            return nullptr;
        };
        // Rebuilt layout children become visible when Qt processes layout events.
        QTRY_VERIFY(visibleRoot());
        auto* root = visibleRoot(); root->setFocus(); QTRY_VERIFY(root->hasFocus());
        QTest::keyClick(root, Qt::Key_L, Qt::ControlModifier); QTRY_VERIFY(breadcrumb.isEditingLocation());
        auto* edit = breadcrumb.findChild<QLineEdit*>(); QVERIFY(edit);
        QTRY_VERIFY(edit->hasFocus());
        auto* editAccessible = QAccessible::queryAccessibleInterface(edit); QVERIFY(editAccessible);
        QCOMPARE(editAccessible->role(), QAccessible::EditableText);
        QVERIFY(!editAccessible->text(QAccessible::Name).isEmpty());
        QTest::keyClick(edit, Qt::Key_Escape); QVERIFY(!breadcrumb.isEditingLocation());
        QVERIFY(QApplication::focusWidget());
        QVERIFY(breadcrumb.isAncestorOf(QApplication::focusWidget()) || breadcrumb.hasFocus());
        breadcrumb.setEditingLocation(true);
        QSignalSpy submitted(&breadcrumb, &QtMaterialBreadcrumb::locationSubmitted);
        QTest::keyClicks(edit, QStringLiteral("/projects/source")); QTest::keyClick(edit, Qt::Key_Return);
        QCOMPARE(submitted.count(), 1); QCOMPARE(submitted.first().first().toString(), QStringLiteral("/projects/source"));
        QVERIFY(breadcrumb.accessibleDescription().contains(QStringLiteral("Root")));
    }

    void paletteAccessibleRowsContainActionMetadata() {
        QtMaterialCommandPalette palette;
        QStandardItemModel model;
        auto* item = new QStandardItem(QStringLiteral("Open project"));
        item->setData(QStringLiteral("Browse a workspace"), QtMaterialCommandPalette::SecondaryTextRole);
        item->setData(QStringLiteral("Ctrl+O"), QtMaterialCommandPalette::ShortcutRole);
        model.appendRow(item); palette.setSourceModel(&model); palette.openPalette();
        QVERIFY(QTest::qWaitForWindowExposed(&palette));
        activateTestWindow(&palette);
        auto* results = palette.findChild<QListView*>(); QVERIFY(results);
        auto* accessible = QAccessible::queryAccessibleInterface(results); QVERIFY(accessible);
        QCOMPARE(accessible->role(), QAccessible::List);
        auto* row = accessible->child(0); QVERIFY(row);
        const QString name = row->text(QAccessible::Name);
        QVERIFY(name.contains(QStringLiteral("Open project")));
        QVERIFY(name.contains(QStringLiteral("Browse a workspace")));
        QVERIFY(name.contains(QStringLiteral("Ctrl+O")));
        results->setFocus(); QTRY_VERIFY(results->hasFocus());
        QVERIFY(row->state().selected); QVERIFY(row->state().focused);
    }

    void splitKeyboardConstraintsAndCollapsedState_data() {
        QTest::addColumn<int>("orientation"); QTest::addColumn<int>("direction");
        QTest::addColumn<int>("handleWidth");
        QTest::newRow("horizontal-ltr") << int(Qt::Horizontal) << int(Qt::LeftToRight) << 8;
        QTest::newRow("horizontal-rtl") << int(Qt::Horizontal) << int(Qt::RightToLeft) << 8;
        QTest::newRow("vertical-ltr") << int(Qt::Vertical) << int(Qt::LeftToRight) << 8;
        QTest::newRow("vertical-rtl") << int(Qt::Vertical) << int(Qt::RightToLeft) << 8;
        QTest::newRow("thin-horizontal-ltr") << int(Qt::Horizontal) << int(Qt::LeftToRight) << 1;
        QTest::newRow("thin-horizontal-rtl") << int(Qt::Horizontal) << int(Qt::RightToLeft) << 1;
        QTest::newRow("thin-vertical-ltr") << int(Qt::Vertical) << int(Qt::LeftToRight) << 1;
        QTest::newRow("thin-vertical-rtl") << int(Qt::Vertical) << int(Qt::RightToLeft) << 1;
    }
    void splitKeyboardConstraintsAndCollapsedState() {
        QFETCH(int, orientation); QFETCH(int, direction); QFETCH(int, handleWidth);
        QtMaterialSplitView split{Qt::Orientation(orientation)};
        split.setHandleWidth(handleWidth);
        split.setLayoutDirection(Qt::LayoutDirection(direction)); split.resize(800, 600);
        split.addWidget(new QWidget); split.addWidget(new QWidget); split.addWidget(new QWidget);
        split.setPaneMinimumExtent(0, 80); split.setPaneMaximumExtent(0, 420);
        split.setSizes({200, 200, 200}); split.show(); QVERIFY(QTest::qWaitForWindowExposed(&split));
        auto* handle = split.handle(1); activateTestWindow(&split); handle->setFocus(); QTRY_VERIFY(handle->hasFocus());
        auto* accessible = QAccessible::queryAccessibleInterface(handle); QVERIFY(accessible);
        QVERIFY(accessible->state().focusable); QVERIFY(accessible->state().focused);
        QVERIFY(!accessible->text(QAccessible::Name).isEmpty());
        QVERIFY(!accessible->text(QAccessible::Description).isEmpty());
        const auto coordinate = [handle, orientation]() { return orientation == int(Qt::Horizontal) ? handle->x() : handle->y(); };
        const int start = coordinate();
        const Qt::Key positive = orientation == int(Qt::Horizontal) ? Qt::Key_Right : Qt::Key_Down;
        const Qt::Key negative = orientation == int(Qt::Horizontal) ? Qt::Key_Left : Qt::Key_Up;
        QTest::keyClick(handle, positive); QCOMPARE(coordinate(), start + split.keyboardResizeStep());
        QTest::keyClick(handle, negative); QCOMPARE(coordinate(), start);
        QTest::keyClick(handle, positive, Qt::ShiftModifier); QCOMPARE(coordinate(), start + 4 * split.keyboardResizeStep());
        QTest::keyClick(handle, Qt::Key_Home); QVERIFY(split.sizes().first() >= 80);
        QTest::keyClick(handle, Qt::Key_End); QVERIFY(split.sizes().first() <= 420);
        QTest::keyClick(handle, Qt::Key_Return); QVERIFY(split.paneCollapsed(0));
        const QByteArray saved = split.savePaneState();
        QTest::keyClick(handle, Qt::Key_Return); QVERIFY(!split.paneCollapsed(0));
        QVERIFY(split.restorePaneState(saved)); QVERIFY(split.paneCollapsed(0));
        split.setPaneCollapsed(0, false); QVERIFY(split.sizes().first() >= 80);
        const QList<int> beforeBadState = split.sizes();
        QVERIFY(!split.restorePaneState(QByteArrayLiteral("truncated")));
        QCOMPARE(split.sizes(), beforeBadState);
    }
};

QTEST_MAIN(tst_NavigationDesktopCertification)
#include "tst_navigation_desktop_certification.moc"
