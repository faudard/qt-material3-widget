#include <QApplication>

#include "themepreviewwindow.h"
#include "qtmaterial/theme/qtmaterialsystemtheme.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QtMaterial::SystemTheme::instance().setPreference(QtMaterial::ThemeModePreference::FollowSystem);
    QtMaterial::SystemTheme::instance().setAutoApplyToThemeManager(true);

    ThemePreviewWindow window;
    window.resize(1240, 860);
    window.show();
    return app.exec();
}
