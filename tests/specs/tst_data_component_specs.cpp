#include <QtTest/QtTest>

#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"

class tst_DataComponentSpecs : public QObject {
    Q_OBJECT

private slots:
    void defaultsAreUsable()
    {
        const auto date = QtMaterial::defaultDatePickerSpec();
        QVERIFY(date.backgroundColor.isValid());
        QVERIFY(date.selectedDateColor.isValid());
        QVERIFY(date.cellSize > 0);

        const auto table = QtMaterial::defaultTableSpec();
        QVERIFY(table.backgroundColor.isValid());
        QVERIFY(table.rowHeight > 0);

        const auto grid = QtMaterial::defaultGridListSpec();
        QVERIFY(grid.itemSize.isValid());
        QVERIFY(grid.spacing >= 0);

        const auto carousel = QtMaterial::defaultCarouselSpec();
        QVERIFY(carousel.preferredPageSize.isValid());
        QVERIFY(carousel.indicatorSize > 0);
    }
};

QTEST_MAIN(tst_DataComponentSpecs)
#include "tst_data_component_specs.moc"
