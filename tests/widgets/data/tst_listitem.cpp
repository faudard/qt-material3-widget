#include <QtTest/QtTest>

// Focused scaffold for QtMaterialListItem tests.

class tst_ListItem : public QObject
{
    Q_OBJECT

private slots:
    void construction();
    void selectionApi();
    void keyboardActivation();
    void sizeHintTouchTarget();
};

void tst_ListItem::construction()
{
    QSKIP("Scaffold only: wire once QtMaterialListItem is integrated.");
}

void tst_ListItem::selectionApi()
{
    QSKIP("Scaffold only: wire once QtMaterialListItem is integrated.");
}

void tst_ListItem::keyboardActivation()
{
    QSKIP("Scaffold only: wire once QtMaterialListItem is integrated.");
}

void tst_ListItem::sizeHintTouchTarget()
{
    QSKIP("Scaffold only: wire once QtMaterialListItem is integrated.");
}

QTEST_MAIN(tst_ListItem)
#include "tst_listitem.moc"
