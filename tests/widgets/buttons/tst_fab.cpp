#include <QtTest/QtTest>
#include <QStyle>
#include "qtmaterial/widgets/buttons/qtmaterialfab.h"
class TestQtMaterialFab : public QObject
{
    Q_OBJECT
private slots:
    void constructs();
    void keepsSquareMinimumSize();
};
void TestQtMaterialFab::constructs()
{
    QtMaterial::QtMaterialFab widget;
    widget.setIcon(widget.style()->standardIcon(QStyle::SP_ArrowUp));
    QVERIFY(true);
}
void TestQtMaterialFab::keepsSquareMinimumSize()
{
    QtMaterial::QtMaterialFab widget;
    QCOMPARE(widget.minimumWidth(), widget.minimumHeight());
}
QTEST_MAIN(TestQtMaterialFab)
#include "tst_fab.moc"
