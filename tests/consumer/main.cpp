#include <QApplication>
#include <QString>

#include <qtmaterial/widgets/buttons/qtmaterialtextbutton.h>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QtMaterial::QtMaterialTextButton button;
    button.setText(QStringLiteral("Consumer OK"));

    return 0;
}