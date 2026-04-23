#include <QtTest/QtTest>
#include <QImage>
#include <QPainter>
#include "qtmaterial/widgets/data/qtmaterialdivider.h"

using namespace QtMaterial;

class tst_Divider : public QObject
{
    Q_OBJECT

private slots:
    void defaultConstruction();
    void orientationApi();
    void insetNormalization();
    void sizeHints();
};

void tst_Divider::defaultConstruction()
{
    QtMaterialDivider divider;
    QCOMPARE(divider.orientation(), Qt::Horizontal);
    QCOMPARE(divider.leadingInset(), 0);
    QCOMPARE(divider.trailingInset(), 0);
}

void tst_Divider::orientationApi()
{
    QtMaterialDivider divider;
    divider.setOrientation(Qt::Vertical);
    QCOMPARE(divider.orientation(), Qt::Vertical);

    divider.setOrientation(Qt::Horizontal);
    QCOMPARE(divider.orientation(), Qt::Horizontal);
}

void tst_Divider::insetNormalization()
{
    QtMaterialDivider divider;
    divider.setLeadingInset(12);
    divider.setTrailingInset(18);
    QCOMPARE(divider.leadingInset(), 12);
    QCOMPARE(divider.trailingInset(), 18);

    divider.setLeadingInset(-4);
    divider.setTrailingInset(-9);
    QCOMPARE(divider.leadingInset(), 0);
    QCOMPARE(divider.trailingInset(), 0);
}

void tst_Divider::sizeHints()
{
    QtMaterialDivider h(Qt::Horizontal);
    QVERIFY(h.sizeHint().width() >= 1);
    QVERIFY(h.sizeHint().height() >= 1);

    QtMaterialDivider v(Qt::Vertical);
    QVERIFY(v.sizeHint().width() >= 1);
    QVERIFY(v.sizeHint().height() >= 1);
}

QTEST_MAIN(tst_Divider)
#include "tst_divider.moc"