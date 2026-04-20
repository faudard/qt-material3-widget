#include <QtTest/QtTest>

// Focused scaffold for QtMaterialDivider render baselines.
// Intended snapshots:
// - horizontal divider
// - vertical divider
// - inset divider
// - dark theme divider

class tst_DividerRender : public QObject
{
    Q_OBJECT

private slots:
    void horizontalDivider();
    void verticalDivider();
    void insetDivider();
};

void tst_DividerRender::horizontalDivider()
{
    QSKIP("Scaffold only: render baseline system not wired yet.");
}

void tst_DividerRender::verticalDivider()
{
    QSKIP("Scaffold only: render baseline system not wired yet.");
}

void tst_DividerRender::insetDivider()
{
    QSKIP("Scaffold only: render baseline system not wired yet.");
}

QTEST_MAIN(tst_DividerRender)
#include "tst_divider_render.moc"
