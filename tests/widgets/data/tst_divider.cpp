#include <QtTest/QtTest>
#include "qtmaterial/widgets/data/qtmaterialdivider.h"
class TestQtMaterialDivider : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
};
void TestQtMaterialDivider::constructs()
{
    QtMaterial::QtMaterialDivider widget;
    QVERIFY(true);
}
QTEST_MAIN(TestQtMaterialDivider)
#include "tst_divider.moc"
