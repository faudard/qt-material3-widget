#include <QApplication>
#include <QDebug>
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
        || !ui.splitButton || !ui.loadingIndicator || !ui.adaptiveShell
        || !ui.tooltip || !ui.badge) {
        qCritical() << "designer smoke: missing widget instance";
        return 1;
    }

    if (ui.slider->parentWidget() != &form
        || ui.slider->orientation() != Qt::Vertical
        || ui.slider->value() != 37) {
        qCritical() << "designer smoke: slider mismatch" << ui.slider->orientation() << ui.slider->value();
        return 2;
    }
    if (ui.outlinedTextField->text() != QString()
        || ui.outlinedTextField->placeholderText() != QStringLiteral("name@example.com")
        || ui.outlinedTextField->maxLength() != 120) {
        qCritical() << "designer smoke: outlined text field mismatch"
                    << ui.outlinedTextField->text()
                    << ui.outlinedTextField->placeholderText()
                    << ui.outlinedTextField->maxLength();
        return 3;
    }
    if (ui.dateField->displayFormat() != QStringLiteral("dd/MM/yyyy")
        || !ui.dateField->isClearable()) {
        qCritical() << "designer smoke: date field mismatch"
                    << ui.dateField->displayFormat() << ui.dateField->isClearable();
        return 4;
    }
    if (ui.rangeSlider->lowerValue() != 20 || ui.rangeSlider->upperValue() != 80) {
        qCritical() << "designer smoke: range slider mismatch"
                    << ui.rangeSlider->lowerValue() << ui.rangeSlider->upperValue();
        return 5;
    }
    if (ui.topAppBar->title() != QStringLiteral("Designer smoke")
        || ui.bottomAppBar->title() != QStringLiteral("Designer actions")) {
        qCritical() << "designer smoke: app bar mismatch"
                    << ui.topAppBar->title() << ui.bottomAppBar->title();
        return 6;
    }
    if (ui.buttonGroup->buttonLabels()
            != QStringList({QStringLiteral("Day"), QStringLiteral("Week"), QStringLiteral("Month")})
        || ui.buttonGroup->currentIndex() != 1) {
        qCritical() << "designer smoke: button group mismatch"
                    << ui.buttonGroup->buttonLabels() << ui.buttonGroup->currentIndex();
        return 7;
    }
    if (ui.navigationBar->destinationLabels()
            != QStringList({QStringLiteral("Home"), QStringLiteral("Search"), QStringLiteral("Profile")})
        || ui.navigationBar->currentIndex() != 1) {
        qCritical() << "designer smoke: navigation bar mismatch"
                    << ui.navigationBar->destinationLabels() << ui.navigationBar->currentIndex();
        return 8;
    }
    if (ui.segmentedList->itemLabels()
            != QStringList({QStringLiteral("Personal"), QStringLiteral("Work"), QStringLiteral("Archive")})
        || !ui.segmentedList->expressive()) {
        qCritical() << "designer smoke: segmented list mismatch"
                    << ui.segmentedList->itemLabels() << ui.segmentedList->expressive();
        return 9;
    }
    if (ui.splitButton->text() != QStringLiteral("Create")
        || !ui.splitButton->expressive()
        || ui.loadingIndicator->indicatorSize() != 44
        || ui.loadingIndicator->isActive()) {
        qCritical() << "designer smoke: expressive/loading mismatch"
                    << ui.splitButton->text() << ui.splitButton->expressive()
                    << ui.loadingIndicator->indicatorSize() << ui.loadingIndicator->isActive();
        return 10;
    }
    if (ui.adaptiveShell->automaticDensity()
        || ui.adaptiveShell->supportingPaneWidth() != 360) {
        qCritical() << "designer smoke: adaptive shell mismatch"
                    << ui.adaptiveShell->automaticDensity()
                    << ui.adaptiveShell->supportingPaneWidth();
        return 11;
    }
    if (ui.tooltip->text() != QStringLiteral("Designer tooltip")
        || ui.tooltip->showDelay() != 250
        || ui.badge->count() != 7
        || ui.badge->maximum() != 99
        || ui.badge->isDot()) {
        qCritical() << "designer smoke: Designer 3.0 promoted widgets mismatch"
                    << ui.tooltip->text() << ui.tooltip->showDelay()
                    << ui.badge->count() << ui.badge->maximum() << ui.badge->isDot();
        return 12;
    }

    return 0;
}
