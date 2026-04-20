#include <QtTest/QtTest>

#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"

class tst_QtMaterialCard : public QObject
{
    Q_OBJECT

private slots:
    void construction();
    void titleBodySetters();
    void sizeHintUsesMinimums();
};

void tst_QtMaterialCard::construction()
{
    QtMaterial::QtMaterialCard card;
    QVERIFY(card.sizeHint().isValid());
}

void tst_QtMaterialCard::titleBodySetters()
{
    QtMaterial::QtMaterialCard card;
    card.setTitleText(QStringLiteral("Card title"));
    card.setBodyText(QStringLiteral("Card body"));
    QCOMPARE(card.titleText(), QStringLiteral("Card title"));
    QCOMPARE(card.bodyText(), QStringLiteral("Card body"));
}

void tst_QtMaterialCard::sizeHintUsesMinimums()
{
    QtMaterial::QtMaterialCard card;
    const QSize size = card.sizeHint();
    QVERIFY(size.width() >= card.minimumSizeHint().width());
    QVERIFY(size.height() >= card.minimumSizeHint().height());
}

QTEST_MAIN(tst_QtMaterialCard)
#include "tst_card.moc"
