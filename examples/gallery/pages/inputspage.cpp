#include "inputspage.h"

#include <QDate>
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>

#include "qtmaterial/widgets/inputs/qtmaterialautocomplete.h"
#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"
#include "qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/widgets/qtmaterialdatepicker.h"

InputsPage::InputsPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    auto* outlined = new QtMaterial::QtMaterialOutlinedTextField(this);
    outlined->setLabelText(QStringLiteral("Outlined text field"));

    auto* filled = new QtMaterial::QtMaterialFilledTextField(this);
    filled->setLabelText(QStringLiteral("Filled text field"));

    auto* date = new QtMaterialDateField(this);
    date->setLabelText(QStringLiteral("Date field"));
    date->setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
    date->setPlaceholderTextForDate(QStringLiteral("dd/MM/yyyy"));

    auto* calendarDialog = new QDialog(this);
    calendarDialog->setWindowTitle(QStringLiteral("Choose a date"));
    calendarDialog->setModal(false);

    auto* calendarLayout = new QVBoxLayout(calendarDialog);
    auto* datePicker = new QtMaterial::QtMaterialDatePicker(calendarDialog);
    calendarLayout->addWidget(datePicker);

    auto* calendarButtons =
        new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
            calendarDialog);
    calendarLayout->addWidget(calendarButtons);

    connect(
        calendarButtons,
        &QDialogButtonBox::accepted,
        calendarDialog,
        [date, datePicker, calendarDialog]() {
            date->setDate(datePicker->selectedDate());
            calendarDialog->accept();
        });
    connect(
        calendarButtons,
        &QDialogButtonBox::rejected,
        calendarDialog,
        &QDialog::reject);

    connect(
        datePicker,
        &QtMaterial::QtMaterialDatePicker::activated,
        calendarDialog,
        [date, calendarDialog](const QDate& selectedDate) {
            date->setDate(selectedDate);
            calendarDialog->accept();
        });

    connect(
        date,
        &QtMaterialDateField::calendarRequested,
        this,
        [date, datePicker, calendarDialog]() {
            if (date->minimumDate().isValid()) {
                datePicker->setMinimumDate(date->minimumDate());
            }
            if (date->maximumDate().isValid()) {
                datePicker->setMaximumDate(date->maximumDate());
            }

            QDate selectedDate =
                date->date().isValid()
                    ? date->date()
                    : QDate::currentDate();

            if (date->minimumDate().isValid()
                && selectedDate < date->minimumDate()) {
                selectedDate = date->minimumDate();
            }
            if (date->maximumDate().isValid()
                && selectedDate > date->maximumDate()) {
                selectedDate = date->maximumDate();
            }

            datePicker->setSelectedDate(selectedDate);
            calendarDialog->adjustSize();
            calendarDialog->show();
            calendarDialog->raise();
            calendarDialog->activateWindow();
        });

    auto* autocomplete =
        new QtMaterial::QtMaterialAutocomplete(this);
    autocomplete->setPlaceholderText(
        QStringLiteral("Autocomplete"));
    autocomplete->setSuggestions({
        QStringLiteral("Alpha"),
        QStringLiteral("Beta"),
        QStringLiteral("Gamma"),
        QStringLiteral("Delta")
    });

    layout->addWidget(outlined);
    layout->addWidget(filled);
    layout->addWidget(date);
    layout->addWidget(autocomplete);
    layout->addStretch(1);
}
