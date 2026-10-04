#include <QApplication>
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
        || !ui.card || !ui.table || !ui.topAppBar || !ui.bottomAppBar) {
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

    return 0;
}
