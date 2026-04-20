#include <QtTest/QtTest>

#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"

class tst_FilledTextField : public QObject
{
    Q_OBJECT

private slots:
    void createsLineEditHost();
};

void tst_FilledTextField::createsLineEditHost()
{
    QtMaterial::QtMaterialFilledTextField field;
    QVERIFY(field.lineEdit() != nullptr);
}

QTEST_MAIN(tst_FilledTextField)
#include "tst_filledtextfield.moc"
