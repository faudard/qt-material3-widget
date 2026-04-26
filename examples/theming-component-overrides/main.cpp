#include <QCoreApplication>
#include <QColor>
#include <QJsonDocument>
#include <QTextStream>

#include "qtmaterial/theme/qtmaterialcomponenttokens.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"

using namespace QtMaterial;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    Q_UNUSED(app);

    ThemeOptions options;
    options.sourceColor = QColor(QStringLiteral("#00639B"));
    options.mode = ThemeMode::Light;

    ThemeBuilder builder;
    Theme theme = builder.build(options);

    ComponentTokenOverride buttonFamily;
    buttonFamily.density.insert(DensityRole::Default, 2);
    buttonFamily.iconSizes.insert(IconSizeRole::Medium, 22);
    buttonFamily.custom.insert(QStringLiteral("minHeight"), 44);
    theme.componentOverrides().setOverride(QStringLiteral("button"), buttonFamily);

    ComponentTokenOverride filledButton;
    filledButton.colors.insert(ColorRole::Primary, QColor(QStringLiteral("#004A77")));
    filledButton.colors.insert(ColorRole::OnPrimary, QColor(QStringLiteral("#FFFFFF")));
    filledButton.custom.insert(QStringLiteral("cornerRole"), QStringLiteral("large"));
    theme.componentOverrides().setOverride(QStringLiteral("button.filled"), filledButton);

    ComponentTokenOverride textField;
    textField.density.insert(DensityRole::Compact, -3);
    textField.custom.insert(QStringLiteral("supportingTextVisible"), true);
    theme.componentOverrides().setOverride(QStringLiteral("textField.outlined"), textField);

    QTextStream out(stdout);
    out << "Theme with component-local overrides:" << Qt::endl;
    out << QString::fromUtf8(ThemeSerializer::toJson(theme, QJsonDocument::Indented)) << Qt::endl;
    return 0;
}
