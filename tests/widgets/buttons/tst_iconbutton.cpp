#include <QtTest/QtTest>
#include <QWidget>
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"
class tst_Iconbutton : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
};
void tst_Iconbutton::basicConstruction()
{
    QtMaterial::QtMaterialIconButton widget; QVERIFY(widget.sizeHint().width() >= 40);
}
QTEST_MAIN(tst_Iconbutton)
#include "tst_iconbutton.moc"
