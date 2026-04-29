#include <QtTest/QtTest>

#include "qtmaterial/core/qtmaterialasyncstate.h"

class tst_AsyncState : public QObject {
    Q_OBJECT
private slots:
    void progressClampsAndSetsDeterminate()
    {
        QtMaterial::QtMaterialAsyncState state;
        QVERIFY(state.isIndeterminate());
        state.setProgress(2.5);
        QVERIFY(state.hasProgress());
        QVERIFY(!state.isIndeterminate());
        QCOMPARE(state.progress(), 1.0);
        state.clearProgress();
        QVERIFY(!state.hasProgress());
        QVERIFY(state.isIndeterminate());
    }

    void propertyStringListsActiveState()
    {
        QtMaterial::QtMaterialAsyncState state;
        state.setBusy(true);
        state.setStatusText(QStringLiteral("Loading"));
        QVERIFY(state.toPropertyString().contains(QStringLiteral("busy")));
        QVERIFY(state.toPropertyString().contains(QStringLiteral("statusText")));
    }
};

QTEST_MAIN(tst_AsyncState)
#include "tst_asyncstate.moc"
