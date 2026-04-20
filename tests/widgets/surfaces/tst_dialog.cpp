#include <QtTest/QtTest>
#include <QWidget>
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"
class tst_Dialog : public QObject
{
    Q_OBJECT
private slots:
    void basicConstruction();
};
void tst_Dialog::basicConstruction()
{
    QWidget host; host.resize(640, 480); QtMaterial::QtMaterialDialog widget(&host); QVERIFY(widget.parentWidget() == &host);
}
QTEST_MAIN(tst_Dialog)
#include "tst_dialog.moc"
