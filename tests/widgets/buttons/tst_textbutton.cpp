#include <QtTest/QtTest>
#include <QWidget>
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
class tst_Textbutton : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
};
void tst_Textbutton::basicConstruction()
{
    QtMaterial::QtMaterialTextButton widget; widget.setText(QStringLiteral("Text")); QVERIFY(widget.sizeHint().width() >= 88);
}
QTEST_MAIN(tst_Textbutton)
#include "tst_textbutton.moc"
