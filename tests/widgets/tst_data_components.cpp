#include <QtTest/QtTest>
#include <QStandardItemModel>

#include "qtmaterial/widgets/qtmaterialdatepicker.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"
#include "qtmaterial/widgets/data/qtmaterialgridlist.h"
#include "qtmaterial/widgets/data/qtmaterialcarousel.h"

class tst_DataComponents : public QObject {
    Q_OBJECT

private slots:
    void datePickerSelectionSignal()
    {
        QtMaterial::QtMaterialDatePicker picker;
        QSignalSpy spy(&picker, &QtMaterial::QtMaterialDatePicker::selectedDateChanged);
        const QDate date(2026, 4, 25);
        picker.setSelectedDate(date);
        QCOMPARE(picker.selectedDate(), date);
        QCOMPARE(spy.count(), 1);
    }

    void tableAcceptsModel()
    {
        QStandardItemModel model(2, 2);
        model.setHeaderData(0, Qt::Horizontal, QStringLiteral("Name"));
        model.setHeaderData(1, Qt::Horizontal, QStringLiteral("Status"));
        model.setData(model.index(0, 0), QStringLiteral("Alpha"));
        model.setData(model.index(0, 1), QStringLiteral("Ready"));

        QtMaterial::QtMaterialTable table;
        table.setModel(&model);
        QCOMPARE(table.model(), &model);
        QVERIFY(table.spec().rowHeight > 0);
    }

    void gridListItems()
    {
        QtMaterial::QtMaterialGridList grid;
        const int index = grid.addGridItem(
            QStringLiteral("Card"),
            QStringLiteral("Supporting text"));
        grid.setColumns(2);

        QCOMPARE(index, 0);
        QCOMPARE(grid.count(), 1);
        QCOMPARE(grid.itemTitle(0), QStringLiteral("Card"));
        QCOMPARE(
            grid.itemSupportingText(0),
            QStringLiteral("Supporting text"));
        QCOMPARE(grid.columns(), 2);
    }

    void carouselNavigation()
    {
        QtMaterial::QtMaterialCarousel carousel;
        carousel.addItem(QStringLiteral("One"));
        carousel.addItem(QStringLiteral("Two"));
        QCOMPARE(carousel.count(), 2);
        QCOMPARE(carousel.currentIndex(), 0);
        carousel.next();
        QCOMPARE(carousel.currentIndex(), 1);
        carousel.previous();
        QCOMPARE(carousel.currentIndex(), 0);
    }
};

QTEST_MAIN(tst_DataComponents)
#include "tst_data_components.moc"
