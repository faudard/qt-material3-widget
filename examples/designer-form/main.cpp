#include <QApplication>
#include <QStandardItemModel>
#include <QWidget>

#include "ui_designerform.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QWidget window;
    Ui::DesignerForm ui;
    ui.setupUi(&window);

    ui.categoryCombo->addItems({
        QStringLiteral("Design"),
        QStringLiteral("Development"),
        QStringLiteral("Documentation"),
    });

    auto* model = new QStandardItemModel(3, 3, &window);
    model->setHorizontalHeaderLabels({
        QStringLiteral("Component"),
        QStringLiteral("Source"),
        QStringLiteral("Status"),
    });
    model->setData(model->index(0, 0), QStringLiteral("Text Field"));
    model->setData(model->index(0, 1), QStringLiteral(".ui"));
    model->setData(model->index(0, 2), QStringLiteral("Ready"));
    model->setData(model->index(1, 0), QStringLiteral("Range Slider"));
    model->setData(model->index(1, 1), QStringLiteral(".ui"));
    model->setData(model->index(1, 2), QStringLiteral("Ready"));
    model->setData(model->index(2, 0), QStringLiteral("Table"));
    model->setData(model->index(2, 1), QStringLiteral("Model/View"));
    model->setData(model->index(2, 2), QStringLiteral("Ready"));
    ui.previewTable->setModel(model);

    QObject::connect(ui.saveButton, &QAbstractButton::clicked, [&ui]() {
        ui.progress->setValue(1.0);
        ui.statusCard->setBodyText(QStringLiteral("The form was authored in Qt Designer and is running with Qt Material 3 widgets."));
    });

    window.resize(880, 820);
    window.show();
    return app.exec();
}
