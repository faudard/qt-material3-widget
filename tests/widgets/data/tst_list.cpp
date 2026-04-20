#include <QtTest/QtTest>

// Focused scaffold for QtMaterialList behavior tests.
// Intended coverage:
// - add/insert/take item flows
// - current-index tracking
// - keyboard navigation
// - divider toggle API

class tst_List : public QObject
{
    Q_OBJECT

private slots:
    void addAndCount();
    void keyboardNavigation();
    void dividerToggle();
};

void tst_List::addAndCount()
{
    QSKIP("Scaffold only: QtMaterialList not wired yet.");
}

void tst_List::keyboardNavigation()
{
    QSKIP("Scaffold only: QtMaterialList not wired yet.");
}

void tst_List::dividerToggle()
{
    QSKIP("Scaffold only: QtMaterialList not wired yet.");
}

QTEST_MAIN(tst_List)
#include "tst_list.moc"
