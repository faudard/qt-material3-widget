#include <QApplication>
#include <QString>

#include <qtmaterial/widgets/buttons/qtmaterialtextbutton.h>

#include <qtmaterial/testing/qtmaterialautomation.h>
#include <qtmaterial/integration/qtmaterialpaletteadapter.h>
#include <qtmaterial/tooling/qtmaterialcomponentregistry.h>
int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QtMaterial::QtMaterialTextButton button;
    button.setText(QStringLiteral("Consumer OK"));
    QtMaterial::QtMaterialAutomation::setTestId(
        &button, QStringLiteral("installed-consumer"));

    return 0;
}