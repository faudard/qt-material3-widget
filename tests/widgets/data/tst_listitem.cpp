#include <QtTest>

#include <QImage>
#include <QPainter>
#include <QSignalSpy>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
#include "qtmaterial/widgets/data/qtmateriallistitem.h"

using namespace QtMaterial;

class tst_ListItem : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void construction();
    void selectionApi();
    void keyboardActivation();
    void sizeHintTouchTarget();
};

void tst_ListItem::construction()
{
    QtMaterialListItem item;

    QCOMPARE(
        item.headlineText(),
        QString());
    QCOMPARE(
        item.supportingText(),
        QString());
    QCOMPARE(
        item.densityVariant(),
        QtMaterialListItem::DensityVariant::Standard);
    QVERIFY(item.focusPolicy() == Qt::StrongFocus);
}

void tst_ListItem::selectionApi()
{
    QtMaterialListItem item;
    QSignalSpy spy(
        &item,
        &QtMaterialListItem::selectionChanged);

    item.setSelected(true);

    QVERIFY(item.isSelected());
    QCOMPARE(spy.count(), 1);

    item.setSelected(true);
    QCOMPARE(spy.count(), 1);
}

void tst_ListItem::keyboardActivation()
{
    QtMaterialListItem item;
    QSignalSpy clickedSpy(
        &item,
        &QtMaterialListItem::clicked);
    QSignalSpy activatedSpy(
        &item,
        &QtMaterialListItem::activated);

    item.setFocus();
    QTest::keyClick(
        &item,
        Qt::Key_Return);

    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(activatedSpy.count(), 1);
}

void tst_ListItem::sizeHintTouchTarget()
{
    QtMaterialListItem item;

    item.setDensityVariant(
        QtMaterialListItem::DensityVariant::Compact);
    const int compactHeight = item.sizeHint().height();

    item.setDensityVariant(
        QtMaterialListItem::DensityVariant::Standard);
    const int standardHeight = item.sizeHint().height();

    item.setDensityVariant(
        QtMaterialListItem::DensityVariant::Large);
    const int largeHeight = item.sizeHint().height();

    QVERIFY(compactHeight > 0);
    QVERIFY(compactHeight <= standardHeight);
    QVERIFY(standardHeight <= largeHeight);
}

QTEST_MAIN(tst_ListItem)

#include "tst_listitem.moc"
