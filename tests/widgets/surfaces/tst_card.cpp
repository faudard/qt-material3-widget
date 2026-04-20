#include <QtTest/QtTest>

#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"

class tst_Card : public QObject
{
    Q_OBJECT

private slots:
    void hasSaneMinimumSize();
};

void tst_Card::hasSaneMinimumSize()
{
    QtMaterial::QtMaterialCard card;
    QVERIFY(card.minimumSizeHint().width() >= 160);
    QVERIFY(card.minimumSizeHint().height() >= 100);
}

QTEST_MAIN(tst_Card)
#include "tst_card.moc"
