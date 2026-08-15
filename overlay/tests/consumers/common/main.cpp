#include <QApplication>
#include <QColor>
#include <QString>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    QtMaterial::QtMaterialFilledButton button(QStringLiteral("Consumer contract"));
    button.resize(180, 48);

    return theme.isDark() ? 1 : 0;
}
