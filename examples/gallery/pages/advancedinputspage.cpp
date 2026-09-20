#include "advancedinputspage.h"

#include <QFormLayout>
#include <QLabel>
#include <QStringListModel>
#include <QVBoxLayout>

#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"
#include "qtmaterial/widgets/inputs/qtmaterialdaterangepicker.h"
#include "qtmaterial/widgets/inputs/qtmaterialrangeslider.h"
#include "qtmaterial/widgets/inputs/qtmaterialsearchview.h"
#include "qtmaterial/widgets/inputs/qtmaterialslider.h"
#include "qtmaterial/widgets/inputs/qtmaterialtimefield.h"

AdvancedInputsPage::AdvancedInputsPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* title = new QLabel(tr("0.8 — Advanced inputs"), this);
    QFont titleFont = title->font();
    titleFont.setPointSize(titleFont.pointSize() + 4);
    titleFont.setBold(true);
    title->setFont(titleFont);
    layout->addWidget(title);

    auto* searchModel = new QStringListModel(
        {tr("Search Bar"), tr("Search View"), tr("Combo Box"), tr("Slider"), tr("Time Picker")},
        this);
    auto* searchView = new QtMaterial::QtMaterialSearchView(this);
    searchView->setSourceModel(searchModel);
    searchView->searchBar()->setPlaceholderText(tr("Filter components"));
    searchView->setMaximumHeight(190);
    layout->addWidget(searchView);

    auto* form = new QFormLayout;
    auto* combo = new QtMaterial::QtMaterialComboBox(this);
    combo->setLabelText(tr("Density"));
    combo->addItems({tr("Standard"), tr("Compact"), tr("Comfortable")});
    form->addRow(tr("Combo"), combo);

    auto* slider = new QtMaterial::QtMaterialSlider(Qt::Horizontal, this);
    slider->setRange(0, 100);
    slider->setValue(45);
    form->addRow(tr("Slider"), slider);

    auto* range = new QtMaterial::QtMaterialRangeSlider(this);
    range->setRange(0, 100);
    range->setValues(20, 80);
    form->addRow(tr("Range"), range);

    auto* time = new QtMaterial::QtMaterialTimeField(this);
    form->addRow(tr("Time"), time);

    layout->addLayout(form);

    auto* dateRange = new QtMaterial::QtMaterialDateRangePicker(this);
    dateRange->setMaximumHeight(360);
    layout->addWidget(dateRange);

    layout->addStretch();
}
