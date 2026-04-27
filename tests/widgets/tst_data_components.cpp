#include <QtTest/QtTest>
#include <QStandardItemModel>
#include <QLabel>

#include "qtmaterial/widgets/qtmaterialdatepicker.h"
#include "qtmaterial/widgets/qtmaterialtable.h"
#include "qtmaterial/widgets/qtmaterialgridlist.h"
#include "qtmaterial/widgets/qtmaterialcarousel.h"

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

    void gridListAcceptsModel()
    {
        QStandardItemModel model;
        auto* item = new QStandardItem(QStringLiteral("Card"));
        item->setData(QStringLiteral("Supporting text"), Qt::UserRole + 1);
        model.appendRow(item);

        QtMaterial::QtMaterialGridList grid;
        grid.setModel(&model);
        grid.setColumns(2);
        QCOMPARE(grid.model(), &model);
        QCOMPARE(grid.columns(), 2);
    }

    void carouselNavigation()
    {
        QtMaterial::QtMaterialCarousel carousel;
        carousel.addPage(new QLabel(QStringLiteral("One")), QStringLiteral("One"));
        carousel.addPage(new QLabel(QStringLiteral("Two")), QStringLiteral("Two"));
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
