#include <QApplication>
#include <QColor>
#include <QString>
#include <QPushButton>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/native/qtmaterialnativeadapter.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QtMaterial::ThemeBuilder builder;
    const QtMaterial::Theme theme =
        builder.buildLightFromSeed(QColor(QStringLiteral("#6750A4")));

    QtMaterial::QtMaterialFilledButton button(QStringLiteral("Consumer contract"));
    button.resize(180, 48);

    if (theme.isDark()) {
        return 1;
    }

    // Migration trial: a native application retains its control, connection
    // and original style when opting into Material rendering and rolling back.
    QPushButton native(QStringLiteral("Legacy action"));
    const auto* originalStyle = native.style();
    int clicks = 0;
    QObject::connect(&native, &QPushButton::clicked, &native, [&clicks] { ++clicks; });
    using Adapter = QtMaterial::QtMaterialNativeAdapter;
    if (!Adapter::apply(&native) || !Adapter::isApplied(&native)) {
        return 2;
    }
    native.click();
    if (clicks != 1 || !Adapter::remove(&native) || Adapter::isApplied(&native)) {
        return 3;
    }
    native.click();
    if (clicks != 2 || native.style() != originalStyle) {
        return 4;
    }
    return 0;
}
