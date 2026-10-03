#include <QApplication>
#include <QWidget>
#include "ui_designer_smoke.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QWidget form;
    Ui::DesignerSmokeForm ui;
    ui.setupUi(&form);
    if (!ui.filledButton || !ui.outlinedTextField || !ui.card || !ui.table)
        return 1;
    return 0;
}
