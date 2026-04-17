#include <QtTest/QtTest>
#include <QWidget>
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
class tst_Outlinedtextfield : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
};
void tst_Outlinedtextfield::basicConstruction()
{
    QtMaterial::QtMaterialOutlinedTextField widget; widget.setLabelText(QStringLiteral("Label")); QVERIFY(widget.lineEdit() != nullptr);
}
QTEST_MAIN(tst_Outlinedtextfield)
#include "tst_outlinedtextfield.moc"
