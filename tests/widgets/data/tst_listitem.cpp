#include <QtTest/QtTest>
#include "qtmaterial/widgets/data/qtmateriallistitem.h"
class TestQtMaterialListItem : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialListItem::constructs()
{
    QtMaterial::QtMaterialListItem widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialListItem)
#include "tst_listitem.moc"
