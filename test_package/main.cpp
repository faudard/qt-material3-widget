#include <QApplication>
#include <QString>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QtMaterial::QtMaterialFilledButton button(QStringLiteral("Conan consumer"));
    button.resize(180, 48);
    return button.text().isEmpty() ? 1 : 0;
}
