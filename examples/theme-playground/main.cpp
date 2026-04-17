#include <QApplication>
#include "themepreviewwindow.h"
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    ThemePreviewWindow window;
    window.resize(420, 180);
    window.show();
    return app.exec();
}
