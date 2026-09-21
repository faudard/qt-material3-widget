#include <QtTest/QtTest>

#include <QLineEdit>
#include <QListView>
#include <QStringListModel>
#include <QStandardItemModel>

#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"
#include "qtmaterial/widgets/inputs/qtmaterialdaterangepicker.h"
#include "qtmaterial/widgets/inputs/qtmaterialrangeslider.h"
#include "qtmaterial/widgets/inputs/qtmaterialsearchbar.h"
#include "qtmaterial/widgets/inputs/qtmaterialsearchview.h"
#include "qtmaterial/widgets/inputs/qtmaterialslider.h"
#include "qtmaterial/widgets/inputs/qtmaterialtimefield.h"
#include "qtmaterial/widgets/inputs/qtmaterialtimepicker.h"
#include "qtmaterial/widgets/selection/qtmaterialchip.h"
#include "qtmaterial/widgets/navigation/qtmaterialmenu.h"
#include "qtmaterial/widgets/data/qtmaterialtable.h"

using namespace QtMaterial;

class tst_ComponentExpansion : public QObject
{
    Q_OBJECT

private slots:
    void searchBarContract()
    {
        QtMaterialSearchBar search;
        QSignalSpy requested(&search, &QtMaterialSearchBar::searchRequested);
        search.setText(QStringLiteral("material"));
        QCOMPARE(search.text(), QStringLiteral("material"));
        QTest::keyClick(search.lineEdit(), Qt::Key_Return);
        QCOMPARE(requested.count(), 1);
    }

    void searchViewFiltersModel()
    {
        QStringListModel model({
            QStringLiteral("Alpha"),
            QStringLiteral("Beta"),
            QStringLiteral("Gamma")
        });
        QtMaterialSearchView view;
        view.setSourceModel(&model);
        view.searchBar()->setText(QStringLiteral("Beta"));
        QCOMPARE(view.resultView()->model()->rowCount(), 1);
    }

    void comboBoxLabelIsAccessible()
    {
        QtMaterialComboBox combo;
        combo.setLabelText(QStringLiteral("Country"));
        QCOMPARE(combo.labelText(), QStringLiteral("Country"));
        QCOMPARE(combo.accessibleName(), QStringLiteral("Country"));
    }

    void sliderFamilyKeepsRangeInvariant()
    {
        QtMaterialSlider slider;
        slider.setRange(0, 10);
        slider.setValue(4);
        QCOMPARE(slider.value(), 4);

        QtMaterialRangeSlider range;
        range.setRange(0, 100);
        range.setValues(80, 20);
        QCOMPARE(range.lowerValue(), 20);
        QCOMPARE(range.upperValue(), 80);
        range.setLowerValue(90);
        QCOMPARE(range.lowerValue(), 80);
    }

    void dateAndTimeContracts()
    {
        QtMaterialTimeField field;
        const QTime time(14, 35);
        field.setTime(time);
        QCOMPARE(field.time(), time);

        QtMaterialTimePicker picker;
        picker.setSelectedTime(time);
        QCOMPARE(picker.selectedTime(), time);

        QtMaterialDateRangePicker range;
        const QDate first(2026, 9, 10);
        const QDate second(2026, 9, 20);
        range.setDateRange(second, first);
        QCOMPARE(range.startDate(), first);
        QCOMPARE(range.endDate(), second);
    }

    void chipFamilyVariants()
    {
        QtMaterialChip chip(QStringLiteral("Filter"));
        chip.setVariant(ChipVariant::Filter);
        QVERIFY(chip.isCheckable());
        chip.setChecked(true);
        QVERIFY(chip.isChecked());

        chip.setVariant(ChipVariant::Input);
        QVERIFY(!chip.isCheckable());
        chip.setRemovable(true);
        QVERIFY(chip.isRemovable());
    }

    void menuAndDataExpansion()
    {
        QtMaterialMenu menu;
        const int first = menu.addItem(QStringLiteral("One"));
        const int second = menu.addItem(QStringLiteral("Two"));
        menu.setItemExclusiveGroup(first, 1);
        menu.setItemExclusiveGroup(second, 1);
        menu.setItemChecked(first, true);
        QVERIFY(menu.isItemChecked(first));
        QCOMPARE(menu.itemExclusiveGroup(second), 1);

        QStandardItemModel model(4, 2);
        QtMaterialTable table;
        table.setModel(&model);
        QVERIFY(!table.multiSelectionEnabled());
        table.setMultiSelectionEnabled(true);
        QVERIFY(table.multiSelectionEnabled());
        table.setDense(true);
        QVERIFY(table.dense());
    }
};

QTEST_MAIN(tst_ComponentExpansion)
#include "tst_component_expansion.moc"
