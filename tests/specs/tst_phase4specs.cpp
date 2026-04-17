#include <QtTest/QtTest>
#include "qtmaterial/specs/qtmaterialspecfactory.h"
class TestSpecFactory : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestSpecFactory::constructs()
{
    QtMaterial::SpecFactory factory;
    QVERIFY(true);
}
QTEST_MAIN(TestSpecFactory)
#include "tst_phase4specs.moc"
