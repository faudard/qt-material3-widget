#include <QAccessible>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLineEdit>
#include <QListView>
#include <QMenu>
#include <QMimeData>
#include <QShortcut>
#include <QSignalSpy>
#include <QSplitterHandle>
#include <QStandardItemModel>
#include <QTest>
#include <QThread>
#include <QToolButton>

#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"
#include "qtmaterial/widgets/layouts/qtmaterialsplitview.h"

using namespace QtMaterial;

class ManualProvider final : public QtMaterialCommandProvider
{
public:
    using QtMaterialCommandProvider::QtMaterialCommandProvider;
    QList<quint64> requests;
    QList<quint64> cancellations;
    QStringList queries;
    QStringList activations;

    void requestCommands(const QString& query, quint64 id) override
    {
        requests.push_back(id);
        queries.push_back(query);
    }
    void cancelRequest(quint64 id) override { cancellations.push_back(id); }
    void activateCommand(const QString& id) override { activations.push_back(id); }
    void publish(quint64 id, const QList<QtMaterialCommand>& commands) { Q_EMIT commandsReady(id, commands); }
};

class ThreadProvider final : public QtMaterialCommandProvider
{
    Q_OBJECT
public:
    void requestCommands(const QString&, quint64 id) override
    {
        QtMaterialCommand command;
        command.id = QStringLiteral("worker.command");
        command.text = QStringLiteral("Worker command");
        Q_EMIT commandsReady(id, {command});
    }
    void activateCommand(const QString& id) override { Q_EMIT activatedInThread(id, QThread::currentThread()); }
Q_SIGNALS:
    void activatedInThread(const QString& id, QThread* thread);
};

class TestWorker final : public QThread
{
public:
    ~TestWorker() override { quit(); wait(); }
};

class tst_DesktopNavigationV2 : public QObject
{
    Q_OBJECT
private slots:
    void modelMetadataAndIncrementalProviderResultsRetainSelection()
    {
        QStandardItemModel model(3, 2);
        for (int row = 0; row < 3; ++row) {
            model.setData(model.index(row, 0), QStringLiteral("Command %1").arg(row));
            model.setData(model.index(row, 1), QStringLiteral("metadata-%1").arg(row));
        }
        QtMaterialCommandPalette palette;
        palette.setSourceModel(&model);
        palette.setQuery(QStringLiteral("metadata-1"));
        auto* results = palette.findChild<QListView*>();
        QCOMPARE(results->model()->rowCount(), 1);
        QCOMPARE(results->currentIndex().data().toString(), QStringLiteral("Command 1"));
        model.setData(model.index(1, 1), QStringLiteral("changed"));
        QCOMPARE(results->model()->rowCount(), 0);
        palette.setQuery(QString());
        results->setCurrentIndex(results->model()->index(1, 0));
        ManualProvider provider;
        palette.addProvider(&provider);
        QtMaterialCommand command;
        command.id = QStringLiteral("provider.command"); command.text = QStringLiteral("Provider result");
        provider.publish(provider.requests.last(), {command});
        QCOMPARE(results->currentIndex().data().toString(), QStringLiteral("Command 1"));
    }

    void fuzzySearchRanksTitlesAndMatchesKeywords()
    {
        QStandardItemModel model;
        for (const QString& title : {QStringLiteral("Optimize Framerate"), QStringLiteral("Open File"), QStringLiteral("Build project")}) {
            model.appendRow(new QStandardItem(title));
        }
        model.setData(model.index(2, 0), QStringList{QStringLiteral("compile")}, QtMaterialCommandPalette::KeywordsRole);
        QtMaterialCommandPalette palette;
        palette.setSourceModel(&model);
        auto* results = palette.findChild<QListView*>();
        QVERIFY(palette.fuzzyMatchingEnabled());
        palette.setQuery(QStringLiteral("opfi"));
        QCOMPARE(results->model()->rowCount(), 1);
        QCOMPARE(results->model()->index(0, 0).data().toString(), QStringLiteral("Open File"));
        palette.setFuzzyMatchingEnabled(false);
        QCOMPARE(results->model()->rowCount(), 0);
        palette.setFuzzyMatchingEnabled(true);
        palette.setQuery(QStringLiteral("compile"));
        QCOMPARE(results->model()->rowCount(), 1);
        QCOMPARE(results->currentIndex().data().toString(), QStringLiteral("Build project"));
        palette.setQuery(QStringLiteral("project build"));
        QCOMPARE(results->model()->rowCount(), 1);
        palette.setQuery(QStringLiteral("["));
        QCOMPARE(results->model()->rowCount(), 0);
    }

    void providersRejectStaleRepliesAndHandleRemoval()
    {
        ManualProvider provider;
        QtMaterialCommandPalette palette;
        palette.addProvider(&provider);
        QVERIFY(palette.isLoading());
        const quint64 oldRequest = provider.requests.last();
        palette.setQuery(QStringLiteral("new"));
        palette.refreshProviders();
        const quint64 currentRequest = provider.requests.last();
        QVERIFY(currentRequest != oldRequest);
        QVERIFY(provider.cancellations.contains(oldRequest));
        QtMaterialCommand oldCommand;
        oldCommand.id = QStringLiteral("old"); oldCommand.text = QStringLiteral("new stale result");
        provider.publish(oldRequest, {oldCommand});
        auto* results = palette.findChild<QListView*>();
        QCOMPARE(results->model()->rowCount(), 0);
        QVERIFY(palette.isLoading());
        auto* empty = palette.findChild<QLabel*>(QStringLiteral("QtMaterialCommandPaletteEmptyState"));
        QVERIFY(empty->isHidden());
        QtMaterialCommand command;
        command.id = QStringLiteral("new"); command.text = QStringLiteral("new command");
        provider.publish(currentRequest, {command});
        QCOMPARE(results->model()->rowCount(), 1);
        QVERIFY(!palette.isLoading());
        palette.removeProvider(&provider);
        QCOMPARE(results->model()->rowCount(), 0);
        QVERIFY(!palette.isLoading());
        provider.publish(currentRequest, {command});
        QCOMPARE(results->model()->rowCount(), 0);
    }

    void providersMergeSectionsFavoritesAndRecentCommands()
    {
        ManualProvider first, second;
        QtMaterialCommandPalette palette;
        palette.addProvider(&first);
        palette.addProvider(&second);
        QtMaterialCommand file;
        file.id = QStringLiteral("file.open"); file.text = QStringLiteral("Open file");
        file.section = QStringLiteral("Files"); file.secondaryText = QStringLiteral("Open a workspace file");
        file.shortcut = QKeySequence(QStringLiteral("Ctrl+O"));
        QtMaterialCommand settings;
        settings.id = QStringLiteral("settings.open"); settings.text = QStringLiteral("Open settings");
        settings.section = QStringLiteral("Settings");
        first.publish(first.requests.last(), {file, settings});
        second.publish(second.requests.last(), {file}); // Stable IDs deduplicate across providers.
        QVERIFY(!palette.isLoading());
        auto* results = palette.findChild<QListView*>();
        QCOMPARE(results->model()->rowCount(), 2);
        palette.setFavoriteCommandIds({settings.id, settings.id, QString()});
        QCOMPARE(palette.favoriteCommandIds(), QStringList{settings.id});
        QCOMPARE(results->model()->index(0, 0).data(QtMaterialCommandPalette::SectionHeadingRole).toString(), QStringLiteral("Favorites"));
        palette.setRecentCommandIds({file.id, settings.id, file.id});
        QCOMPARE(palette.recentCommandIds(), (QStringList{file.id, settings.id}));
        QCOMPARE(results->model()->index(1, 0).data(QtMaterialCommandPalette::SectionHeadingRole).toString(), QStringLiteral("Recent commands"));
        QCOMPARE(results->model()->index(1, 0).data(QtMaterialCommandPalette::SecondaryTextRole).toString(), file.secondaryText);
        QCOMPARE(results->model()->index(1, 0).data(QtMaterialCommandPalette::ShortcutRole).toString(), file.shortcut.toString(QKeySequence::NativeText));
        palette.setLayoutDirection(Qt::RightToLeft);
        QPixmap image(palette.size() * 2);
        image.setDevicePixelRatio(2);
        palette.render(&image);
        QVERIFY(!image.isNull());
    }

    void providerFailureAndDestructionClearPendingState()
    {
        auto* provider = new ManualProvider;
        QtMaterialCommandPalette palette;
        palette.addProvider(provider);
        QSignalSpy failed(&palette, &QtMaterialCommandPalette::providerFailed);
        Q_EMIT provider->requestFailed(provider->requests.last(), QStringLiteral("Unavailable"));
        QCOMPARE(failed.count(), 1);
        QVERIFY(!palette.isLoading());
        palette.refreshProviders();
        QVERIFY(palette.isLoading());
        delete provider;
        QVERIFY(palette.providers().isEmpty());
        QVERIFY(!palette.isLoading());
        auto* model = new QStandardItemModel(1, 1);
        palette.setSourceModel(model);
        delete model;
        QVERIFY(!palette.sourceModel());
        QCOMPARE(palette.findChild<QListView*>()->model()->rowCount(), 0);
    }

    void keyboardSkipsDisabledCommandsAndUpdatesHistory()
    {
        QStandardItemModel model;
        auto* disabled = new QStandardItem(QStringLiteral("Disabled"));
        disabled->setEnabled(false);
        model.appendRow(disabled);
        model.appendRow(new QStandardItem(QStringLiteral("Open file")));
        model.setData(model.index(1, 0), QStringLiteral("file.open"), QtMaterialCommandPalette::IdRole);
        QtMaterialCommandPalette palette;
        palette.setSourceModel(&model);
        palette.openPalette();
        QVERIFY(QTest::qWaitForWindowExposed(&palette));
        auto* search = palette.findChild<QLineEdit*>();
        auto* results = palette.findChild<QListView*>();
        QTRY_VERIFY(search->hasFocus());
        QCOMPARE(results->currentIndex().row(), 1);
        QTest::keyClick(search, Qt::Key_Home);
        QCOMPARE(results->currentIndex().row(), 1);
        QTest::keyClick(search, Qt::Key_D, Qt::ControlModifier);
        QCOMPARE(palette.favoriteCommandIds(), QStringList{QStringLiteral("file.open")});
        QSignalSpy activated(&palette, &QtMaterialCommandPalette::commandActivated);
        results->setFocus();
        QTest::keyClick(results, Qt::Key_Return);
        QCOMPARE(activated.count(), 1);
        QCOMPARE(activated.first().first().value<QModelIndex>(), model.index(1, 0));
        QCOMPARE(palette.recentCommandIds(), QStringList{QStringLiteral("file.open")});
        QVERIFY(!palette.isVisible());
    }

    void activationShortcutsBelongToHostAndAreConfigurable()
    {
        QWidget host;
        auto* palette = new QtMaterialCommandPalette(&host);
        QCOMPARE(palette->activationShortcuts().size(), 2);
        QCOMPARE(host.findChildren<QShortcut*>(QString(), Qt::FindDirectChildrenOnly).size(), 2);
        host.show();
        QVERIFY(QTest::qWaitForWindowExposed(&host));
        auto* shortcut = host.findChildren<QShortcut*>(QString(), Qt::FindDirectChildrenOnly).first();
        QVERIFY(QMetaObject::invokeMethod(shortcut, "activated"));
        QTRY_VERIFY(palette->isVisible());
        QTRY_VERIFY(palette->findChild<QLineEdit*>()->hasFocus());
        palette->reject();
        palette->setActivationShortcuts({QKeySequence(QStringLiteral("Ctrl+J"))});
        QCOMPARE(host.findChildren<QShortcut*>(QString(), Qt::FindDirectChildrenOnly).size(), 1);
        delete palette;
        QVERIFY(host.findChildren<QShortcut*>(QString(), Qt::FindDirectChildrenOnly).isEmpty());
    }

    void providersCanRunInWorkerThread()
    {
        TestWorker worker;
        auto* provider = new ThreadProvider;
        provider->moveToThread(&worker);
        connect(&worker, &QThread::finished, provider, &QObject::deleteLater);
        worker.start();
        QtMaterialCommandPalette palette;
        QSignalSpy activated(provider, &ThreadProvider::activatedInThread);
        palette.addProvider(provider);
        auto* results = palette.findChild<QListView*>();
        QTRY_COMPARE(results->model()->rowCount(), 1);
        QTRY_VERIFY(!palette.isLoading());
        // Activation is also dispatched to the provider's owning thread.
        QVERIFY(QMetaObject::invokeMethod(results, "activated", Qt::DirectConnection,
            Q_ARG(QModelIndex, results->model()->index(0, 0))));
        QTRY_COMPARE(activated.count(), 1);
        QCOMPARE(activated.first().at(1).value<QThread*>(), &worker);
        palette.removeProvider(provider);
        worker.quit();
        QVERIFY(worker.wait(3000));
    }

    void breadcrumbEditsLocationWithEnterAndEscape()
    {
        QtMaterialBreadcrumb breadcrumb;
        breadcrumb.setItems({QStringLiteral("Workspace"), QStringLiteral("Files")});
        breadcrumb.setLocationEditable(true);
        breadcrumb.setLocation(QStringLiteral("/workspace/files"));
        breadcrumb.show();
        QVERIFY(QTest::qWaitForWindowExposed(&breadcrumb));
        breadcrumb.setFocus();
        breadcrumb.setEditingLocation(true);
        auto* editor = breadcrumb.findChild<QLineEdit*>();
        QVERIFY(editor);
        QTRY_VERIFY(editor->hasFocus());
        QCOMPARE(editor->selectedText(), breadcrumb.location());
        QSignalSpy submitted(&breadcrumb, &QtMaterialBreadcrumb::locationSubmitted);
        editor->setText(QStringLiteral("/workspace/new"));
        QTest::keyClick(editor, Qt::Key_Escape);
        QVERIFY(!breadcrumb.isEditingLocation());
        QCOMPARE(submitted.count(), 0);
        QCOMPARE(breadcrumb.location(), QStringLiteral("/workspace/files"));
        breadcrumb.setEditingLocation(true);
        editor->setText(QStringLiteral("/workspace/new"));
        QTest::keyClick(editor, Qt::Key_Return);
        QCOMPARE(submitted.count(), 1);
        QCOMPARE(submitted.first().first().toString(), QStringLiteral("/workspace/new"));
        QVERIFY(!breadcrumb.isEditingLocation());
        breadcrumb.setEditingLocation(true);
        breadcrumb.setLocationEditable(false);
        QVERIFY(!breadcrumb.isEditingLocation());
    }

    void breadcrumbOverflowRetainsIconsAndHonorsVisibleLimit()
    {
        QtMaterialBreadcrumb breadcrumb;
        breadcrumb.setItems({QStringLiteral("Root"), QStringLiteral("Folder"), QStringLiteral("Project"), QStringLiteral("File")});
        breadcrumb.setCurrentIndex(2);
        breadcrumb.setMaximumVisibleItems(2);
        QPixmap pixels(16, 16); pixels.fill(Qt::blue);
        const QIcon icon(pixels);
        breadcrumb.setItemIcon(1, icon);
        QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
        int segments = 0;
        bool hiddenIcon = false;
        for (auto* button : breadcrumb.findChildren<QToolButton*>()) {
            if (button->menu()) {
                for (auto* action : button->menu()->actions()) {
                    if (action->data().toInt() == 1) { hiddenIcon = !action->icon().isNull(); }
                }
            } else { ++segments; }
        }
        QCOMPARE(segments, 2);
        QVERIFY(hiddenIcon);
        QVERIFY(!breadcrumb.itemIcon(1).isNull());
    }

    void breadcrumbUrlDropsAreOptInAndCopyOnly()
    {
        QtMaterialBreadcrumb breadcrumb;
        breadcrumb.setItems({QStringLiteral("Workspace"), QStringLiteral("Files")});
        breadcrumb.setItemUrl(0, QUrl::fromLocalFile(QStringLiteral("/workspace")));
        QCOMPARE(breadcrumb.itemUrl(0), QUrl::fromLocalFile(QStringLiteral("/workspace")));
        QVERIFY(!breadcrumb.dragDropEnabled());
        breadcrumb.setDragDropEnabled(true);
        breadcrumb.resize(500, 48);
        breadcrumb.show();
        QVERIFY(QTest::qWaitForWindowExposed(&breadcrumb));
        QMimeData mime;
        const QList<QUrl> urls{QUrl::fromLocalFile(QStringLiteral("/workspace/file.xml"))};
        mime.setUrls(urls);
        QSignalSpy dropped(&breadcrumb, &QtMaterialBreadcrumb::urlsDropped);
        const QPoint position(breadcrumb.width() - 4, breadcrumb.height() / 2);
        QDragEnterEvent enter(position, Qt::CopyAction | Qt::MoveAction, &mime, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(&breadcrumb, &enter);
        QVERIFY(enter.isAccepted());
        QDropEvent drop(QPointF(position), Qt::CopyAction | Qt::MoveAction, &mime, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(&breadcrumb, &drop);
        QCOMPARE(dropped.count(), 1);
        QCOMPARE(drop.dropAction(), Qt::CopyAction);
        QCOMPARE(dropped.first().first().toInt(), breadcrumb.currentIndex());
        breadcrumb.setDragDropEnabled(false);
        QVERIFY(!breadcrumb.acceptDrops());
    }

    void splitStateRestoresCollapsedPaneAndOriginalPolicy()
    {
        QtMaterialSplitView split;
        split.resize(640, 240);
        split.addWidget(new QWidget); split.addWidget(new QWidget);
        split.show();
        QVERIFY(QTest::qWaitForWindowExposed(&split));
        split.setSizes({220, 412});
        const int expandedSize = split.sizes().first();
        split.setPaneCollapsed(0, true);
        const QByteArray state = split.savePaneState();
        split.setPaneCollapsed(0, false);
        QVERIFY(split.restorePaneState(state));
        QVERIFY(split.paneCollapsed(0));
        split.setPaneCollapsed(0, false);
        QVERIFY(!split.paneCollapsed(0));
        QVERIFY(qAbs(split.sizes().first() - expandedSize) <= 2);
        QVERIFY(!split.paneCollapsible(0));
        const QList<int> before = split.sizes();
        QVERIFY(!split.restorePaneState(state.left(12)));
        QCOMPARE(split.sizes(), before);
        QtMaterialSplitView other;
        other.addWidget(new QWidget);
        QVERIFY(!other.restorePaneState(state));
    }

    void splitAnimatedCollapseRestoresConstraintsAndCanReverse()
    {
        QtMaterialSplitView split;
        split.resize(640, 240);
        split.addWidget(new QWidget); split.addWidget(new QWidget);
        split.setPaneMinimumExtent(0, 120);
        split.setAnimatedCollapseEnabled(true);
        split.setCollapseAnimationDuration(100);
        split.show();
        QVERIFY(QTest::qWaitForWindowExposed(&split));
        split.setSizes({240, 392});
        QSignalSpy collapsed(&split, &QtMaterialSplitView::paneCollapsedChanged);
        split.setPaneCollapsed(0, true);
        QTRY_COMPARE(split.sizes().first(), 0);
        QTRY_COMPARE(collapsed.count(), 1);
        QCOMPARE(split.paneMinimumExtent(0), 120);
        split.setPaneCollapsed(0, false);
        QTRY_VERIFY(split.sizes().first() >= 120);
        QTRY_COMPARE(collapsed.count(), 2);
        QVERIFY(!split.paneCollapsible(0));
        split.setPaneCollapsed(0, true);
        split.setPaneCollapsed(0, false);
        QTRY_VERIFY(!split.paneCollapsed(0));
        QTRY_VERIFY(split.sizes().first() >= 120);
        QCOMPARE(split.paneMinimumExtent(0), 120);
    }

    void splitKeyboardUsesNativeConstraintsAndDefaultReset()
    {
        QtMaterialSplitView split;
        split.resize(900, 240);
        for (int i = 0; i < 3; ++i) { split.addWidget(new QWidget); }
        split.setPaneMinimumExtent(0, 160);
        split.setPaneMaximumExtent(0, 340);
        split.setKeyboardResizeStep(12);
        split.setDefaultPaneSizes({220, 320, 344});
        split.show();
        QVERIFY(QTest::qWaitForWindowExposed(&split));
        split.resetPaneSizes();
        const int lastSize = split.sizes().last();
        QTest::keyClick(split.handle(1), Qt::Key_Right);
        QCOMPARE(split.sizes().last(), lastSize);
        QTest::keyClick(split.handle(1), Qt::Key_End);
        QVERIFY(split.sizes().first() <= 340);
        QTest::keyClick(split.handle(1), Qt::Key_Home);
        QVERIFY(split.sizes().first() >= 160);
        split.setPaneCollapsed(0, true);
        split.resetPaneSizes();
        QVERIFY(!split.paneCollapsed(0));
        QVERIFY(!split.paneCollapsible(0));
        QVERIFY(qAbs(split.sizes().first() - 220) <= 2);
    }

    void splitRemembersSizesAcrossHideShow()
    {
        QtMaterialSplitView split;
        split.resize(640, 240);
        split.addWidget(new QWidget); split.addWidget(new QWidget);
        split.setRememberPaneSizes(true);
        QSignalSpy changed(&split, &QtMaterialSplitView::paneStateChanged);
        split.show();
        QVERIFY(QTest::qWaitForWindowExposed(&split));
        split.setSizes({180, 452});
        const QList<int> before = split.sizes();
        split.hide();
        QVERIFY(changed.count() > 0);
        split.setSizes({400, 232});
        split.show();
        QTRY_COMPARE(split.sizes(), before);
    }
};

QTEST_MAIN(tst_DesktopNavigationV2)
#include "tst_desktop_navigation_v2.moc"
