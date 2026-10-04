#include <QAccessible>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QLocale>
#include <QPointer>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTest>
#include <QTimer>
#include <algorithm>
#include <memory>

#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"
#include "../../helpers/widgettestactivation.h"

using namespace QtMaterial;

class DeferredCommands final : public QtMaterialCommandProvider {
public:
    QList<quint64> requests;
    QList<quint64> cancelled;
    QStringList queries;
    QStringList activated;
    void requestCommands(const QString& query, quint64 request) override {
        requests.push_back(request); queries.push_back(query);
    }
    void cancelRequest(quint64 request) override { cancelled.push_back(request); }
    void activateCommand(const QString& id) override { activated.push_back(id); }
    void publish(quint64 request, const QList<QtMaterialCommand>& commands) {
        Q_EMIT commandsReady(request, commands);
    }
    void enqueue(quint64 request, const QList<QtMaterialCommand>& commands) {
        QTimer::singleShot(0, this, [this, request, commands]() { publish(request, commands); });
    }
};

static QtMaterialCommand command(const QString& id, const QString& title = QStringLiteral("Command")) {
    QtMaterialCommand result; result.id = id; result.text = title; return result;
}
static QListView* view(QtMaterialCommandPalette& palette) { return palette.findChild<QListView*>(); }
static QStringList resultIds(QtMaterialCommandPalette& palette) {
    QStringList ids;
    auto* model = view(palette)->model();
    for (int i = 0; i < model->rowCount(); ++i) {
        ids.push_back(model->index(i, 0).data(QtMaterialCommandPalette::IdRole).toString());
    }
    return ids;
}

class tst_CommandPaletteStress : public QObject {
    Q_OBJECT
private slots:
    void largeProviderSupportsFilteringAndTailActivation() {
        DeferredCommands provider;
        QtMaterialCommandPalette palette;
        palette.addProvider(&provider);
        palette.openPalette();
        QVERIFY(QTest::qWaitForWindowExposed(&palette));
        activateTestWindow(&palette);
        QList<QtMaterialCommand> commands;
        commands.reserve(10000);
        for (int i = 9999; i >= 0; --i) {
            const QString id = QStringLiteral("command.%1").arg(i, 5, 10, QLatin1Char('0'));
            auto item = command(id, QStringLiteral("Open resource %1").arg(i));
            item.keywords = QStringList{QStringLiteral("resource-%1").arg(i)};
            commands.push_back(item);
        }
        provider.publish(provider.requests.last(), commands);
        QCOMPARE(view(palette)->model()->rowCount(), 10000);
        QCOMPARE(resultIds(palette).first(), QStringLiteral("command.00000"));
        auto* search = palette.findChild<QLineEdit*>();
        QTRY_VERIFY(search->hasFocus());
        QTest::keyClick(search, Qt::Key_End);
        QCOMPARE(view(palette)->currentIndex().data(QtMaterialCommandPalette::IdRole).toString(), QStringLiteral("command.09999"));
        QTest::keyClick(search, Qt::Key_Return);
        QCOMPARE(provider.activated, QStringList{QStringLiteral("command.09999")});
        palette.openPalette();
        provider.publish(provider.requests.last(), commands);
        palette.setFuzzyMatchingEnabled(false);
        const int requests = provider.requests.size();
        palette.setQuery(QStringLiteral("resource-9999"));
        QCOMPARE(view(palette)->model()->rowCount(), 0);
        QTRY_COMPARE(provider.requests.size(), requests + 1);
        QCOMPARE(provider.queries.last(), QStringLiteral("resource-9999"));
        provider.publish(provider.requests.last(), commands);
        QCOMPARE(view(palette)->model()->rowCount(), 1);
        QCOMPARE(resultIds(palette), QStringList{QStringLiteral("command.09999")});
    }

    void queryBurstsAreDebouncedToTheLatestRequest() {
        DeferredCommands provider;
        QtMaterialCommandPalette palette;
        palette.addProvider(&provider);
        const quint64 original = provider.requests.last();
        for (int i = 0; i < 100; ++i) { palette.setQuery(QStringLiteral("query-%1").arg(i)); }
        QCOMPARE(provider.requests.size(), 1);
        QCOMPARE(provider.cancelled, QList<quint64>{original});
        QTRY_COMPARE(provider.requests.size(), 2);
        QCOMPARE(provider.queries.last(), QStringLiteral("query-99"));
        provider.publish(original, {command(QStringLiteral("old"), QStringLiteral("query-99 stale"))});
        QVERIFY(palette.isLoading());
        QCOMPARE(view(palette)->model()->rowCount(), 0);
        provider.publish(provider.requests.last(), {command(QStringLiteral("latest"), QStringLiteral("query-99 latest"))});
        QCOMPARE(resultIds(palette), QStringList{QStringLiteral("latest")});
        QVERIFY(!palette.isLoading());
    }

    void queuedRepliesAndErrorsCannotResurrectCancelledQueries() {
        DeferredCommands first, second;
        QtMaterialCommandPalette palette;
        palette.addProvider(&first); palette.addProvider(&second);
        QSignalSpy failures(&palette, &QtMaterialCommandPalette::providerFailed);
        QList<quint64> firstIds, secondIds;
        for (int i = 0; i < 30; ++i) {
            palette.setQuery(QStringLiteral("revision-%1").arg(i)); palette.refreshProviders();
            firstIds.push_back(first.requests.last()); secondIds.push_back(second.requests.last());
        }
        for (int i = firstIds.size() - 1; i >= 0; --i) {
            first.enqueue(firstIds.at(i), {command(QStringLiteral("first.%1").arg(i), QStringLiteral("revision-29 command"))});
            second.enqueue(secondIds.at(i), {command(QStringLiteral("second.%1").arg(i), QStringLiteral("revision-29 command"))});
            if (i != firstIds.size() - 1) { Q_EMIT first.requestFailed(firstIds.at(i), QStringLiteral("stale failure")); }
        }
        QTRY_VERIFY(!palette.isLoading());
        QCOMPARE(resultIds(palette), (QStringList{QStringLiteral("first.29"), QStringLiteral("second.29")}));
        QCOMPARE(failures.count(), 0);
        QVERIFY(first.cancelled.contains(firstIds.at(0)));
        QVERIFY(second.cancelled.contains(secondIds.at(0)));
    }

    void orderingAndDuplicateOwnershipIgnoreCompletionOrder_data() {
        QTest::addColumn<bool>("reverseCompletion");
        QTest::newRow("registration-order") << false;
        QTest::newRow("reverse-completion") << true;
    }
    void orderingAndDuplicateOwnershipIgnoreCompletionOrder() {
        QFETCH(bool, reverseCompletion);
        DeferredCommands first, second;
        QtMaterialCommandPalette palette;
        palette.addProvider(&first); palette.addProvider(&second);
        auto shared = command(QStringLiteral("shared"));
        shared.secondaryText = QStringLiteral("first owns this ID");
        const QList<QtMaterialCommand> a = {command(QStringLiteral("z")), shared, command(QStringLiteral("a"))};
        auto duplicate = shared; duplicate.secondaryText = QStringLiteral("second duplicate");
        const QList<QtMaterialCommand> b = {command(QStringLiteral("m")), duplicate};
        if (reverseCompletion) { second.publish(second.requests.last(), b); first.publish(first.requests.last(), a); }
        else { first.publish(first.requests.last(), a); second.publish(second.requests.last(), b); }
        const QStringList expected = {QStringLiteral("a"), QStringLiteral("m"), QStringLiteral("shared"), QStringLiteral("z")};
        QCOMPARE(resultIds(palette), expected);
        QCOMPARE(view(palette)->model()->index(2, 0).data(QtMaterialCommandPalette::SecondaryTextRole).toString(), shared.secondaryText);
        const int firstRequests = first.requests.size(), secondRequests = second.requests.size();
        palette.setQuery(QStringLiteral("command"));
        QVERIFY(resultIds(palette).isEmpty());
        QTRY_COMPARE(first.requests.size(), firstRequests + 1);
        QTRY_COMPARE(second.requests.size(), secondRequests + 1);
        if (reverseCompletion) { second.publish(second.requests.last(), b); first.publish(first.requests.last(), a); }
        else { first.publish(first.requests.last(), a); second.publish(second.requests.last(), b); }
        QCOMPARE(resultIds(palette), expected); // Equal fuzzy scores use stable IDs.
        palette.refreshProviders();
        auto shuffled = a; std::reverse(shuffled.begin(), shuffled.end());
        second.publish(second.requests.last(), b); first.publish(first.requests.last(), shuffled);
        QCOMPARE(resultIds(palette), expected);
    }

    void queuedRepliesAfterRemovalAndDestructionAreSafe() {
        auto provider = std::make_unique<DeferredCommands>();
        QtMaterialCommandPalette palette;
        palette.addProvider(provider.get());
        provider->enqueue(provider->requests.last(), {command(QStringLiteral("removed"))});
        palette.removeProvider(provider.get());
        QCoreApplication::processEvents();
        QVERIFY(resultIds(palette).isEmpty());
        palette.addProvider(provider.get());
        provider->enqueue(provider->requests.last(), {command(QStringLiteral("destroyed"))});
        provider.reset();
        QCoreApplication::processEvents();
        QVERIFY(palette.providers().isEmpty());
        QVERIFY(resultIds(palette).isEmpty());
        QVERIFY(!palette.isLoading());
        DeferredCommands survivor;
        auto dying = std::make_unique<QtMaterialCommandPalette>();
        dying->addProvider(&survivor);
        const quint64 pending = survivor.requests.last();
        survivor.enqueue(pending, {command(QStringLiteral("late"))});
        dying.reset();
        QVERIFY(survivor.cancelled.contains(pending));
        QCoreApplication::processEvents();
    }

    void sectionOrderingDoesNotDependOnLocale_data() {
        QTest::addColumn<QString>("localeName");
        QTest::newRow("english") << QStringLiteral("en_US");
        QTest::newRow("turkish") << QStringLiteral("tr_TR");
        QTest::newRow("swedish") << QStringLiteral("sv_SE");
    }
    void sectionOrderingDoesNotDependOnLocale() {
        QFETCH(QString, localeName);
        struct LocaleGuard {
            QLocale previous;
            ~LocaleGuard() { QLocale::setDefault(previous); }
        } localeGuard;
        QLocale::setDefault(QLocale(localeName));
        DeferredCommands provider;
        QtMaterialCommandPalette palette;
        palette.addProvider(&provider);
        QList<QtMaterialCommand> commands;
        const QStringList sections = {QString::fromUtf8("\xc3\xa5ngstr\xc3\xb6m"), QStringLiteral("zebra"),
            QStringLiteral("tools"), QStringLiteral("Tools"), QStringLiteral("alpha")};
        for (int i = 0; i < sections.size(); ++i) {
            auto item = command(QString::number(i)); item.section = sections.at(i); commands.push_back(item);
        }
        provider.publish(provider.requests.last(), commands);
        const QStringList expected = {QStringLiteral("4"), QStringLiteral("3"), QStringLiteral("2"), QStringLiteral("1"), QStringLiteral("0")};
        QCOMPARE(resultIds(palette), expected);
        const int requests = provider.requests.size();
        palette.setQuery(QStringLiteral("command"));
        QVERIFY(resultIds(palette).isEmpty());
        QTRY_COMPARE(provider.requests.size(), requests + 1);
        provider.publish(provider.requests.last(), commands);
        QCOMPARE(resultIds(palette), expected);
    }

    void invalidProviderCommandsAreRejectedAndSourceOrderIsPreserved() {
        DeferredCommands provider;
        QtMaterialCommandPalette palette;
        QStandardItemModel source;
        source.appendRow(new QStandardItem(QStringLiteral("Model Z")));
        source.appendRow(new QStandardItem(QStringLiteral("Model A")));
        palette.setSourceModel(&source);
        palette.addProvider(&provider);
        provider.publish(provider.requests.last(), {
            command(QStringLiteral("z")), command(QString(), QStringLiteral("Missing ID")),
            command(QStringLiteral("a")), command(QStringLiteral("empty-title"), QString())});
        QCOMPARE(resultIds(palette), (QStringList{QString(), QString(), QStringLiteral("a"), QStringLiteral("z")}));
        QCOMPARE(view(palette)->model()->index(0, 0).data().toString(), QStringLiteral("Model Z"));
        QCOMPARE(view(palette)->model()->index(1, 0).data().toString(), QStringLiteral("Model A"));
    }

    void hideCancelsAndReopeningRejectsOldReplies() {
        DeferredCommands provider;
        QtMaterialCommandPalette palette;
        palette.addProvider(&provider); palette.openPalette();
        const quint64 old = provider.requests.last();
        palette.reject();
        QVERIFY(provider.cancelled.contains(old));
        provider.publish(old, {command(QStringLiteral("stale"))});
        QVERIFY(resultIds(palette).isEmpty());
        QVERIFY(!palette.isLoading());
        palette.openPalette();
        const quint64 fresh = provider.requests.last();
        QVERIFY(fresh > old);
        provider.publish(old, {command(QStringLiteral("stale"))});
        QVERIFY(palette.isLoading());
        provider.publish(fresh, {command(QStringLiteral("fresh"))});
        QCOMPARE(resultIds(palette), QStringList{QStringLiteral("fresh")});
    }

    void keyboardOnlyFromHostToFavoriteActivationAndBack_data() {
        QTest::addColumn<int>("direction");
        QTest::addColumn<int>("shortcut");
        QTest::newRow("ltr-ctrl-k") << int(Qt::LeftToRight) << int(Qt::Key_K);
        QTest::newRow("rtl-ctrl-p") << int(Qt::RightToLeft) << int(Qt::Key_P);
    }
    void keyboardOnlyFromHostToFavoriteActivationAndBack() {
        QWidget host;
        QLineEdit invoker(&host); invoker.resize(200, 32);
        QtMaterialCommandPalette palette(&host);
        QFETCH(int, direction); QFETCH(int, shortcut);
        host.setLayoutDirection(Qt::LayoutDirection(direction));
        QStandardItemModel model;
        for (int i = 0; i < 24; ++i) {
            auto* item = new QStandardItem(QStringLiteral("Command %1").arg(i));
            item->setData(QStringLiteral("id.%1").arg(i), QtMaterialCommandPalette::IdRole);
            item->setEnabled(i != 0 && i != 12 && i != 23);
            model.appendRow(item);
        }
        palette.setSourceModel(&model);
        host.show(); QVERIFY(QTest::qWaitForWindowExposed(&host));
        activateTestWindow(&host); invoker.setFocus(); QTRY_VERIFY(invoker.hasFocus());
        QTest::keyClick(&invoker, Qt::Key(shortcut), Qt::ControlModifier);
        QTRY_VERIFY(palette.isVisible());
        auto* search = palette.findChild<QLineEdit*>();
        auto* results = view(palette);
        QTRY_VERIFY(search->hasFocus());
        QCOMPARE(results->currentIndex().row(), 1);
        QTest::keyClick(search, Qt::Key_End); QCOMPARE(results->currentIndex().row(), 22);
        QTest::keyClick(search, Qt::Key_Down); QCOMPARE(results->currentIndex().row(), 1);
        QTest::keyClick(search, Qt::Key_Up); QCOMPARE(results->currentIndex().row(), 22);
        QTest::keyClick(search, Qt::Key_Home); QCOMPARE(results->currentIndex().row(), 1);
        QTest::keyClick(search, Qt::Key_PageDown);
        QVERIFY(results->currentIndex().row() > 1);
        QVERIFY(results->currentIndex().flags().testFlag(Qt::ItemIsEnabled));
        const int pagedRow = results->currentIndex().row();
        QTest::keyClick(search, Qt::Key_PageUp);
        QVERIFY(results->currentIndex().row() < pagedRow);
        QVERIFY(results->currentIndex().flags().testFlag(Qt::ItemIsEnabled));
        QTest::keyClick(search, Qt::Key_Home);
        QTest::keyClick(search, Qt::Key_Tab); QTRY_VERIFY(results->hasFocus());
        QTest::keyClick(results, Qt::Key_Backtab); QTRY_VERIFY(search->hasFocus());
        QTest::keyClick(search, Qt::Key_D, Qt::ControlModifier);
        QCOMPARE(palette.favoriteCommandIds(), QStringList{QStringLiteral("id.1")});
        QSignalSpy activation(&palette, &QtMaterialCommandPalette::commandActivated);
        QTest::keyClick(search, Qt::Key_Return);
        QCOMPARE(activation.count(), 1);
        QCOMPARE(activation.first().first().value<QModelIndex>(), model.index(1, 0));
        QCOMPARE(palette.recentCommandIds(), QStringList{QStringLiteral("id.1")});
        QTRY_VERIFY(invoker.hasFocus());
        QTest::keyClick(&invoker, Qt::Key(shortcut), Qt::ControlModifier);
        QTRY_VERIFY(search->hasFocus());
        QTest::keyClick(search, Qt::Key_Escape);
        QVERIFY(!palette.isVisible()); QTRY_VERIFY(invoker.hasFocus());
    }

    void reopeningPalettePreservesInvokerLifetime_data() {
        QTest::addColumn<bool>("destroyInvoker");
        QTest::newRow("return-to-invoker") << false;
        QTest::newRow("invoker-destroyed") << true;
    }
    void reopeningPalettePreservesInvokerLifetime() {
        QFETCH(bool, destroyInvoker);
        QWidget host;
        auto invoker = std::make_unique<QLineEdit>(&host);
        QtMaterialCommandPalette palette(&host);
        host.show(); QVERIFY(QTest::qWaitForWindowExposed(&host));
        activateTestWindow(&host); invoker->setFocus(); QTRY_VERIFY(invoker->hasFocus());
        palette.openPalette();
        auto* search = palette.findChild<QLineEdit*>(); QVERIFY(search);
        QTRY_VERIFY(search->hasFocus());
        palette.openPalette(); // An already visible palette must keep its original invoker.
        if (destroyInvoker) { invoker.reset(); }
        palette.reject(); QVERIFY(!palette.isVisible());
        if (invoker) { QTRY_VERIFY(invoker->hasFocus()); }
        QCoreApplication::processEvents();
    }

    void emptyAndAllDisabledResultsCannotActivate() {
        QStandardItemModel model;
        auto* disabled = new QStandardItem(QStringLiteral("Unavailable"));
        disabled->setEnabled(false); model.appendRow(disabled);
        QtMaterialCommandPalette palette; palette.setSourceModel(&model);
        palette.openPalette(); QVERIFY(QTest::qWaitForWindowExposed(&palette));
        auto* search = palette.findChild<QLineEdit*>();
        QSignalSpy activated(&palette, &QtMaterialCommandPalette::commandActivated);
        for (Qt::Key key : {Qt::Key_Down, Qt::Key_Up, Qt::Key_Home, Qt::Key_End, Qt::Key_Return}) {
            QTest::keyClick(search, key); QVERIFY(!view(palette)->currentIndex().isValid());
        }
        QVERIFY(palette.isVisible()); QCOMPARE(activated.count(), 0);
        QTest::keyClicks(search, QStringLiteral("not found"));
        QCOMPARE(view(palette)->model()->rowCount(), 0);
        auto* empty = palette.findChild<QLabel*>(QStringLiteral("QtMaterialCommandPaletteEmptyState"));
        QVERIFY(empty->isVisible());
        auto* accessible = QAccessible::queryAccessibleInterface(search);
        QVERIFY(accessible); QCOMPARE(accessible->role(), QAccessible::EditableText);
        QTest::keyClick(search, Qt::Key_Escape); QVERIFY(!palette.isVisible());
    }
};

QTEST_MAIN(tst_CommandPaletteStress)
#include "tst_commandpalette_stress.moc"
