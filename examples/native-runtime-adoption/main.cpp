#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QIntValidator>
#include <QMenu>
#include <QPushButton>
#include <QStandardItemModel>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/native/qtmaterialnativeadapter.h"
#include "ui_adoptionform.h"

int main(int argc, char** argv)
{
    QApplication application(argc, argv);
    QWidget window;
    Ui::NativeAdoptionForm ui;
    ui.setupUi(&window);

    // App ownership is unchanged: native model, delegate, validator and
    // actions are not transferred to the Material adaptation controller.
    auto* categories = new QStandardItemModel(&window);
    categories->appendRow(new QStandardItem(QStringLiteral("Design")));
    categories->appendRow(new QStandardItem(QStringLiteral("Engineering")));
    categories->appendRow(new QStandardItem(QStringLiteral("Operations")));
    ui.categoryCombo->setModel(categories);
    ui.ageEdit->setValidator(new QIntValidator(0, 120, &window));
    ui.ageEdit->setClearButtonEnabled(true);

    auto* menu = new QMenu(ui.moreButton);
    menu->addAction(QStringLiteral("Inspect native menu"));
    ui.moreButton->setMenu(menu);
    ui.moreButton->setPopupMode(QToolButton::InstantPopup);

    using Adapter = QtMaterial::QtMaterialNativeAdapter;
    Adapter::Options defaults;
    defaults.density = QtMaterial::Density::Comfortable;
    defaults.buttonVariant = QtMaterial::ButtonVariant::Outlined;
    // Only .ui controls marked qtm3MaterialAdapt=true are adapted.
    Adapter::watch(&window, Adapter::WatchPolicy::DeclaredOnly, defaults);

    QObject::connect(ui.saveButton, &QPushButton::clicked, &window, [&ui]() {
        ui.progressBar->setValue(qMin(100, ui.progressBar->value() + 25));
        ui.statusLabel->setText(QStringLiteral("Saved — native clicked() signal"));
    });
    QObject::connect(
        ui.excludeButton, &QCheckBox::toggled, &window,
        [&ui](bool checked) {
            Adapter::setOptOut(ui.saveButton, checked);
        });

    // Exercise discovery after construction without rewriting the .ui file.
    auto* lateButton = new QPushButton(
        QStringLiteral("Dynamically inserted native button"), &window);
    lateButton->setProperty(Adapter::adaptPropertyName(), true);
    lateButton->setProperty(
        "qtm3MaterialVariant", QStringLiteral("filled-tonal"));
    ui.verticalLayout->addWidget(lateButton);
    QObject::connect(
        lateButton, &QPushButton::clicked, &window, [&ui]() {
            ui.statusLabel->setText(
                QStringLiteral("Runtime-inserted control still emits clicked()"));
        });

    window.resize(540, 620);
    window.show();
    const int exitCode = application.exec();
    Adapter::unwatch(&window);
    return exitCode;
}
