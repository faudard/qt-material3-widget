#include <QtTest/QtTest>

#include <QAbstractItemView>
#include <QLineEdit>
#include <QPalette>
#include <QListView>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QTimeEdit>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemecontext.h"
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

    void comboBoxUsesMaterialThemeAndKeepsNativeContracts()
    {
        Theme theme = ThemeBuilder().buildLightFromSeed(
            QColor(QStringLiteral("#6750A4")));
        theme.shapes().setRadius(ShapeRole::ExtraSmall, 9);
        ThemeContext context(theme);

        QtMaterialComboBox combo;
        combo.setThemeContext(&context);
        combo.addItems({
            QStringLiteral("2022"),
            QStringLiteral("2023"),
            QStringLiteral("2024")
        });
        combo.setCurrentIndex(2);

        QCOMPARE(combo.effectiveThemeContext(), &context);
        QCOMPARE(combo.currentText(), QStringLiteral("2024"));
        QCOMPARE(
            combo.palette().color(QPalette::Base),
            theme.colorScheme().color(
                ColorRole::SurfaceContainerHighest));
        QCOMPARE(
            combo.palette().color(QPalette::Highlight),
            theme.colorScheme().color(
                ColorRole::SecondaryContainer));
        QVERIFY(combo.minimumSizeHint().height() >= 40);
        QVERIFY(combo.view());
        QVERIFY(combo.view()->itemDelegate());

        combo.setEditable(true);
        QVERIFY(combo.lineEdit());
        combo.setEditText(QStringLiteral("Custom"));
        QCOMPARE(combo.currentText(), QStringLiteral("Custom"));
    }

    void comboBoxThemeChangesRefreshResolvedStyle()
    {
        const Theme first =
            ThemeBuilder().buildLightFromSeed(
                QColor(QStringLiteral("#6750A4")));
        const Theme second =
            ThemeBuilder().buildDarkFromSeed(
                QColor(QStringLiteral("#006874")));
        ThemeContext context(first);

        QtMaterialComboBox combo;
        combo.setThemeContext(&context);

        QCOMPARE(
            combo.palette().color(QPalette::Base),
            first.colorScheme().color(
                ColorRole::SurfaceContainerHighest));
        QCOMPARE(
            combo.palette().color(QPalette::Highlight),
            first.colorScheme().color(
                ColorRole::SecondaryContainer));

        QVERIFY(context.setTheme(second));

        QCOMPARE(
            combo.palette().color(QPalette::Base),
            second.colorScheme().color(
                ColorRole::SurfaceContainerHighest));
        QCOMPARE(
            combo.palette().color(QPalette::Highlight),
            second.colorScheme().color(
                ColorRole::SecondaryContainer));
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

    void inputKeyboardAccessibilityAndRtlContracts()
    {
        QtMaterialSearchBar search;
        QCOMPARE(search.focusProxy(), search.lineEdit());
        QCOMPARE(search.accessibleName(), QStringLiteral("Search"));
        search.setText(QStringLiteral("material"));
        search.setLayoutDirection(Qt::RightToLeft);
        QCOMPARE(search.lineEdit()->layoutDirection(), Qt::RightToLeft);

        QSignalSpy requested(&search, &QtMaterialSearchBar::searchRequested);
        QTest::keyClick(search.lineEdit(), Qt::Key_Return);
        QCOMPARE(requested.count(), 1);

        QStringListModel model({
            QStringLiteral("Alpha"),
            QStringLiteral("Beta"),
            QStringLiteral("Gamma")
        });
        QtMaterialSearchView view;
        view.setSourceModel(&model);
        view.setLayoutDirection(Qt::RightToLeft);
        view.searchBar()->setText(QStringLiteral("Gamma"));
        QCOMPARE(view.resultView()->model()->rowCount(), 1);
        QCOMPARE(view.resultView()->layoutDirection(), Qt::RightToLeft);

        QtMaterialRangeSlider range;
        range.setRange(0, 100);
        range.setValues(25, 75);
        range.setFocus();
        QTest::keyClick(&range, Qt::Key_Right);
        QVERIFY(range.lowerValue() >= 25);
        range.setLayoutDirection(Qt::RightToLeft);
        const QSize rtlHint = range.sizeHint();
        range.setLayoutDirection(Qt::LeftToRight);
        QCOMPARE(range.sizeHint(), rtlHint);

        QtMaterialDateRangePicker dates;
        dates.setDateRange(QDate(2026, 10, 10), QDate(2026, 10, 3));
        QCOMPARE(dates.startDate(), QDate(2026, 10, 3));
        QCOMPARE(dates.endDate(), QDate(2026, 10, 10));
        dates.setLayoutDirection(Qt::RightToLeft);
        QCOMPARE(dates.startPicker()->layoutDirection(), Qt::RightToLeft);
        QCOMPARE(dates.endPicker()->layoutDirection(), Qt::RightToLeft);

        QtMaterialTimePicker time;
        QCOMPARE(time.accessibleName(), QStringLiteral("Time picker"));
        time.setSelectedTime(QTime(14, 35));
        QCOMPARE(time.timeField()->time(), QTime(14, 35));
        time.setLayoutDirection(Qt::RightToLeft);
        QCOMPARE(time.timeField()->layoutDirection(), Qt::RightToLeft);
    }

    void disabledInputsDoNotMutateFromKeyboard()
    {
        QtMaterialSlider slider;
        slider.setRange(0, 10);
        slider.setValue(5);
        slider.setEnabled(false);
        QTest::keyClick(&slider, Qt::Key_Right);
        QCOMPARE(slider.value(), 5);

        QtMaterialRangeSlider range;
        range.setRange(0, 100);
        range.setValues(20, 80);
        range.setEnabled(false);
        QTest::keyClick(&range, Qt::Key_Right);
        QCOMPARE(range.lowerValue(), 20);
        QCOMPARE(range.upperValue(), 80);

        QtMaterialSearchBar search;
        search.setText(QStringLiteral("locked"));
        search.setEnabled(false);
        QTest::keyClick(search.lineEdit(), Qt::Key_A);
        QCOMPARE(search.text(), QStringLiteral("locked"));
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

        chip.setVariant(ChipVariant::Assist);
        QVERIFY(!chip.isCheckable());
        chip.setRemovable(false);

        chip.setVariant(ChipVariant::Suggestion);
        QVERIFY(!chip.isCheckable());

        QCOMPARE(chip.accessibleName(), QStringLiteral("Filter"));
        QCOMPARE(chip.focusPolicy(), Qt::StrongFocus);
    }

    void chipDesktopScaleFactors_data()
    {
        QTest::addColumn<qreal>("dpr");
        QTest::newRow("100-percent") << qreal(1.00);
        QTest::newRow("125-percent") << qreal(1.25);
        QTest::newRow("150-percent") << qreal(1.50);
        QTest::newRow("175-percent") << qreal(1.75);
        QTest::newRow("200-percent") << qreal(2.00);
    }

    void chipDesktopScaleFactors()
    {
        QFETCH(qreal, dpr);
        const QList<ChipVariant> variants = {
            ChipVariant::Assist,
            ChipVariant::Filter,
            ChipVariant::Input,
            ChipVariant::Suggestion
        };
        for (ChipVariant variant : variants) {
            QtMaterialChip chip(QStringLiteral("Chip"));
            chip.setVariant(variant);
            chip.setLayoutDirection(Qt::RightToLeft);
            chip.resize(qMax(120, chip.sizeHint().width()), qMax(48, chip.sizeHint().height()));

            QPixmap pixmap(
                qMax(1, qRound(chip.width() * dpr)),
                qMax(1, qRound(chip.height() * dpr)));
            pixmap.setDevicePixelRatio(dpr);
            pixmap.fill(Qt::transparent);
            chip.render(&pixmap);
            QVERIFY(!pixmap.isNull());
            QCOMPARE(pixmap.devicePixelRatio(), dpr);
        }
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
