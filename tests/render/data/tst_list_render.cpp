#include <QtTest/QtTest>

// Focused scaffold for QtMaterialList render baselines.
// Intended snapshots:
// - neutral list with three items
// - focused/current-item list
// - dividers on/off
// - dark theme list

class tst_ListRender : public QObject
{
    Q_OBJECT

private slots:
    void basicList();
    void focusedList();
    void dividedList();
};

void tst_ListRender::basicList()
{
    QSKIP("Scaffold only: render baseline system not wired yet.");
}

void tst_ListRender::focusedList()
{
    QSKIP("Scaffold only: render baseline system not wired yet.");
}

void tst_ListRender::dividedList()
{
    QSKIP("Scaffold only: render baseline system not wired yet.");
}

QTEST_MAIN(tst_ListRender)
#include "tst_list_render.moc"
