#include <QtTest/QtTest>
#include <QWidget>
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
class tst_Filledbutton : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
};
void tst_Filledbutton::basicConstruction()
{
    QtMaterial::QtMaterialFilledButton widget; widget.setText(QStringLiteral("Filled")); QVERIFY(widget.sizeHint().height() >= 40);
}
QTEST_MAIN(tst_Filledbutton)
#include "tst_filledbutton.moc"
