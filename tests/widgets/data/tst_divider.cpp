#include <QtTest/QtTest>

// Focused scaffold for QtMaterialDivider behavior tests.
// Intended coverage:
// - orientation API
// - inset API
// - size hint in both orientations

class tst_Divider : public QObject
{
    Q_OBJECT

private slots:
    void orientationApi();
    void insetApi();
    void sizeHints();
};

void tst_Divider::orientationApi()
{
    QSKIP("Scaffold only: QtMaterialDivider not wired yet.");
}

void tst_Divider::insetApi()
{
    QSKIP("Scaffold only: QtMaterialDivider not wired yet.");
}

void tst_Divider::sizeHints()
{
    QSKIP("Scaffold only: QtMaterialDivider not wired yet.");
}

QTEST_MAIN(tst_Divider)
#include "tst_divider.moc"
