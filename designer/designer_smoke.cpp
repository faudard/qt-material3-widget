#include <QApplication>
#include <QStringList>
#include <QWidget>

#include "ui_designer_smoke.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QWidget form;
    Ui::DesignerSmokeForm ui;
    ui.setupUi(&form);

    if (!ui.filledButton || !ui.outlinedTextField || !ui.slider
        || !ui.dateField || !ui.searchBar || !ui.rangeSlider || !ui.divider
        || !ui.card || !ui.table || !ui.topAppBar || !ui.bottomAppBar
        || !ui.buttonGroup || !ui.navigationBar || !ui.segmentedList
        || !ui.splitButton || !ui.loadingIndicator) {
        return 1;
    }

    if (ui.slider->parentWidget() != &form
        || ui.slider->orientation() != Qt::Vertical
        || ui.slider->value() != 37) {
        return 2;
    }
    if (ui.outlinedTextField->text() != QString()
        || ui.outlinedTextField->placeholderText() != QStringLiteral("name@example.com")
        || ui.outlinedTextField->maxLength() != 120) {
        return 3;
    }
    if (ui.dateField->displayFormat() != QStringLiteral("dd/MM/yyyy")
        || !ui.dateField->isClearable()) {
        return 4;
    }
    if (ui.rangeSlider->lowerValue() != 20 || ui.rangeSlider->upperValue() != 80) {
        return 5;
    }
    if (ui.topAppBar->title() != QStringLiteral("Designer smoke")
        || ui.bottomAppBar->title() != QStringLiteral("Designer actions")) {
        return 6;
    }
    if (ui.buttonGroup->buttonLabels()
            != QStringList({QStringLiteral("Day"), QStringLiteral("Week"), QStringLiteral("Month")})
        || ui.buttonGroup->currentIndex() != 1) {
        return 7;
    }
    if (ui.navigationBar->destinationLabels()
            != QStringList({QStringLiteral("Home"), QStringLiteral("Search"), QStringLiteral("Profile")})
        || ui.navigationBar->currentIndex() != 1) {
        return 8;
    }
    if (ui.segmentedList->itemLabels()
            != QStringList({QStringLiteral("Personal"), QStringLiteral("Work"), QStringLiteral("Archive")})
        || !ui.segmentedList->expressive()) {
        return 9;
    }
    if (ui.splitButton->text() != QStringLiteral("Create")
        || !ui.splitButton->expressive()
        || ui.loadingIndicator->indicatorSize() != 44
        || ui.loadingIndicator->isActive()) {
        return 10;
    }

    return 0;
}
