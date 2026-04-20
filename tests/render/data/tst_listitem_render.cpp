#include <QtTest/QtTest>

// Focused scaffold for list-item render baselines.

class tst_ListItemRender : public QObject
{
    Q_OBJECT

private slots:
    void oneLineStates();
    void twoLineStates();
    void selectedStates();
};

void tst_ListItemRender::oneLineStates()
{
    QSKIP("Scaffold only: render baseline system not wired yet.");
}

void tst_ListItemRender::twoLineStates()
{
    QSKIP("Scaffold only: render baseline system not wired yet.");
}

void tst_ListItemRender::selectedStates()
{
    QSKIP("Scaffold only: render baseline system not wired yet.");
}

QTEST_MAIN(tst_ListItemRender)
#include "tst_listitem_render.moc"
