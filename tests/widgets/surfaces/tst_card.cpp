#include <QtTest/QtTest>
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"
class TestQtMaterialCard : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialCard::constructs()
{
    QtMaterial::QtMaterialCard widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialCard)
#include "tst_card.moc"
